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

#ifndef RENDER_RENDERTARGET_HPP_
#define RENDER_RENDERTARGET_HPP_

#include <GL/gl.h>
#include <stack>

class Texture;

enum RtAttachment {
    RT_ATTACHMENT0,
    RT_ATTACHMENT1,
    RT_ATTACHMENT2,
    RT_ATTACHMENT3,
    RT_ATTACHMENT4,
    RT_ATTACHMENT5,
    RT_ATTACHMENT6,
    RT_ATTACHMENT7
};

struct Viewport {
    int x, y, w, h;

    Viewport(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
};

class RenderTarget {
    static const int MaxAttachments = 8;
    static int _attachmentCount;
    static RtAttachment _selectedAttachments[MaxAttachments];
    static std::stack<Viewport> _viewports;

    static int _viewportX, _viewportY;
    static int _viewportW, _viewportH;

    int _nextTicket;
    int _attachmentTicket[MaxAttachments];
    const Texture *_attachments[MaxAttachments];


    GLuint _glName;

    bool attachmentSwapRequired(int num, const RtAttachment bufs[]);

public:
    RenderTarget();
    ~RenderTarget();

    void selectAttachments(int num);
    void selectAttachmentList(int count, ...);
    void selectAttachments(int num, const RtAttachment bufs[]);
    void setReadBuffer(RtAttachment buf);

    RtAttachment attachTextureAny(const Texture &tex);
    void attachTexture(const Texture &tex, int index, int level = 0);
    void attachDepthBuffer(const Texture &tex);
    void attachDepthStencilBuffer(const Texture &tex);
    void detachTexture(int index);
    void detachDepthBuffer();
    void detachDepthStencilBuffer();

    void bind();

    GLuint glName() const {
        return _glName;
    }

    static void unbind();
    static void resetViewport();
    static void setViewport(int x, int y, int w, int h);
    static void getViewport(int &x, int &y, int &w, int &h);
    static void pushViewport(int x, int y, int w, int h);
    static void popViewport();

    static int viewportX() {
        return _viewportX;
    }

    static int viewportY() {
        return _viewportY;
    }

    static int viewportW() {
        return _viewportW;
    }

    static int viewportH() {
        return _viewportH;
    }
};

#endif // RENDER_RENDERTARGET_HPP_
