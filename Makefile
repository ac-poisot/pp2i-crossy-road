CFLAGS = -Wall -Wextra -g -pedantic
CSANI = -fsanitize=address,undefined
CC = clang


# .o
core.o: core.c core.h
	$(CC) -c $(CFLAGS) $(CSANI) core.c

cli.o: cli.c cli.h core.h
	$(CC) -c $(CFLAGS) $(CSANI) cli.c

gui.o: gui.c gui.h
	$(CC) -c $(CFLAGS) $(CSANI) gui.c

main.o: main.c
	$(CC) -c $(CFLAGS) $(CSANI) main.c



# create executables
main_only_core: core.o
	$(CC) $(CFLAGS) $(CSANI) -o main_only_core core.o

main_cli: core.o cli.o
	$(CC) $(CFLAGS) $(CSANI) -o main_cli core.o cli.o

main_graphics: core.o gui.o
	$(CC) $(CFLAGS) $(CSANI) -o main_graphics core.o gui.o

cli_test: cli.o core.o
	$(CC) $(CFLAGS) $(CSANI) -o main_cli cli.o core.o -lncurses


# executions

clean:
	rm -f main.o core.o cli.o gui.o main_only_core main_cli main_graphics

run_only_core: main_only_core
	./main_only_core

run_cli: cli_test
	./main_cli

run_graphics: main_graphics
	./main_graphics