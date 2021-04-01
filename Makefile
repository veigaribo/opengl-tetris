CC=gcc
CFLAGS=$(shell pkg-config --cflags glfw3 glew)
LDFLAGS=$(shell pkg-config --libs glfw3 glew)
CUSTOMCFLAGS=

SRCDIR=src
ODIR=obj

SOURCES=$(notdir $(wildcard $(SRCDIR)/*.c))
OBJS=$(patsubst %.c,%.o,$(SOURCES))

$(ODIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $(CFLAGS) $(CUSTOMCFLAGS) $<

tetris: $(addprefix $(ODIR)/,$(OBJS))
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm $(ODIR)/*.o tetris
