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

minmax.o: minmax.c minmax.h list.h
	$(CC) -c $(CFLAGS) $(CSANI) minmax.c

main.o: main.c
	$(CC) -c $(CFLAGS) $(CSANI) main.c

core_test.o: core_test.c core.h
	$(CC) -c $(CFLAGS) $(CSANI) core_test.c

test_minmax.o: test_minmax.c minmax.h
	$(CC) -c $(CFLAGS) $(CSANI) test_minmax.c

list.o : list.h list.c
	$(CC) -c $(CFLAGS) $(CSANI) list.c

button.o :
	$(CC) -c $(CFLAGS) $(CSANI) gui/button.c

display.o :
	$(CC) -c $(CFLAGS) $(CSANI) gui/display.c

sprite_management.o :
	$(CC) -c $(CFLAGS) $(CSANI) gui/sprite_management.c

sound_management.o :
	$(CC) -c $(CFLAGS) $(CSANI) gui/sound_management.c


# create executables
main_only_core: core.o
	$(CC) $(CFLAGS) $(CSANI) -o main_only_core core.o

main_cli: core.o cli.o
	$(CC) $(CFLAGS) $(CSANI) -o main_cli core.o cli.o -lncurses

main_graphics: core.o gui.o button.o display.o sprite_management.o sound_management.o
	$(CC) $(CFLAGS) $(CSANI) -o main_graphics core.o gui.o button.o display.o sprite_management.o sound_management.o -lSDL2  -lSDL2_image -lSDL2 -lSDL2_ttf -lSDL2_mixer

core_test: core.o core_test.o
	$(CC) $(CFLAGS) $(CSANI) -o core_test core.o core_test.o

test_minmax: minmax.o core.o test_minmax.o list.o
	$(CC) $(CFLAGS) $(CSANI) -o test_minmax core.o list.o minmax.o test_minmax.o



# executions

clean:
	rm -f *.o gui/*.o main_only_core main_cli main_graphics core_test main_minmax test_minmax

run_only_core: main_only_core
	./main_only_core

run_cli: main_cli
	./main_cli

run_graphics: main_graphics
	./main_graphics

run_minmax: main_minmax
	./main_minmax

run_core_test: core_test
	./core_test

run_test_minmax: test_minmax
	./test_minmax

run_all_tests: core_test cli_test
	./core_test
	./main_cli