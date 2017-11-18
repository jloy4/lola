#include <wiringPi.h>
#include <softPwm.h>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <ctime>
#include <ratio>

using namespace std;

class LolaLight {
	public:
		void setPins(int oled_pin_, int touch_pin_) {
			// initialize pins and values
			oled_pin = oled_pin_;
			touch_pin = touch_pin_;
			val[0] = 0;
			val[1] = 0;
			// configure pins
			wiringPiSetup();
			softPwmCreate(oled_pin_,0,100);
			pinMode(touch_pin_,INPUT);
			setLight(100);
		}

		void setLight(int intensity) {
		  if (intensity >= 0 || intensity <= 100) {
		    //int val_ = m_light_values[100 - intensity_];
		    int val = 100 - intensity;
		    softPwmWrite(oled_pin, val);
			}
		}

		void lightWrite() {
			while(1) {
				val[0] = val[1];
				
				cout << time_up << '\n';
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
				cout << val[1] << '\n';
				delay(50);
			}
		}

		void updateState() {
		  t_state[0] = t_state[1];
		  t_state[1] = digitalRead(touch_pin);
		}

		void updateTimeSpanUp() {
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

		void touchListen() {
		  while (1) {
		   this->updateState();
		   this->updateTimeSpanUp();
		   delay(50);
		  }
		}

		void touchThread() {
		  //return thread([this] {this.touchListen{};});
		  thread touch(&LolaLight::touchListen,this);
		  touch.join();
		}

		void lightThread() {
			thread light(&LolaLight::lightWrite,this);
			light.join();
		}

	private:
		int oled_pin;
		int touch_pin;
		int sensitivity = 5;
		int time_up = 0;
		std::vector<int> val = std::vector<int>(2);
		std::vector<int> t_state = std::vector<int>(2);
	  std::chrono::high_resolution_clock::time_point t_up;
};

int main(int argc, char * argv[]) {
	LolaLight lola;
	lola.setPins(1,0);
	lola.touchListen();
	lola.touchThread();

	return 0;
}
