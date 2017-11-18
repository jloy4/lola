all :
	$(info Compiling...)
	g++ -Wall -g -std=c++11 lola_full.cpp -o lola_full -lwiringPi -lpthread -lcurl -lz -lsfml-network 
