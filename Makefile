CC = gcc
CFLAGS = -Wall -Werror -Wextra
SOURCES = s21_grep.c
OBJECTS = $(SOURCES:.c = .o)

all: s21_grep

s21_grep: s21_grep.o
	$(CC) $(OBJECTS) -o s21_grep

debug: s21_grep.o
	$(CC) $(OBJECTS) -g -o s21_grep

test:
	clang-format -n $(SOURCES)

linter:
	clang-format -i $(SOURCES)

rebuild: clean all

clean: 
	rm -rf *.o s21_grep
