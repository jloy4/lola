#ifndef DEF_LOLA_LIGHT
#define DEF_LOLA_LIGHT

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
#include <SFML/Network.hpp>
#include <sys/socket.h>
#include <string>
#include <sstream>

#define PORT 49890

using namespace std;

class LolaLight {
	public:
		LolaLight();

		void setPins(int oled_pin_, int touch_pin_);
		void setLight(int intensity);
		void lightWrite();
		void updateState();
		void updateTimeSpanUp();
		void touchListen();
		int homekitListen();
		void start();

	private:
		int oled_pin;
		int touch_pin;
		int sensitivity = 5;
		double time_up = 0;
		vector<int> light_status = vector<int>(2);
		vector<int> val = vector<int>(2);
		int val_temp = 0;
		vector<int> t_state = vector<int>(2);
	  chrono::high_resolution_clock::time_point t_up;

		class ReuseableListener : public sf::TcpListener {
			public:
				void reuse() {
					char reuse = 1;
					setsockopt( getHandle(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
				}
		};
};

#endif
