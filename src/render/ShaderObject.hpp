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

#ifndef RENDER_SHADER_OBJECT_HPP_
#define RENDER_SHADER_OBJECT_HPP_

#include <time.h>

#define MAX_SOURCES 16

enum ShaderType {
    INVALID_SHADER  = -1,
    VERTEX_SHADER   = GL_VERTEX_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    COMPUTE_SHADER  = GL_COMPUTE_SHADER
};

struct ShaderSource {
    const char *file;
    char *src;
    time_t timestamp;
};

class ShaderObject {
    ShaderType _type;
    GLuint _name;

    int _sourceCount;
    ShaderSource _sources[MAX_SOURCES];

    void loadFile(ShaderSource *s, const char *path);
    void check();

public:
    ShaderObject() : _type(INVALID_SHADER), _name(-1), _sourceCount(0) {}

    void addFile(const char *path);
    int refresh();
    void compile(ShaderType type);

    ShaderType type() const {
        return _type;
    }

    GLuint name() const {
        return _name;
    }

    int sourceCount() const {
        return _sourceCount;
    }

    const ShaderSource &source(int i) const {
        return _sources[i];
    }
};

#endif /* SHADER_OBJECT_H_ */
