/*
Copyright (c) 2013 Benedikt Bitterli

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <GL/glew.h>
#include <sys/time.h>
#include <GL/freeglut.h>

#include "render/RenderTarget.hpp"
#include "render/MatrixStack.hpp"
#include "render/Texture.hpp"
#include "lodepng/lodepng.h"
#include "render/Shader.hpp"
#include "math/Vec3.hpp"
#include "math/Mat4.hpp"
#include "Debug.hpp"
#include "Fluid.hpp"
#include "Util.hpp"

using namespace std;

#define RECORD_FRAMES 0

const int GWidth = 1280;
const int GHeight = 720;
const int FWidth = 640;
const int FHeight = 360;

static Fluid *fluid;
static Shader *quad;

static void render() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Texture *d = fluid->density();
    Texture *u = fluid->u();
    Texture *v = fluid->v();
    Texture *p = fluid->p();
    Texture *t = fluid->t();

    d->bindAny();
    u->bindAny();
    v->bindAny();
    p->bindAny();
    t->bindAny();
    quad->bind();
    quad->uniformI("D", d->boundUnit());
    quad->uniformI("U", u->boundUnit());
    quad->uniformI("V", v->boundUnit());
    quad->uniformI("P", p->boundUnit());
    quad->uniformI("T", t->boundUnit());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

#if RECORD_FRAMES
    static int iteration = 0;
    static unsigned char density[FWidth*FHeight], frame[FWidth*FHeight*3];
    d->bindAny();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, density);

    for (int i = 0; i < FWidth*FHeight*3; i++)
        frame[i] = max(0xFF - density[i/3], 0);

    char path[1024];
    sprintf(path, "Frame%05d.png", iteration++);
    lodepng_encode24_file(path, frame, FWidth, FHeight);
#endif

    const float deltaT = 0.499f*1e-3*1920/FWidth;
    fluid->setup();
    float T = 0.0;
    while (T < deltaT) {
        float dt = min(fluid->recommendedTimestep(), 0.25f*1e-3f);
        if (T + dt > deltaT) {
            dt = deltaT - T;
            T = deltaT;
        }
        fluid->update(dt);
        T += dt;
    }
    fluid->teardown();
}

static void initShaders() {
    quad = new Shader("src/shaders/", "Preamble.txt", "Quad.vert", 0, "Quad.frag", 1);
}

static void initRender() {
    glDebugMessageCallbackARB((GLDEBUGPROCARB)errorCallback, NULL);
    //glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    RenderTarget::resetViewport();

    fluid = new Fluid(FWidth, FHeight);
    fluid->initScene();
}

static void initGl() {
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();
}

static void display() {
    render();
    glutSwapBuffers();
}

static void keyboard(unsigned char mkey, int x, int y) {
    switch(mkey) {
        case 0x1b:
            exit(EXIT_SUCCESS);
            break;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(GWidth, GHeight);
    glutInitWindowPosition(128, 128);

    glutInitContextVersion(4, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutCreateWindow("GPU Fluid Solver");

    glutKeyboardFunc(keyboard);

    initGl();
    initShaders();
    initRender();

    while(1) {
        display();

        glutMainLoopEvent();
    }

    return 0;
}
