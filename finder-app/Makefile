CC := $(if $(CROSS_COMPILE),$(CROSS_COMPILE)gcc,gcc)

all: writer

writer: writer.c
	$(CC) writer.c -o writer

clean:
	rm -f writer *.o
