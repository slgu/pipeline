all:
	g++ -O2 *.cpp -L /usr/lib/x86_64-linux-gnu/ -lglut -lGL -lGLU -lGLEW -lm  -o glrender
mac:
	g++ *.cpp -framework OpenGL -framework GLUT -o glrender
clean:
	rm -rf *.o glrender
