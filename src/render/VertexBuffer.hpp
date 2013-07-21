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

#ifndef RENDER_VBOBUFFER_HPP_
#define RENDER_VBOBUFFER_HPP_

#include "BufferObject.hpp"

#define MAX_VBO_ATTRIBUTES 16

enum VboAttributes {
    VBO_ATT_POSITION  = (1 << 0),
    VBO_ATT_NORMAL    = (1 << 1),
    VBO_ATT_COLOR     = (1 << 2),
    VBO_ATT_TEXCOORD0 = (1 << 3),
    VBO_ATT_TEXCOORD1 = (1 << 4),
    VBO_ATT_COUNT     = 5
};

class Shader;

class VertexBuffer: public BufferObject {
    struct VertexAttrib {
        const char *name;
        int size;
        bool norm;
        GLenum type;
        size_t offset;
        GLint index;

        VertexAttrib() {}
        VertexAttrib(const char *_name, int _size, bool _norm, GLenum _type, size_t _offset) :
            name(_name), size(_size), norm(_norm), type(_type), offset(_offset), index(-1) {}
    };

    int _length;
    int _elementSize;

    int _attributeCount;
    VertexAttrib _attributes[MAX_VBO_ATTRIBUTES];

    void enableVertexAttrib(int index);
    void disableVertexAttrib(int index);

public:
    VertexBuffer(int length);

    void addAttribute(const char *name, GLint size, GLenum type, int norm);
    void setStandardAttributes(int attributes);

    void initBuffer();

    void enableVertexAttributes();
    void disableVertexAttributes();

    void mapAttributes(Shader &shader);
    void draw(Shader &shader, GLenum mode, int count = 0);
    void drawIndexed(BufferObject &ibo, Shader &shader, GLenum mode, int count = 0);

    int length() const {
        return _length;
    }

    int elementSize() const {
        return _elementSize;
    }

    int attributeCount() const {
        return _attributeCount;
    }
};

#endif /* RENDER_VBOBUFFER_HPP_ */
