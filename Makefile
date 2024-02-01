CC=gcc
CFLAGS=$(shell pkg-config --cflags glfw3 glew)
LDFLAGS=$(shell pkg-config --libs glfw3 glew)
CUSTOMCFLAGS=

# Use `DEBUG=1` in the invocation of `make` to enable
ifdef DEBUG
	CUSTOMCFLAGS := $(CUSTOMCFLAGS) -g -DDEBUG
else
	CUSTOMCFLAGS := $(CUSTOMCFLAGS) -O3
endif

SRCDIR=src
ODIR=obj

SOURCES=field.c game.c input.c main.c piece.c

# Use `TRACK_FPS=1`
ifdef TRACK_FPS
	CUSTOMCFLAGS := $(CUSTOMCFLAGS) -DTRACK_FPS
	SOURCES := $(SOURCES) fps.c
endif

# Use `FAST_RENDER=1`
ifdef FAST_RENDER
	SOURCES := $(SOURCES) fast_render.c
else
	SOURCES := $(SOURCES) fancy_render.c
endif

OBJS=$(patsubst %.c,%.o,$(SOURCES))

$(ODIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $(CFLAGS) $(CUSTOMCFLAGS) $<

tetris: $(addprefix $(ODIR)/,$(OBJS))
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm $(ODIR)/*.o tetris
