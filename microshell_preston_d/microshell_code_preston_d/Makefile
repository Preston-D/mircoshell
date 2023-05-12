CC=gcc
CFLAGS=-Wall -g

OBJS=myshell.o argparse.o builtin.o

default: myshell

myshell: $(OBJS)
	$(CC) $(CFLAGS) -o myshell $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o myshell
