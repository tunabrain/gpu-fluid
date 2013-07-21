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

#include <GL/glew.h>

#include "VertexBuffer.hpp"
#include "Shader.hpp"

static int glTypeSize(GLenum type) {
    switch (type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return 2;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        return 4;
    case GL_DOUBLE:
        return 8;
    default:
        return 0;
    }
}

VertexBuffer::VertexBuffer(int length) : BufferObject(ELEMENT_ARRAY_BUFFER),
        _length(length), _elementSize(0), _attributeCount(0) {}

void VertexBuffer::initBuffer() {
    init(_length*_elementSize);
}

void VertexBuffer::addAttribute(const char *name, GLint size, GLenum type, int norm) {
    _attributes[_attributeCount++] = VertexAttrib(name, size, norm, type, _elementSize);
    _elementSize += size*glTypeSize(type);
}

void VertexBuffer::setStandardAttributes(int attributes) {
    for (int i = 0; i < VBO_ATT_COUNT; i++) {
        if (attributes & (1 << i)) {
            switch (1 << i) {
            case VBO_ATT_POSITION:
                addAttribute("Position", 3, GL_FLOAT, 0);
                break;
            case VBO_ATT_NORMAL:
                addAttribute("Normal", 3, GL_FLOAT, 0);
                break;
            case VBO_ATT_COLOR:
                addAttribute("Color", 4, GL_UNSIGNED_BYTE, 1);
                break;
            case VBO_ATT_TEXCOORD0:
                addAttribute("TexCoord0", 2, GL_FLOAT, 0);
                break;
            case VBO_ATT_TEXCOORD1:
                addAttribute("TexCoord1", 2, GL_FLOAT, 0);
                break;
            }
        }
    }
}

void VertexBuffer::enableVertexAttrib(int i) {
    const VertexAttrib& a = _attributes[i];

    if (a.index >= 0) {
        glEnableVertexAttribArray(a.index);
        glVertexAttribPointer(a.index, a.size, a.type, a.norm, _elementSize,
                (const GLvoid *) a.offset);
    }
}

void VertexBuffer::disableVertexAttrib(int i) {
    if (_attributes[i].index >= 0) {
        glDisableVertexAttribArray(_attributes[i].index);
    }
}

void VertexBuffer::enableVertexAttributes() {
    for (int i = 0; i < _attributeCount; i++)
        enableVertexAttrib(i);
}

void VertexBuffer::disableVertexAttributes() {
    for (int i = 0; i < _attributeCount; i++)
        disableVertexAttrib(i);
}

void VertexBuffer::mapAttributes(Shader &shader) {
    for (int i = 0; i < _attributeCount; i++) {
        _attributes[i].index = glGetAttribLocation(shader.program(), _attributes[i].name);
    }
}

void VertexBuffer::draw(Shader &shader, GLenum mode, int count) {
    bind();
    mapAttributes(shader);
    enableVertexAttributes();
    glDrawArrays(mode, 0, count ? count : _length);
    disableVertexAttributes();
    unbind();
}

void VertexBuffer::drawIndexed(BufferObject &ibo, Shader &shader, GLenum mode, int count) {
    bind();
    ibo.bind();
    mapAttributes(shader);
    enableVertexAttributes();
    glDrawElements(mode, count ? count : ibo.size()/2, GL_UNSIGNED_SHORT, NULL);
    disableVertexAttributes();
    ibo.unbind();
    unbind();
}
