#include "LolaLight.h"

using namespace std;

LolaLight::LolaLight() : sensitivity(5), time_up(0), val_temp(0) {}   

void LolaLight::setPins(int oled_pin_, int touch_pin_) {
	// initialize pins and values
	oled_pin = oled_pin_;
	touch_pin = touch_pin_;
	val[0] = 0;
	val[1] = 0;
	light_status[0] = light_status[1] = 0;
	// configure pins
	wiringPiSetup();
	softPwmCreate(oled_pin_,0,100);
	pinMode(touch_pin_,INPUT);
	setLight(0);
}

void LolaLight::setLight(int intensity) {
	if (intensity >= 0 || intensity <= 100) {
		//int val_ = m_light_values[100 - intensity_];
		int temp = 100 - intensity;
		softPwmWrite(oled_pin, temp);
	}
}

void LolaLight::lightWrite() {
	cout << "handling light intensity" << endl;
	while(1) {
		val[0] = val[1];

		//cout << time_up << '\n';
		if (val[1] == 0 && time_up > sensitivity) {
			delay(300);
			if (time_up > 300) {
				while (time_up > 300 && val[1] < 100) {
					val[1]++;
					setLight(val[1]);
					delay(20);
				}
			} else {val[1] = 100;}
		}
		else if (val[1] > 0 && time_up > sensitivity) {
			delay(300);
			if (time_up > 300) {
				while (time_up > 300 && val[1] < 100) {
					val[1]++;
					setLight(val[1]);
					delay(20);
				}
			} else {val[1] = 0;}
		}

		if (val[0] != val[1]) setLight(val[1]);
		//cout << val[1] << '\n';
		delay(50);
	}
}

void LolaLight::updateState() {
	t_state[0] = t_state[1];
	t_state[1] = digitalRead(touch_pin);
}

void LolaLight::updateTimeSpanUp() {
	if (t_state[1] == 1 && t_state[1] != t_state[0]) {
		t_up = std::chrono::high_resolution_clock::now();
	}
	if (t_state[1] == 0) {
		t_up = std::chrono::high_resolution_clock::now();
	}
	auto t_now = std::chrono::high_resolution_clock::now();
	chrono::duration<double,std::milli> time_span_up = t_now - t_up;
	time_up = time_span_up.count();
}

void LolaLight::touchListen() {
	cout << "listening to sensor" << endl;
	while (1) {
		this->updateState();
		this->updateTimeSpanUp();
		delay(50);
	}
}

int LolaLight::homekitListen() {
	// Create a server socket to accept new connections
	ReuseableListener listener;
	//listener.setBlocking(false);

	// Listen to the given port for incoming connections
	if (listener.listen(PORT) != sf::Socket::Done)
		return 0;
	std::cout << "Server is listening to port " << PORT << ", waiting for connections... " << std::endl;

	listener.reuse();

	while(1) {
		val[0] = val[1];
		light_status[0] = light_status[1];

		// Wait for a connection
		sf::TcpSocket socket;
		if (listener.accept(socket) != sf::Socket::Done)
			return 0;
		std::cout << "Client connected: " << socket.getRemoteAddress() << std::endl;

		// Send a message to the connected client
		const char out[] = "HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r";
		if (socket.send(out, sizeof(out)) != sf::Socket::Done)
			return 0;
		std::cout << "Message sent to the client: \"" << out << "\"" << std::endl;

		// Receive a message back from the client
		char in[128];
		std::size_t received;
		if (socket.receive(in, sizeof(in), received) != sf::Socket::Done)
			return 0;
		std::cout << "Answer received from the client: \"" << in << "\"" << std::endl;

		// Interpreting answers
		string req(in);
		cout << req.find("lola/on") << endl;

		if (req.find("lola/status")>0 && req.find("lola/status")<4000) {
			//const char c[] = '0'+light_status[1];
			//std::itoa(light_status[1],c,10);
			ostringstream status;
			status << light_status[1];
			socket.send(status, sizeof(status));
			cout << "status: " << status << endl;
		}

		if (req.find("lola/bright_status")>0 && req.find("lola/bright_status")<4000) {
			//const char c[] = '0'+val[1];
			//std::itoa(val[1],c,10);
			ostringstream bright;
			bright << val[1];
			socket.send(bright, sizeof(bright));
			cout << "brightness: " << bright << endl;
		}

		if (req.find("lola/on")>0 && req.find("lola/on")<4000) {
			if (val_temp > 0) {
				val[1]=val_temp;
			} else {
				val[1]=100;
			}
			light_status[1]=1;
		}

		if (req.find("lola/off")>0 && req.find("lola/off")<4000) {
			val_temp = val[1];
			val[1]=0;
			light_status[1]=0;
		}

		if (req.find("lola/set")>0 && req.find("lola/set")<4000) {
			string temp = req.substr(14,17);
			cout << "THE VALUE IS: " << temp << endl;
			val[1] = std::stoi(temp);
		}

		cout << "0: " << light_status[0] << " | 1: " << light_status[1] << endl;
		cout << "val_0: " << val[0] << " | val_1: " << val[1] << endl;
		if ((light_status[0]==light_status[1] && val[1]==100 && val[0]!=100)) {
			goto finished;
		} else {
			setLight(val[1]);
		}

finished:
		usleep(50000);
	}  
}

void LolaLight::start() {
	//return thread([this] {this.touchListen{};});
	thread touch(&LolaLight::touchListen,this);
	thread light(&LolaLight::lightWrite,this);
	thread homekit(&LolaLight::homekitListen,this);

	touch.join();
	light.join();
	homekit.join();
}
