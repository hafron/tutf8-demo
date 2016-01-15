objs = decode.o format_byte.o

CFLAGS = -Wall -O2 -c
LDFLAGS = 

CFLAGS += `sdl2-config --cflags`
LDFLAGS += `sdl2-config --libs` -lSDL2_ttf

all: tutf8-demo

tutf8-demo: $(objs)
	gcc $(objs) -o tutf8-demo $(LDFLAGS)

%.o : %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -rf tutf8-demo *.o

