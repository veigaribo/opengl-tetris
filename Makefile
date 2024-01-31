CC=gcc
CFLAGS=$(shell pkg-config --cflags glfw3 glew)
LDFLAGS=$(shell pkg-config --libs glfw3 glew)
CUSTOMCFLAGS=

# Use `TRACK_FPS=1` in the invocation of `make` to enable
ifdef TRACK_FPS
	TRACK_FPS_FLAGS=-DTRACK_FPS
else
	TRACK_FPS_FLAGS=
endif

# Use `DEBUG=1` in the invocation of `make` to enable
ifdef DEBUG
	CUSTOMCFLAGS := $(CUSTOMCFLAGS) -g
endif

SRCDIR=src
ODIR=obj

SOURCES=$(notdir $(wildcard $(SRCDIR)/*.c))
OBJS=$(patsubst %.c,%.o,$(SOURCES))

$(ODIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $(CFLAGS) $(CUSTOMCFLAGS) $(TRACK_FPS_FLAGS) $<

tetris: $(addprefix $(ODIR)/,$(OBJS))
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm $(ODIR)/*.o tetris
