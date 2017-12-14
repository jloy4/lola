LOLA=LolaLight
SENSOR=tsl2561

all:
	$(info Compiling...)
	g++ -Wall -c $(SENSOR).cpp -o $(SENSOR).o -lm
	g++ -Wall -g -std=c++11 $(SENSOR).o main.cpp $(LOLA).cpp -o lola -lwiringPi -lpthread -lz -lsfml-network 

clean:
	rm *.o > /dev/null 2>&1 &
