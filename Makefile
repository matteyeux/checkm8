CC		= gcc
TARGET	= checkm8

CFLAGS	= -I. -g -Wall -Wextra
LDFLAGS	= -lusb-1.0
SRC 	= $(wildcard src/*.c)
OBJ		= $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

src/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
