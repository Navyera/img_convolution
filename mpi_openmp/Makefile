CC        = mpicc
LIBS      = -lm
CFLAGS    = -O2 -Wall

TARGET = img_conv

SRCDIR = ./src/
BINDIR = ./bin/
INCL_DIR = ./include/

CFILES = main.c\
         args.c\
         utils.c\
         convolute.c\
         neighbours.c\
         work.c\
         io.c

DEPS = ./include/*.h

filter = identity

SRC = $(addprefix $(SRCDIR), $(CFILES))
OBJ = $(addprefix $(BINDIR), $(CFILES:.c=.o))

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

bin/%.o : src/%.c $(DEPS)
	$(CC) -c $(CFLAGS) -I $(INCL_DIR) $< -o $@ -D FILTER=$(filter)

clean:
	rm -f $(OBJ)
