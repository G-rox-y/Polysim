all: compile link

compile:
	g++ -I src/include -c main.cpp

link:
	g++ main.o -o Polysim -L src/lib -l sfml-graphics -l sfml-window -l sfml-system -static -static-libgcc -static-libstdc++