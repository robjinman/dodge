NAME = demo
CC = g++
CFLAGS = -std=c++0x -Wall -g -DDEBUG -O3
INCL = -I../Dodge/include -I../Dodge/include/SDL
LIBS = -L../Dodge/lib -L/usr/lib -lDodge -lX11 -lGLESv2 -lEGL -lpnglite -lz -lBox2D
OBJS = main.o

all: $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LIBS)

$(OBJS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $(INCL) $< -o $@

clean:
	rm -f $(NAME)
	rm -f *.o
