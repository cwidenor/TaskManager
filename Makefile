CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
FLAGS = -g -w
CLIBS = `pkg-config --libs gtk+-3.0`
SRC_C = task drawings.c file_systems.c free.c graphs.c home.c page_one.c process_actions.c process_info.c process_list.c

task:
	$(CC) $(CFLAGS) -o $(SRC_C) $(CLIBS) $(FLAGS)

.PHONY: task