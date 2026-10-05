CC=gcc
CFLAGS=-Wall -std=c99 -pedantic
LDFLAGS=
EXEC=giff_dev
SRC= newgiff.c strbuf.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)
	@git diff | ./giff_dev

debug: CFLAGS+=-g
debug: $(EXEC)

$(EXEC): $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

# hello.o: hello.c
# 	$(EXEC) -o $@ -c $< $(CFLAGS)

# main.o: main.c hello.h
# 	$(EXEC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrpropper test

test:
	@git diff

clean:
	@rm -f *.o

mrpropper: clean
	@rm -f $(EXEC)
