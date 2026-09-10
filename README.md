# OpenGL Draw Stuff
A very simple OpenGL abstraction header only library made to draw stuff. As of now, it focuses on 2D rendering, but it does support 3D rendering as well.

To know more on how to use this library, you can look into the `tests` folder. When building, be sure to turn the image files into C headers first.

As the programmer using this library, you are responsible to handle dangling pointers.

## Features
- Header-only
- 2D rendering
- Basic 3D rendering
- Shape drawing and transformations
- Texture loading

## Dependencies
- GLM >= 1.1.0
- stb_image >= 2.30
- OpenGL 4.6
- GLAD

Note that I haven't tested this library with every version of the listed dependencies. Lower versions of the dependencies may work.

This library relies on glm for positions and transformations of shapes. Uses stb for texture imports.

