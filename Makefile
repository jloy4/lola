all :
	$(info Compiling...)
	g++ -Wall -g -std=c++11 main.cpp LolaLight.cpp -o lola -lwiringPi -lpthread -lcurl -lz -lsfml-network 
