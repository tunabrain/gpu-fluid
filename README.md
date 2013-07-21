![Fluid](https://raw.github.com/tunabrain/gpu-fluid/master/Header.png)

OpenGL GPU Fluid Solver
=========

This project implements a 2D fluid solver completely on the GPU using OpenGL 4.3.

The solver features a marker-and-cell grid, vorticity confinement, fluid implicit particle, 3rd order Runge-Kutta advection, a conjugate gradient solver with incomplete Poisson preconditioner, and a heat diffusion/buoyancy model.

Videos rendered with this code:

* http://www.youtube.com/watch?v=SzqYnjIR4n0
* http://www.youtube.com/watch?v=vMB8elqhum0

Compilation
-----------

Compilation requires freeglut and a recent version of GLEW. The makefile in the repository should work on Windows (tested with MinGW) and Linux. 

Usage
-----

A recent GPU with fresh drivers is necessary to run this application. Support for OpenGL 4.3 core profile is required. Tested on a GTX480.

When run, the program will open a graphics window and display a preview of the current simulation progress. If the macro <code>RECORD_FRAMES</code> in <code>Main.cpp</code> is set, the program will save out the individual frames as pngs using lodepng.

Note that the window resolution is independent of the fluid resolution; they are controlled individually using the constants <code>GWidth/GHeight</code> for the graphics window and <code>FWidth/FHeight</code> for the fluid grid resolution in <code>Main.cpp</code>. This is to allow for fluid resolutions much larger than the screen resolution. Frames will always be saved at the fluid resolution.

Code
----

<code>Main.cpp</code> controls the application setup and invokes the fluid solver. <code>Fluid.cpp</code>, along with all the shader files, performs all of the fluid related work. All the remaining files are utilities to deal with OpenGL.

Scene setup and inflows are controlled in <code>Fluid::initScene</code> and at the end of <code>Fluid::update</code>. These can be modified to get more interesting looking simulations. 