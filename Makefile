CC = cc

PNAME = tui

SRC  = $(shell find src -name "*.c")
OBJ  = $(SRC:.c=.o)
BIN = build

EXEC = $(BIN)/$(PNAME)
INCFLAGS  = -Isrc/

CCFLAGS += $(INCFLAGS)
CCFLAGS += -O0
CCFLAGS += -ggdb
CCFLAGS += -Wall
CCFLAGS += -pedantic
CCFLAGS += -fno-omit-frame-pointer

LDFLAGS  = $(INCFLAGS)
LDFLAGS += -lm
LDFLAGS += -lpthread

INSTALL_PATH = /usr/local/bin

all: build

run: build
	$(BIN)/$(PNAME) $*

build: $(OBJ)
	$(CC) $(CCFLAGS) -o $(BIN)/$(PNAME) $(filter %.o,$^) $(LDFLAGS)

clean:
	rm $(BIN)/* $(OBJ)

install: build
	cp $(BIN)/$(PNAME) $(INSTALL_PATH)

%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS)

