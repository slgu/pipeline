all:
	g++ -O2 *.cpp -L /usr/lib/x86_64-linux-gnu/ -lglut -lGL -lGLU -lGLEW -lm  -o myprog
mac:
	g++ *.cpp -framework OpenGL -framework GLUT -o myprog
clean:
	rm -rf *.o myprog
