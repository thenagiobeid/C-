CFLAGS = -I ./include
##LIB    = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: asteroids

asteroids: asteroids.cpp log.cpp timers.cpp danielM.cpp moathN.cpp nagiO.cpp
	g++ $(CFLAGS) asteroids.cpp log.cpp danielM.cpp moathN.cpp nagiO.cpp timers.cpp lab3sget.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -oasteroids -lssl -lcrypto

clean:
	rm -f asteroids
	rm -f *.o

