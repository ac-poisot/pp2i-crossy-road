CFLAGS = -Wall -Wextra -g -pedantic -std=gnu99
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

core_test.o: core_test.c core.h
	$(CC) -c $(CFLAGS) $(CSANI) core_test.c


# create executables
main_only_core: core.o
	$(CC) $(CFLAGS) $(CSANI) -o main_only_core core.o

main_cli: core.o cli.o
	$(CC) $(CFLAGS) $(CSANI) -o main_cli core.o cli.o -lncurses

main_graphics: core.o gui.o
	$(CC) $(CFLAGS) $(CSANI) -o main_graphics core.o gui.o -lSDL2  -lSDL2_image -lSDL2 -lSDL2_ttf

core_test: core.o core_test.o
	$(CC) $(CFLAGS) $(CSANI) -o core_test core.o core_test.o



# executions

clean:
	rm -f main.o core.o cli.o gui.o main_only_core main_cli main_graphics core_test core_test.o

run_only_core: main_only_core
	./main_only_core

run_cli: main_cli
	./main_cli

run_graphics: main_graphics
	./main_graphics

run_core_test: core_test
	./core_test

run_all_tests: core_test cli_test
	./core_test
	./main_cli