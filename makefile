vdl: vdl.o logger.o sensehat.o serial.o nmea.o dlgps.o loggermqtt.o dlfirmata.o
	g++ -g -o vdl vdl.o logger.o sensehat.o serial.o nmea.o dlgps.o loggermqtt.o dlfirmata.o -lm -lRTIMULib -lpaho-mqtt3c -lboost_thread -lboost_system -lpthread -lopenFrameworksArduinoD
vdl.o: vdl.cpp vdl.h logger.h sensehat.h serial.h nmea.h dlgps.h
	g++ -g -c vdl.cpp
logger.o: logger.cpp logger.h sensehat.h serial.h nmea.h dlgps.h loggermqtt.h
	g++ -g -c logger.cpp
SenseHat.o: sensehat.cpp sensehat.h
	g++ -g -c sensehat.cpp
serial.o: serial.cpp serial.h
	g++ -g -c serial.cpp
dlgps.o: dlgps.cpp dlgps.h
	g++ -g -c dlgps.cpp
nmea.o: nmea.cpp nmea.h
	g++ -g -c nmea.cpp
loggermqtt.o: loggermqtt.cpp loggermqtt.h
	g++ -g -c loggermqtt.cpp
dlfirmata.o: dlfirmata.cpp dlfirmata.h
	g++ -g -c dlfirmata.cpp
clean:
	touch *
	rm *.o
