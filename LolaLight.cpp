#include "LolaLight.h"

using namespace std;

LolaLight::LolaLight() : sensitivity(10), time_up(0), val_temp(0) {}   

void LolaLight::setPins(int oled_pin_, int touch_pin_) {
	// initialize pins and values
	oled_pin = oled_pin_;
	touch_pin = touch_pin_;
	val[0] = val[1] = 0;
	light_status[0] = light_status[1] = 0;
	
	// configure pins
	wiringPiSetup();
	softPwmCreate(oled_pin_,0,100);
	pinMode(touch_pin_,INPUT);
	setLight(0);
}

void LolaLight::setLight(int intensity) {
	if (intensity < 0) {intensity = 0;}
	else if (intensity > 100) {intensity = 100;}

	light_status[0] = light_status[1];
	if (intensity > 0) {light_status[1]=1;}
	else {light_status[1]=0;}
	
	val[0] = val[1]; 
	val[1] = intensity;
	//int val_ = m_light_values[100 - intensity_];
	int temp1 = 100 - val[0];
	int temp2 = 100 - intensity;
	if (temp1 > temp2) {
		for (int i=temp1;i>=temp2;i--) {
			softPwmWrite(oled_pin, i);
			delay(2);
		}
	}
	else {
		for (int i=temp1;i<=temp2;i++) {
			softPwmWrite(oled_pin, i);
			delay(2);
		}
	}
}

void print(string str) {
	std::cout << str << std::endl;
}

//void LolaLight::lightWrite() {
//	cout << "handling light intensity" << endl;
//	int wait_time = 300;
//	while(1) {
//		//cout << time_up << '\n';
//		int int_temp = val[1];
//		if (val[1] == 0 && time_up > sensitivity) {
//			delay(wait_time);
//			if (time_up > wait_time) {
//				std::cout << "Increasing brightness" << std::endl;
//				while (time_up > wait_time && val[1] < 100) {
//					int_temp++;
//					setLight(int_temp);
//					delay(10);
//				}
//				print("security on");
//				while (time_up > wait_time) {
//					delay(50);
//				}
//				print("security off");
//			} else {
//				std::cout << "full brightness" << std::endl;
//				//val[1] = 100;
//				setLight(100);
//			}
//		}
//		else if (val[1] > 0 && time_up > sensitivity) {
//			delay(70);
//			std::cout << "time_up: " << time_up << endl;
//			if (time_up < 70) {
//				double temp = time_up;
//				std::cout << "reducing brightness" << std::endl;
//				while (time_up > temp && val[1] > 0) {
//					int_temp--;
//					setLight(int_temp);
//					delay(10);
//				}
//				std::cout << "time_up: " << time_up << endl;
//				goto end_light;
//			}
//			delay(wait_time-70);
//			if (time_up > wait_time) {
//				std::cout << "increasing brightness" << std::endl;
//				while (time_up > wait_time && val[1] < 100) {
//					//val[1]++;
//					int_temp++;
//					setLight(int_temp);
//					delay(10);
//				}
//			} else {
//				std::cout << "light off" << std::endl;
//				//val[1] = 0;
//				setLight(0);
//				light_status[1] = 0;
//			}
//		}
//		end_light:
//
//		//if (val[0] != val[1]) setLight(val[1]);
//		//cout << val[1] << '\n';
//		delay(100);
//	}
//}

int LolaLight::pressed() {
	return digitalRead(touch_pin);
}

void LolaLight::waitForRelease() {
	while (pressed()) {
		usleep(1);
	}
	usleep(1000*1000);
}

void LolaLight::lightWrite() {
	while (1) {
		int int_temp = val[1];
		if (pressed() && val[1]==0) {
			usleep(3*100*1000);
			t_manual = std::chrono::high_resolution_clock::now();
			if (pressed()) {
				print("light up");
				while (pressed() && val[1]<100) {
					int_temp++;
					setLight(int_temp);
					usleep(10*1000);
				}
				waitForRelease();
			} else {
				print("light on");
				setLight(100);
			}
		}
		else if (pressed() && val[1]>0) {
			usleep(150*1000);
			if (!pressed()) {
				usleep(50*1000);
				t_manual = std::chrono::high_resolution_clock::now();
				if (pressed()) {
					print("light down");
					while (pressed() && val[1]>0) {
						int_temp--;
						setLight(int_temp);
						usleep(10*1000);
					}
					waitForRelease();
				} else {
					print("light off");
					setLight(0);
				}
			}
		}
		usleep(1*1000);
	}
}
//void LolaLight::updateState() {
//	t_state[0] = t_state[1];
//	t_state[1] = digitalRead(touch_pin);
//}

//void LolaLight::updateTimeSpanUp() {
//	if (t_state[1] == 1 && t_state[1] != t_state[0]) {
//		t_up = std::chrono::high_resolution_clock::now();
//	}
//	if (t_state[1] == 0) {
//		t_up = std::chrono::high_resolution_clock::now();
//	}
//	auto t_now = std::chrono::high_resolution_clock::now();
//	chrono::duration<double,std::milli> time_span_up = t_now - t_up;
//	time_up = time_span_up.count();
//	//cout << time_up << endl;
//}

//void LolaLight::touchListen() {
//	cout << "listening to sensor" << endl;
//	while (1) {
//		this->updateState();
//		this->updateTimeSpanUp();
//		delay(5);
//	}
//}

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

		// Wait for a connection
		sf::TcpSocket socket;
		if (listener.accept(socket) != sf::Socket::Done)
			return 0;
		std::cout << "Client connected: " << socket.getRemoteAddress() << std::endl;
		t_manual = std::chrono::high_resolution_clock::now();

		// Receive a message back from the client
		char in[128];
		std::size_t received;
		if (socket.receive(in, sizeof(in), received) != sf::Socket::Done)
			return 0;
		std::cout << "Answer received from the client: \"" << in << "\"" << std::endl;

		// Send a message to the connected client
		char out[] = "HTTP/1.1 200 OK\n\r\n\r";


		// Interpreting answers
		string req(in);

		if (req.find("lola/status")>0 && req.find("lola/status")<4000) {
			char status_string[32];
			sprintf(status_string, "%d", light_status[1]);
			std::strcat(out, status_string);
		}

		if (req.find("lola/bright_status")>0 && req.find("lola/bright_status")<4000) {
			char val_string[32];
			sprintf(val_string, "%d", val[1]);
			std::strcat(out, val_string);
		}

		if (req.find("lola/on")>0 && req.find("lola/on")<4000) {
			if (light_status[0]!=light_status[1] && val[1]!=100 && val[0]==100) {
				if (val_temp > 0) {
					setLight(val_temp);
				} else {
					setLight(100);
				}
			}
		}

		if (req.find("lola/off")>0 && req.find("lola/off")<4000) {
			val_temp = val[1];
			setLight(0);
		}

		if (req.find("lola/set")>0 && req.find("lola/set")<4000) {
			string temp = req.substr(14,17);
			int val_ = std::stoi(temp);
			cout << val_ << endl;
			setLight(val_);
		}

		if (socket.send(out, sizeof(out)) != sf::Socket::Done)
			return 0;
		std::cout << "Message sent to the client: \"" << out << "\"" << std::endl;

		cout << "0: " << light_status[0] << " | 1: " << light_status[1] << endl;
		cout << "val_0: " << val[0] << " | val_1: " << val[1] << endl;

		usleep(1*1000);
	}  
}

void LolaLight::lightSensor() {
	void *tsl = tsl2561_init(address, i2c_device);
	tsl2561_enable_autogain(tsl);
	tsl2561_set_integration_time(tsl, TSL2561_INTEGRATION_TIME_13MS);

	if(tsl == NULL){ // check if error is present
		std::cout << "no TSL2561 detected ... Check your wiring or I2C ADDR!" << std::endl;
	} 
	long thres_up = 300;
	long thres_down = 30;
	int gap = 20;
	bool up = false;
	bool down = false;

	while (1) {
		//std::chrono::duration<double, std::milli> time_manual = t_manual;
		auto t_now = chrono::high_resolution_clock::now();
		chrono::duration<double,std::milli> time_manual = t_now - t_manual;

		// if no manual control after 1 min, correct intensity
		if (time_manual.count() > 20*1000) {
			lux = tsl2561_lux(tsl);

			if (lux < thres_down && !up && val[1] != 0) {
				setLight(val[1]+gap);
				up = true;
			} 
			else if (lux > thres_up && !down) {
				setLight(val[1]-gap);
				down = true;
			} 
			else if (lux > thres_down && lux < thres_up && up) {
				setLight(val[1]-gap);
				up = false;
			}
			else if (lux > thres_down && lux < thres_up && down) {
				setLight(val[1]+gap);
				down = false;
			}
		}
			
		
		cout << up << " | " << down << " | " << lux << endl;
	}

	//tsl2561_close(tsl);
	//i2c_device = NULL;

}
void LolaLight::start() {
	//thread touch(&LolaLight::touchListen,this);
	thread light(&LolaLight::lightWrite,this);
	thread homekit(&LolaLight::homekitListen,this);
	thread light_sensor(&LolaLight::lightSensor,this);

	//touch.join();
	light.join();
	homekit.join();
	light_sensor.join();
}
