CFLAGS = -std=c99 -Wall -Wextra -g -pedantic
CSANI = -fsanitize=address,undefined
CC = clang


# .o
core.o: core.c core.h
	$(CC) -c $(CFLAGS) $(CSANI) core.c

cli.o: cli.c cli.h
	$(CC) -c $(CFLAGS) $(CSANI) cli.c

gui.o: gui.c gui.h
	$(CC) -c $(CFLAGS) $(CSANI) gui.c

main.o: main.c
	$(CC) -c $(CFLAGS) $(CSANI) main.c



# fiare les execultables
main_only_core: main.o core.o
	$(CC) $(CFLAGS) $(CSANI) -o main_only_core main.o core.o

main_textuel: main.o core.o cli.o
	$(CC) $(CFLAGS) $(CSANI) -o main_textuel main.o core.o cli.o

main_graphique: main.o core.o gui.o
	$(CC) $(CFLAGS) $(CSANI) -o main_graphique main.o core.o gui.o


# les exécutions
clean:
	rm -f main.o core.o cli.o gui.o main_only_core main_textuel main_graphique

run_only_core: main_only_core
	./main_only_core

run_textuel: main_textuel
	./main_textuel

run_graphique: main_graphique
	./main_graphique