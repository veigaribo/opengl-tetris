# A Tetris Clone

This version contains a ~~bug~~ quirk where, if you manage to move the piece just as it's going to move on its own, it may actually clip inside of another piece (or wall).

## Controls

| Key | Action                          |
| --- | ------------------------------- |
| A   | Move current piece to the left  |
| S   | Move current piece down         |
| D   | Move current piece to the right |
| Z   | Rotate current piece clockwise  |

## Building

To build, you will need the following C/C++ libraries and headers available:

- GLFW (_Graphics Library Framework_)
- GLEW (_The OpenGL Extension Wrangler_)

### Ubuntu (and likely most Debian-based distributions as well):

```bash
apt install libglfw3 libglfw3-dev libglew2.1 libglew-dev
```

### Fedora:

```bash
dnf install glfw glfw-devel libGLEW glew-devel
```

Then, to compile it using GCC, just run

```bash
make tetris
```

(_Assumes the `pkg-config` command is globally available_)
