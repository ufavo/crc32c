NAME 		= crc32c
#config
PREFIX = /usr/local
CFLAGS += -std=c99 -pedantic -Wall -Wextra -O3
CC ?= cc

SRC = $(wildcard src/*.c)
OBJ = ${SRC:.c=.o}
OBJ_CLI = $(OBJ) cli-tool.o

ifeq ($(NOHWACCEL),)
	
	ifeq ($(ARCH),)
		ARCH = $(shell uname -m)
	endif

	ifeq ($(findstring x86_64,$(ARCH)),x86_64)
		CFLAGS += -mcrc32 -DHW_CRC32C
	else ifeq ($(findstring aarch64,$(ARCH)),aarch64)
	
		ifneq ($(findstring -march,$(CFLAGS)),-march)
			CFLAGS += -march=armv8.1-a
		endif
		CFLAGS += -DHW_CRC32C

	endif
endif

.PHONY: all options clean install uninstall test

all: lib$(NAME).a $(NAME)

options:
	@echo $(NAME) build options:
	@echo "ARCH     = ${ARCH}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo $(NAME) install options:
	@echo "PREFIX   = ${PREFIX}"

clean:
	rm -f lib$(NAME).a $(OBJ) $(OBJ_CLI) test src/table.h $(NAME) tablegen

%.o: %.c src/table.h
	$(CC) -c ${CFLAGS} $< -o $@

src/table.h: tablegen.c
	$(CC) ${CFLAGS} tablegen.c -o tablegen
	./tablegen > $@


lib$(NAME).a: $(OBJ)
	ar rcs $@ $<

$(NAME): $(OBJ_CLI)
	$(CC) ${CFLAGS} $(OBJ_CLI) -o $@

test: lib$(NAME).a
	$(CC) $(CFLAGS) test.c $< -o test
	./test


install: lib$(NAME).a $(NAME)
	mkdir -p $(PREFIX)/lib/
	cp -f lib$(NAME).a $(PREFIX)/lib/
	mkdir -p $(PREFIX)/include/
	cp -f include/$(NAME).h $(PREFIX)/include/
	mkdir -p $(PREFIX)/bin/
	cp -f $(NAME) $(PREFIX)/bin/
	

uninstall:
	rm -f $(PREFIX)/lib/lib$(NAME).a $(PREFIX)/include/$(NAME).h $(PREFIX)/bin/$(NAME)
