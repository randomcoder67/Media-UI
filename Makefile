normal:
	gcc `pkg-config --cflags gtk+-3.0` gamesUI.c gamesJSON.c main.c `pkg-config --libs gtk+-3.0` -I/usr/include/json-c -L/usr/lib -ljson-c -g -o gamesUI

check:
	gcc `pkg-config --cflags gtk+-3.0` gamesUI.c gamesJSON.c main.c `pkg-config --libs gtk+-3.0` -I/usr/include/json-c -L/usr/lib -ljson-c -g -o gamesUI -Wall -Wextra

clean:
	rm gamesUI.o gamesJSON.o main.o gamesUI
