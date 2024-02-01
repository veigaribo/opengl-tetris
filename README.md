# A Tetris Clone

## Controls

| Key       | Action                          |
| --------- | ------------------------------- |
| A / H / ← | Move current piece to the left  |
| S / J / ↓ | Move current piece down         |
| D / L / → | Move current piece to the right |
| Z         | Rotate current piece clockwise  |

## Building

To build, you will need the following C/C++ libraries and headers available:

- GLFW (_Graphics Library Framework_)
- GLEW (_The OpenGL Extension Wrangler_)

### Ubuntu (and likely most Debian-based distributions as well):

```shell
# apt install libglfw3 libglfw3-dev libglew2.1 libglew-dev
```

### Fedora:

```shell
# dnf install glfw glfw-devel libGLEW glew-devel
```

### Arch Linux:

For wayland:

```shell
# pacman -S glfw-wayland glew
```

For XOrg:

```shell
# pacman -S glfw-x11 glew
```

<hr>

Then, to compile it using GCC, just run

```shell
make tetris
```

or simply

```shell
make
```

(_Assumes the `pkg-config` command is globally available_)

Additionally

```shell
make DEBUG=1
```

compiles with the `-g` flag (for debug information) and no `-O`,

```shell
make TRACK_FPS=1
```

will flood your stdout with some measure of frames per second when the game is ran,

```shell
make FAST_RENDER=1
```

will render the game by mapping each tile of the field to a texel instead of doing more complex vertex / triangle stuff, which I think is pretty neat, but doesn't seem faster at all... I mean, it's still about 10000 FPS on my machine but...

Of course, you can mix those flags whatever way you like.
