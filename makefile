all:
	g++ -Wall -c funciones_e_d_f.cpp -lwiringPi
	g++ -Wall -c funciones_emisor.cpp -lwiringPi
	g++ -Wall -c emisor.cpp -lwiringPi
	g++ -Wall -o emisor funciones_e_d_f.o funciones_emisor.o emisor.o -lwiringPi

clean:
	rm *.o emisor

list:
	ls