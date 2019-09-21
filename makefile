all:
	g++ -c NSGA-2/*.hpp
	g++ -c Simulador\ CANbus/*.hpp
	g++ NSGA-2/*.cpp Simulador\ CANbus/*.cpp -o NSGA-2.bin -lm
