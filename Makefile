CC ?= gcc
CPP ?= g++
LIBS+=`pkg-config --libs allegro_font-5.0 allegro_image-5.0`
CFLAGS+=-g -Wall -Werror `pkg-config --cflags allegro-5.0`
CPPFLAGS+=-std=c++11 $(CFLAGS)

allegro: main.o canvas.o
	$(CPP) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CFLAGS) -o $@ -c $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f *.o allegro
