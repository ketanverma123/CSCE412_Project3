CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17

all: loadbalancer

loadbalancer: main.o LoadBalancer.o WebServer.o Firewall.o Logger.o Config.o
	$(CXX) $(CXXFLAGS) -o loadbalancer main.o LoadBalancer.o WebServer.o Firewall.o Logger.o Config.o

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

LoadBalancer.o: src/LoadBalancer.cpp
	$(CXX) $(CXXFLAGS) -c src/LoadBalancer.cpp

WebServer.o: src/WebServer.cpp
	$(CXX) $(CXXFLAGS) -c src/WebServer.cpp

Firewall.o: src/Firewall.cpp
	$(CXX) $(CXXFLAGS) -c src/Firewall.cpp

Logger.o: src/Logger.cpp
	$(CXX) $(CXXFLAGS) -c src/Logger.cpp

Config.o: src/Config.cpp
	$(CXX) $(CXXFLAGS) -c src/Config.cpp

clean:
	rm -f loadbalancer *.o