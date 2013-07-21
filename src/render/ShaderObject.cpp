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

#include <stdlib.h>
#include <GL/glew.h>
#include <string.h>

#include "Debug.hpp"
#include "ShaderObject.hpp"
#include "File.hpp"

void ShaderObject::addFile(const char *path) {
    loadFile(&_sources[_sourceCount++], path);
}

int ShaderObject::refresh() {
    int compileFlag = 0;

    for (int i = 0; i < _sourceCount; i++)
        if (ftime(_sources[i].file) > _sources[i].timestamp) {
            loadFile(&_sources[i], _sources[i].file);
            compileFlag = 1;
        }

    if (compileFlag)
        compile(_type);

    return compileFlag;
}

void ShaderObject::compile(ShaderType type_) {
    const GLchar *source[MAX_SOURCES];
    for (int i = 0; i < _sourceCount; i++)
        source[i] = _sources[i].src;

    GLuint shader = glCreateShader(type_);
    glShaderSource(shader, _sourceCount, source, NULL);
    glCompileShader(shader);

    _name = shader;
    _type = type_;

    check();
}

void ShaderObject::loadFile(ShaderSource *s, const char *path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL)
        FAIL("Unable to open file '%s'\n", path);

    int size = fsize(file);
    char *src = (char *)malloc((size + 1)*sizeof(char));
    fread(src, sizeof(char), size, file);
    src[size] = '\0';

    s->file = path;
    s->src = src;
    s->timestamp = ftime(path);
}

void ShaderObject::check() {
    GLuint obj = _name;
    GLchar tmp[2048];

    int status, length;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
    GLchar *log = NULL;
    if (length > 1) {
        log = (GLchar *)malloc(length*sizeof(GLchar));
        glGetShaderInfoLog(obj, length, NULL, log);
    }

    if (!status || length > 1) {
        int src_length;
        glGetShaderiv(obj, GL_SHADER_SOURCE_LENGTH, &src_length);

        GLchar *src = (GLchar *)malloc(src_length*sizeof(GLchar));
        glGetShaderSource(obj, src_length, NULL, src);

        DBG("shader", WARN, "---------------------------\n");
        int line = 1;
        GLchar *src2 = src;
        for (int i = 0; i < src_length; i++, src2++) {
            if (*src2 == '\n') {
                memcpy(tmp, src, src2 - src);
                tmp[src2 - src] = '\0';

                DBG("shader", WARN, "%4d | %s\n", line, tmp);
                src = src2 + 1;
                line++;
            }
        }
        DBG("shader", WARN, "%4d | %s\n", line, src);
        DBG("shader", WARN, "---------------------------\n");
        DBG("shader", WARN, "%s\n", log);
        FAIL("Unable to compile shader\n");
    } else if (length > 1) {
        DBG("shader", WARN, "%s\n", log);
    }
}
