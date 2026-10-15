CC ?= cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2

all: fuzzy_loan

fuzzy_loan: fuzzy.c model.c main.c fuzzy.h model.h params.h
	$(CC) $(CFLAGS) fuzzy.c model.c main.c -o fuzzy_loan -lm

run: fuzzy_loan
	./fuzzy_loan

clean:
	rm -f fuzzy_loan
