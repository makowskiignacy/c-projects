.PHONY: all clean

all: forests

forests: main.o parsujaca_wiersz.o przechowujaca.o interfejs.o
	gcc -Wall -Wextra -o $@ $^

parsujaca_wiersz.o: parsujaca_wiersz.c parsujaca_wiersz.h
	gcc -Wall -Wextra -c -o $@ $<

przechowujaca.o: przechowujaca.c przechowujaca.h
	gcc -Wall -Wextra -c -o $@ $<

interfejs.o: interfejs.c interfejs.h parsujaca_wiersz.h przechowujaca.h
	gcc -Wall -Wextra -c -o $@ $<

main.o: main.c interfejs.h
	gcc -Wall -Wextra -c -o $@ $<

clean:
	rm -f *.o main

