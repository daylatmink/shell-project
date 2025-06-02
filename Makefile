CC=gcc
CFLAGS=-Wall -Wextra -g
SRC=cell.c builtin.c utils.c -lreadline
OUT=cell

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
	
