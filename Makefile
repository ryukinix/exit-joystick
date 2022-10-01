all:
	g++ -I /usr/include/SDL2 main.cpp -o exit-joystick -lSDL2

run: all
	./exit-joystick
