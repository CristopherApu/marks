# Compilador C++
CXX = g++

# Flags del compilador
CXXFLAGS = -Wall -g -std=c++11 -lwiringPi

# Archivos objeto
OBJS = emisor.o receptor.o clock.o

# Nombre del ejecutable
APPNAME  = programa

# Regla para construir el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(APPNAME ) $(OBJS)

# Reglas para compilar los archivos objeto
emisor.o: emisor.cpp emisor.h
	$(CXX) $(CXXFLAGS) -c emisor.cpp

receptor.o: receptor.cpp receptor.h
	$(CXX) $(CXXFLAGS) -c receptor.cpp

clock.o: clock.cpp
	$(CXX) $(CXXFLAGS) -c clock.cpp

# Regla para limpiar los archivos objeto y el ejecutable
clean:
	rm -f $(OBJS) $(APPNAME )