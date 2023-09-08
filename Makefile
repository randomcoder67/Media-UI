normal:
	gcc `pkg-config --cflags gtk+-3.0` gamesUI.c gamesJSON.c main.c `pkg-config --libs gtk+-3.0` -I/usr/include/json-c -L/usr/lib -ljson-c -g -o gamesUI

install:
	cp gamesUI "$(HOME)/.local/bin/gamesUI"
	cp mediaUI.desktop "$(XDG_DATA_HOME)/applications/mediaUI.desktop"
	
uninstall:
	rm "$(HOME)/.local/bin/gamesUI"
	rm "$(XDG_DATA_HOME)/applications/mediaUI.desktop"

check:
	gcc `pkg-config --cflags gtk+-3.0` gamesUI.c gamesJSON.c main.c `pkg-config --libs gtk+-3.0` -I/usr/include/json-c -L/usr/lib -ljson-c -g -o gamesUI -Wall -Wextra

clean:
	rm gamesUI gamesUI.o gamesJSON.o main.o
