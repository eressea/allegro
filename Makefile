CPP=g++
LIBS=`pkg-config --libs allegro_font-5.0 allegro_image-5.0`
CFLAGS=-g -std=c++11 -Wall -Werror `pkg-config --cflags allegro-5.0`

allegro: main.o
	$(CPP) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CFLAGS) -o $@ -c $^

clean:
	rm -f *.o allegro
