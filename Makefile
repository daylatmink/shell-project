CC=gcc
CFLAGS=-Wall -Wextra -g
SRC_FILES=cell.c builtin.c utils.c processlist.c
OUT=cell

$(OUT): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC_FILES) -lreadline

clean:
	rm -f $(OUT)