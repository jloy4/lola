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
#include <cstring>
#include <sstream>
#include <arpa/inet.h>
#include "tsl2561.h"

#define PORT 49890

class LolaLight {
	public:
		LolaLight();

		void setPins(int oled_pin_, int touch_pin_);
		void setLight(int intensity);
		int pressed();
		void waitForRelease();
		void lightWrite();
		void updateState();
		void updateTimeSpanUp();
		void touchListen();
		int homekitListen();
		void lightSensor();
		void start();

	private:
		int oled_pin;
		int touch_pin;
		int sensitivity = 5;
		double time_up = 0;
		std::vector<int> light_status = std::vector<int>(2);
		std::vector<int> val = std::vector<int>(2);
		int val_temp = 0;
		//std::vector<int> t_state = std::vector<int>(2);
		//std::chrono::high_resolution_clock::time_point t_up;
		std::chrono::high_resolution_clock::time_point t_manual;

		// Light sensor
		int address = 0x39;
		const char *i2c_device = "/dev/i2c-1";
		long lux = 0;

		//Homekit
		class ReuseableListener : public sf::TcpListener {
			public:
				void reuse() {
					char reuse = 1;
					setsockopt( getHandle(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
				}
		};
};

#endif
