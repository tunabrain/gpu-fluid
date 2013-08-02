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

#include <algorithm>
#include <GL/glew.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Fluid.hpp"
#include "render/RenderTarget.hpp"
#include "render/BufferObject.hpp"
#include "render/Texture.hpp"
#include "render/Shader.hpp"
#include "Debug.hpp"
#include "File.hpp"
#include "Util.hpp"

using namespace std;

Fluid::Fluid(int width, int height) : _width(width), _height(height) {
    _tWidth  = _width;
    _tHeight = _height;

    makePreamble("src/shaders/Preamble.txt", "src/shaders/Fluid/Preamble.txt");

    _rt = new RenderTarget();

    _hX = 1.0/min(_width, _height);
    _density   = 1.0;
    _diffusion = 0.05;
    _gravity   = -9.81;
    _tAmb      = 22.0;

    _heatIters     = 80;
    _pressureIters = 160;

    _particleCount = (_width - 1)*(_height - 1)*4;
    _particleMax   = (_width - 1)*(_height - 1)*8;

    _pTexW = 2048;
    _pTexH = (_particleMax - 1)/_pTexW + 1;

    _matVecProduct    = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "MatVecProduct.frag", 2);
    _addSub           = new Shader("src/shaders/Fluid/", "Preamble.txt", "ScalarOp.vert", 0, "AddSub.frag", 2);
    _scaledAdd        = new Shader("src/shaders/Fluid/", "Preamble.txt", "ScalarOp.vert", 0, "ScaledAdd.frag", 1);
    _advect           = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "Advect.frag", 1);
    _precon           = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "ApplyPreconditioner.frag", 2);
    _applyP           = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "ApplyPressure.frag", 2);
    _buildPRhs        = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "BuildPressureRhs.frag", 1);
    _buildPMat        = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "BuildPressureMatrix.frag", 3);
    _divide           = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "Divide.frag", 1);
    _gather           = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "Gather.frag", 1);
    _addReduce        = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "AddReduce.frag", 1);
    _maxReduce        = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "MaxReduce.frag", 1);
    _buildVorticity   = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "BuildVorticity.frag", 1);
    _confineV         = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "ConfineVorticity.frag", 2);
    _addVorticity     = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "AddVorticity.frag", 2);
    _buildHMat        = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "BuildHeatMatrix.frag", 3);
    _addBuoyancy      = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "AddBuoyancy.frag", 1);
    _histoDownsample  = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "HistoDownsample.frag", 1);
    _histoUpsample    = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "HistoUpsample.frag", 1);
    _clampCounts      = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "ClampCounts.frag", 1);
    _particleSpawn    = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "ParticleSpawn.frag", 0);
    _particleToGrid   = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "ParticleToGrid.frag", 4);
    _set              = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "Set.frag", 1);
    _calcVelocity     = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "CalcVelocity.frag", 1);
    _inflow           = new Shader("src/shaders/Fluid/", "Preamble.txt", "Fluid.vert", 0, "Inflow.frag", 1);
    _particleAdvect   = new Shader("src/shaders/Fluid/", "Preamble.txt", "ParticleQuad.vert", 0, "ParticleAdvect.frag", 1);
    _particleFromGrid = new Shader("src/shaders/Fluid/", "Preamble.txt", "ParticleQuad.vert", 0, "ParticleFromGrid.frag", 1);
    _fastSweep        = new Shader("src/shaders/Fluid/", "Preamble.txt", "ParticleQuad.vert", 0, "FastSweep.frag", 0);
    _particleRender   = new Shader("src/shaders/Fluid/", "Preamble.txt", "ParticleRender.vert", 0, "ParticleRender.frag", 1);
    _particleHisto    = new Shader("src/shaders/Fluid/", "Preamble.txt", "ParticleCount.vert", 0, 0, 0);
    _particleBucket   = new Shader("src/shaders/Fluid/", "Preamble.txt", "ParticleBucket.vert", 0, 0, 0);
    _spawnInflow      = new Shader("src/shaders/Fluid/", "Preamble.txt", "SpawnInflowParticles.vert", 0, 0, 0);

    _blackPbo = new BufferObject(PIXEL_UNPACK_BUFFER, _tWidth*_tHeight*sizeof(float));
    _blackPbo->bind();
    _blackPbo->map();
    memset(_blackPbo->data(), 0, _tWidth*_tHeight*sizeof(float));
    _blackPbo->unmap();
    _blackPbo->unbind();

    for (int i = 0; i < 2; i++) {
        _dotPTransfer[i] = new Texture(TEXTURE_2D, 2, 2);
        _dotPTransfer[i]->setFormat(TEXEL_FLOAT, 1, 4);
        _dotPTransfer[i]->init();
    }

    _histoLevels = 1;
    for (int t = max(_width, _height); t > 1; t = (t - 1)/2 + 1, _histoLevels++);

    _histoCount = new Texture*[_histoLevels];
    _histoIndex = new Texture*[_histoLevels];

    int w = _width;
    int h = _height;
    for (int i = 0; i < _histoLevels; i++) {
        _histoCount[i] = new Texture(TEXTURE_2D, w, h);
        _histoCount[i]->setFormat(TEXEL_UNSIGNED, 1, 4);
        _histoCount[i]->init();
        _histoCount[i]->copyPbo(*_blackPbo);
        _histoIndex[i] = new Texture(TEXTURE_2D, w, h);
        _histoIndex[i]->setFormat(TEXEL_UNSIGNED, 1, 4);
        _histoIndex[i]->init();
        _histoIndex[i]->copyPbo(*_blackPbo);

        w = (w - 1)/2 + 1;
        h = (h - 1)/2 + 1;
    }

    Texture **ts[] = {
		&_u, &_v, &_d, &_t, &_aDiag, &_aPlusX, &_aPlusY, &_p, &_r, &_z, &_s,
		&_tmp1, &_tmp2, &_uTmp, &_vTmp, &_tTmp, &_dTmp
    };
    for (int i = 0; i < 17; i++) {
        *(ts[i]) = new Texture(TEXTURE_2D, _tWidth, _tHeight);
        (*(ts[i]))->setFormat(TEXEL_FLOAT, 1, 4);
        (*(ts[i]))->init();
        (*(ts[i]))->copyPbo(*_blackPbo);
    }

    _particlePos = new Texture(TEXTURE_2D, _pTexW, _pTexH);
    _particlePos->setFormat(TEXEL_FLOAT, 2, 4);
    _particlePos->init();
    _particlePosTmp = new Texture(TEXTURE_2D, _pTexW, _pTexH);
    _particlePosTmp->setFormat(TEXEL_FLOAT, 2, 4);
    _particlePosTmp->init();

    _particleQ = new Texture(TEXTURE_2D, _pTexW, _pTexH);
    _particleQ->setFormat(TEXEL_FLOAT, 4, 4);
    _particleQ->init();
    _particleQTmp = new Texture(TEXTURE_2D, _pTexW, _pTexH);
    _particleQTmp->setFormat(TEXEL_FLOAT, 4, 4);
    _particleQTmp->init();

    printf("Texture memory usage: %dmb\n", (int)(Texture::memoryUsage()/(1024*1024)));
}

void Fluid::makePreamble(const char *src, const char *dst) {
    static char text[4*1024], preamble[4*1024];

    FILE *fp = fopen(src, "rb");
    fread(text, 1, fsize(fp), fp);
    fclose(fp);

    sprintf(preamble,
        "%s\n"
        "#define WIDTH          %d\n"
        "#define HEIGHT         %d\n"
        "#define T_WIDTH        %d\n"
        "#define T_HEIGHT       %d\n"
        "bool fluidCell(ivec2 coord) {\n"
        "    return coord.x >= 0 && coord.y >= 0 && coord.x < WIDTH - 1 && coord.y < HEIGHT - 1;\n"
        "}\n",
        text,
        _width,
        _height,
        _tWidth,
        _tHeight
    );

    fp = fopen(dst, "wb");
    fwrite(preamble, 1, strlen(preamble), fp);
    fclose(fp);
}

void Fluid::particleQuad(Shader & s) {
    ASSERT(RenderTarget::viewportX() == 0 &&
           RenderTarget::viewportY() == 0, "Viewport needs to be 0 aligned\n");
    ASSERT(RenderTarget::viewportW() >= _pTexW &&
           RenderTarget::viewportH() >= _pTexH, "Viewport too small\n");

    float x1 = _pTexW*2.0/RenderTarget::viewportW() - 1.0;
    float y1 = ((_particleCount - 1)/_pTexW + 1)*2.0/RenderTarget::viewportH() - 1.0;

    s.uniformF("QuadInfo", -1.0, -1.0, x1, y1);
    s.uniformI("PointInfo", _pTexW, _particleCount);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Fluid::shaderQuad(Shader &s, int x, int y, int w, int h) {
    shaderQuad(s, x, y, w, h, x, y, w, h);
}

void Fluid::shaderLoop(Shader &s, int x, int y, int w, int h) {
    shaderQuad(s, x, y, w, h, x, y, w, h, false);
}

void Fluid::shaderQuad(Shader &s, int x, int y, int w, int h, int vx, int vy, int vw, int vh, bool filled) {
    ASSERT(RenderTarget::viewportX() == 0 &&
           RenderTarget::viewportY() == 0, "Viewport needs to be 0 aligned\n");
    ASSERT(RenderTarget::viewportW() >= x + w &&
           RenderTarget::viewportH() >= y + h, "Viewport too small\n");

    float sx = 1.0/RenderTarget::viewportW()*2.0;
    float sy = 1.0/RenderTarget::viewportH()*2.0;

    float x0 = x*sx - 1.0;
    float y0 = y*sy - 1.0;
    float x1 = x0 + w*sx;
    float y1 = y0 + h*sy;

    if (!filled) {
        x0 += 0.5*sx;
        x1 -= 0.5*sx;
        y0 += 0.5*sy;
        y1 -= 0.5*sy;
    }

    s.uniformF("QuadInfo", x0, y0, x1, y1);
    s.uniformF("TexelInfo", vx, vy, vx + vw, vy + vh);

    glDrawArrays((filled ? GL_TRIANGLE_FAN : GL_LINE_LOOP), 0, 4);
}

void Fluid::parallelReduce(Shader &s, Texture &src, Texture &target, int subdiv) {
    src.bindAny();
    _tmp2->bindAny();

    _rt->selectAttachmentList(1, _rt->attachTextureAny(*_tmp2));

    s.bind();
    s.uniformI("R", src.boundUnit());

    int writeOffset = 0, readOffset = 0;
    int w = _width - 1, h = _height - 1, iter = 0;
    while (w > subdiv || h > subdiv) {
        int innerW = max((w - 1)/subdiv + 1, 1);
        int innerH = max((h - 1)/subdiv + 1, 1);
        w = (innerW + subdiv - 1) & (~(subdiv - 1));
        h = (innerH + subdiv - 1) & (~(subdiv - 1));

        if (w == subdiv && h == subdiv) {
            _rt->selectAttachmentList(1, _rt->attachTextureAny(target));
            writeOffset = 0;
        }

        s.uniformI("Inner", (innerW + readOffset)*subdiv, innerH*subdiv);
        shaderQuad(s, writeOffset, 0, w, h, readOffset, 0, w*subdiv, h*subdiv);

        if (w != subdiv || h != subdiv)
            glTextureBarrierNV();

        s.uniformI("R", _tmp2->boundUnit());

        readOffset = writeOffset;
        if (writeOffset)
            writeOffset = 0;
        else
            writeOffset = w;

        iter++;
    }
}

void Fluid::addReduce(Texture &src, Texture &target) {
    parallelReduce(*_addReduce, src, target, 4);
}

float Fluid::maxReduce(Texture &src, Texture &target) {
    parallelReduce(*_maxReduce, src, target, 2);

    float lastStep[4];
    target.bindAny();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, lastStep);

    return max(lastStep[0], max(lastStep[1], max(lastStep[2], lastStep[3])));
}

void Fluid::matVecProduct(Texture &aDiag, Texture &aPlusX, Texture &aPlusY, Texture &b, Texture &result, Texture &ab) {
    aDiag.bindAny();
    aPlusX.bindAny();
    aPlusY.bindAny();
    b.bindAny();

    RtAttachment att1 = _rt->attachTextureAny(result);
    RtAttachment att2 = _rt->attachTextureAny(ab);
    _rt->selectAttachmentList(2, att1, att2);

    _matVecProduct->bind();
    _matVecProduct->uniformI("ADiag",  aDiag .boundUnit());
    _matVecProduct->uniformI("APlusX", aPlusX.boundUnit());
    _matVecProduct->uniformI("APlusY", aPlusY.boundUnit());
    _matVecProduct->uniformI("B",      b     .boundUnit());
    shaderQuad(*_matVecProduct, 0, 0, _width - 1, _height - 1);
}

void Fluid::addSub(Texture &subA, Texture &subB, Texture &addA, Texture &addB, Texture &dstSub, Texture &dstAdd, Texture &alpha, Texture &beta) {
    subA.bindAny();
    subB.bindAny();
    addA.bindAny();
    addB.bindAny();
    alpha.bindAny();
    beta.bindAny();

    RtAttachment a1 = _rt->attachTextureAny(dstSub);
    RtAttachment a2 = _rt->attachTextureAny(dstAdd);
    _rt->selectAttachmentList(2, a1, a2);

    _addSub->bind();
    _addSub->uniformI("SrcSubA", subA.boundUnit());
    _addSub->uniformI("SrcSubB", subB.boundUnit());
    _addSub->uniformI("SrcAddA", addA.boundUnit());
    _addSub->uniformI("SrcAddB", addB.boundUnit());
    _addSub->uniformI("Alpha",  alpha.boundUnit());
    _addSub->uniformI("Beta",    beta.boundUnit());
    shaderQuad(*_addSub, 0, 0, _width - 1, _height - 1);
}

void Fluid::scaledAdd(Texture &addA, Texture &addB, Texture &dst, Texture &alpha, Texture &beta) {
    addA.bindAny();
    addB.bindAny();
    alpha.bindAny();
    beta.bindAny();

    _rt->selectAttachmentList(1, _rt->attachTextureAny(dst));

    _scaledAdd->bind();
    _scaledAdd->uniformI("SrcAddA", addA.boundUnit());
    _scaledAdd->uniformI("SrcAddB", addB.boundUnit());
    _scaledAdd->uniformI("Alpha",  alpha.boundUnit());
    _scaledAdd->uniformI("Beta",    beta.boundUnit());
    shaderQuad(*_scaledAdd, 0, 0, _width - 1, _height - 1);
}

void Fluid::advect(Texture &src, Texture &dst, float timestep, float offX, float offY, int w, int h) {
    src.bindAny();
    _u->bindAny();
    _v->bindAny();

    RtAttachment att = _rt->attachTextureAny(dst);
    _rt->selectAttachmentList(1, att);

    _advect->bind();
    _advect->uniformI("D", src.boundUnit());
    _advect->uniformI("U", _u->boundUnit());
    _advect->uniformI("V", _v->boundUnit());
    _advect->uniformI("Limits", w - 1, h - 1);
    _advect->uniformF("Timestep", timestep);
    _advect->uniformF("Offset", offX, offY);
    _advect->uniformF("InvHx", 1.0/_hX);
    shaderQuad(*_advect, 0, 0, w, h);
}

void Fluid::buildPRhs(Texture &rhs) {
    _u->bindAny();
    _v->bindAny();

    RtAttachment dst = _rt->attachTextureAny(rhs);
    _rt->selectAttachmentList(1, dst);

    _buildPRhs->bind();
    _buildPRhs->uniformI("U", _u->boundUnit());
    _buildPRhs->uniformI("V", _v->boundUnit());
    _buildPRhs->uniformF("InvHx", 1.0);
    shaderQuad(*_buildPRhs, 0, 0, _width - 1, _height - 1);
}

void Fluid::buildPMat(float timestep) {
    RtAttachment a1 = _rt->attachTextureAny(*_aDiag);
    RtAttachment a2 = _rt->attachTextureAny(*_aPlusX);
    RtAttachment a3 = _rt->attachTextureAny(*_aPlusY);

    _rt->selectAttachmentList(3, a1, a2, a3);

    _buildPMat->bind();
    _buildPMat->uniformF("Scale", timestep/_density*1.0/_hX);
    shaderQuad(*_buildPMat, 0, 0, _width - 1, _height - 1);
}

void Fluid::buildHMat(float timestep) {
    RtAttachment a1 = _rt->attachTextureAny(*_aDiag);
    RtAttachment a2 = _rt->attachTextureAny(*_aPlusX);
    RtAttachment a3 = _rt->attachTextureAny(*_aPlusY);

    _rt->selectAttachmentList(3, a1, a2, a3);

    _buildHMat->bind();
    _buildHMat->uniformF("Scale", timestep*_diffusion*1.0/(_hX*_hX));
    shaderQuad(*_buildHMat, 0, 0, _width - 1, _height - 1);
}

void Fluid::buildVorticity(Texture &dst) {
    _u->bindAny();
    _v->bindAny();

    _rt->selectAttachmentList(1, _rt->attachTextureAny(dst));

    _buildVorticity->bind();
    _buildVorticity->uniformI("U", _u->boundUnit());
    _buildVorticity->uniformI("V", _v->boundUnit());
    _buildVorticity->uniformF("Scale", 0.5/_hX);
    shaderQuad(*_buildVorticity, 0, 0, _width, _height);
}

void Fluid::confineVorticity(float epsilon, Texture &src, Texture &dstU, Texture &dstV) {
    src.bindAny();

    RtAttachment att1 = _rt->attachTextureAny(dstU);
    RtAttachment att2 = _rt->attachTextureAny(dstV);
    _rt->selectAttachmentList(2, att1, att2);

    _confineV->bind();
    _confineV->uniformI("W", src.boundUnit());
    _confineV->uniformF("Scale", 0.5/_hX);
    _confineV->uniformF("Hx", _hX);
    _confineV->uniformF("Epsilon", epsilon);
    shaderQuad(*_confineV, 0, 0, _width, _height);
}

void Fluid::addVorticity(float timestep, Texture &srcU, Texture &srcV, Texture &dstU, Texture &dstV) {
    srcU.bindAny();
    srcV.bindAny();
    _u->bindAny();
    _v->bindAny();

    RtAttachment att1 = _rt->attachTextureAny(dstU);
    RtAttachment att2 = _rt->attachTextureAny(dstV);
    _rt->selectAttachmentList(2, att1, att2);

    _addVorticity->bind();
    _addVorticity->uniformI("WU", srcU.boundUnit());
    _addVorticity->uniformI("WV", srcV.boundUnit());
    _addVorticity->uniformI("U",   _u->boundUnit());
    _addVorticity->uniformI("V",   _v->boundUnit());
    _addVorticity->uniformF("Timestep", timestep);
    shaderQuad(*_addVorticity, 0, 0, _width - 1, _height - 1);
}

void Fluid::addBuoyancy(float timestep, Texture &dstV) {
    _t->bindAny();
    _v->bindAny();

    _rt->selectAttachmentList(1, _rt->attachTextureAny(dstV));

    _addBuoyancy->bind();
    _addBuoyancy->uniformI("T", _t->boundUnit());
    _addBuoyancy->uniformI("V", _v->boundUnit());
    _addBuoyancy->uniformF("Timestep", timestep);
    _addBuoyancy->uniformF("G", _gravity);
    _addBuoyancy->uniformF("Density", _density);
    _addBuoyancy->uniformF("TAmb", _tAmb);
    shaderQuad(*_addBuoyancy, 0, 0, _width - 1, _height);
}

int Fluid::addInflow(float x, float y, float w, float h, int pAmount, const Vec4 &qMin, const Vec4 &qVal) {
	x /= _hX;
	y /= _hX;
	w /= _hX;
	h /= _hX;

    Texture *qs[] = {_d, _t, _u, _v};

    _inflow->bind();
    for (int i = 0; i < 4; i++) {
        _rt->selectAttachmentList(1, _rt->attachTextureAny(*qs[i]));
        qs[i]->bindAny();
        _inflow->uniformI("Source", qs[i]->boundUnit());
        _inflow->uniformF("Value", qVal.a[i]);
        _inflow->uniformF("ValueMin", qMin.a[i]);
        shaderQuad(*_inflow, x, y, w, h, 0, 1, 2, 0);
    }

    glTextureBarrierNV();

    int pAdd = min(_particleMax - _particleCount, pAmount);

    if (!pAdd)
        return 0;

    _particlePos->bindImage(0, false);
    _particleQ  ->bindImage(1, false);
    _spawnInflow->bind();
    _spawnInflow->uniformI("ResultP", 0);
    _spawnInflow->uniformI("ResultQ", 1);
    _spawnInflow->uniformI("PointInfo", _pTexW, _particleCount);
    _spawnInflow->uniformF("QuadInfo", x, y, w - 1, h - 1);
    _spawnInflow->uniformF("QMin", qMin);
    _spawnInflow->uniformF("QValue", qVal);
    glDrawArrays(GL_POINTS, _particleCount, pAdd);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

    return pAdd;
}

void Fluid::applyPressure(Texture &p, Texture &dstU, Texture &dstV, float timestep) {
    _u->bindAny();
    _v->bindAny();
    p.bindAny();

    RtAttachment a1 = _rt->attachTextureAny(dstU);
    RtAttachment a2 = _rt->attachTextureAny(dstV);
    _rt->selectAttachmentList(2, a1, a2);

    _applyP->bind();
    _applyP->uniformI("U", _u->boundUnit());
    _applyP->uniformI("V", _v->boundUnit());
    _applyP->uniformI("P",  p.boundUnit());
    _applyP->uniformF("Scale", timestep/_density*1.0/_hX);
    shaderQuad(*_applyP, 0, 0, _width, _height);
}

void Fluid::applyPreconditioner(Texture &r, Texture &z, Texture &ab) {
    r.bindAny();

    RtAttachment att1 = _rt->attachTextureAny(z);
    RtAttachment att2 = _rt->attachTextureAny(ab);
    _rt->selectAttachmentList(2, att1, att2);

    _precon->bind();
    _precon->uniformI("R", r.boundUnit());
    shaderQuad(*_precon, 0, 0, _width - 1, _height - 1);
}

void Fluid::calcVelocity(Texture &target) {
    _rt->selectAttachmentList(1, _rt->attachTextureAny(target));
    _u->bindAny();
    _v->bindAny();
    _calcVelocity->bind();
    _calcVelocity->uniformI("U", _u->boundUnit());
    _calcVelocity->uniformI("V", _v->boundUnit());
    shaderQuad(*_calcVelocity, 0, 0, _width - 1, _height - 1);
}

void Fluid::conjugateGradients(int &iters) {
    Texture *sigmaTex = _dotPTransfer[0];
    Texture *sigmaNTex = _dotPTransfer[1];

    clear(*_p);
    applyPreconditioner(*_r, *_z, *_tmp1);
    copy(*_s, *_z);
    addReduce(*_tmp1, *sigmaTex);

    for (int i = 0; i < iters; i++) {
        matVecProduct(*_aDiag, *_aPlusX, *_aPlusY, *_s, *_z, *_tmp1);
        addReduce(*_tmp1, *sigmaNTex);

        addSub(*_r, *_z, *_p, *_s, *_r, *_p, *sigmaTex, *sigmaNTex);
        glTextureBarrierNV();

        applyPreconditioner(*_r, *_z, *_tmp1);
        addReduce(*_tmp1, *sigmaNTex);

        scaledAdd(*_s, *_z, *_s, *sigmaNTex, *sigmaTex);
        glTextureBarrierNV();

        swap(sigmaTex, sigmaNTex);

        if (i == iters - 1) {
            float residual = maxReduce(*_r, *sigmaNTex);
            fflush(stdout);
            if (residual > 1e-2)
                iters = min(iters + 10, 4000);
            else {
                iters = max(iters - 1, 100*_width/1920);
                printf("Residual error: %f, iters %d\n", residual, iters);
            }
        }
    }
}

void Fluid::particleAdvect(float timestep) {
    _particlePos->bindAny();
    _u->bindAny();
    _v->bindAny();

    _rt->selectAttachmentList(1, _rt->attachTextureAny(*_particlePos));

    _particleAdvect->bind();
    _particleAdvect->uniformI("PPos", _particlePos->boundUnit());
    _particleAdvect->uniformI("U", _u->boundUnit());
    _particleAdvect->uniformI("V", _v->boundUnit());
    _particleAdvect->uniformF("Timestep", timestep);
    _particleAdvect->uniformF("InvHx", 1.0/_hX);
    particleQuad(*_particleAdvect);

    glTextureBarrierNV();
}

void Fluid::particleToGrid() {
    _particleQ->bindAny();
    _particlePos->bindAny();
    _histoCount[0]->bindAny();
    _histoIndex[0]->bindAny();

    RtAttachment att1 = _rt->attachTextureAny(*_d);
    RtAttachment att2 = _rt->attachTextureAny(*_t);
    RtAttachment att3 = _rt->attachTextureAny(*_u);
    RtAttachment att4 = _rt->attachTextureAny(*_v);
    _rt->selectAttachmentList(4, att1, att2, att3, att4);

    _particleToGrid->bind();
    _particleToGrid->uniformI("PPos", _particlePos->boundUnit());
    _particleToGrid->uniformI("Q",    _particleQ  ->boundUnit());
    _particleToGrid->uniformI("PointInfo", _pTexW, _particleCount);
    _particleToGrid->uniformI("Counts",  _histoCount[0]->boundUnit());
    _particleToGrid->uniformI("Offsets", _histoIndex[0]->boundUnit());
    shaderQuad(*_particleToGrid, 0, 0, _width - 1, _height - 1);
}

void Fluid::particleFromGrid(Texture &q) {
    q.bindAny();
    _d->bindAny();
    _t->bindAny();
    _u->bindAny();
    _v->bindAny();
    _uTmp->bindAny();
    _vTmp->bindAny();
    _tTmp->bindAny();
    _dTmp->bindAny();
    _particlePos->bindAny();

    _rt->selectAttachmentList(1, _rt->attachTextureAny(q));

    _particleFromGrid->bind();
    _particleFromGrid->uniformI("PPos", _particlePos->boundUnit());
    _particleFromGrid->uniformI("Q", q.boundUnit());
    _particleFromGrid->uniformI("D", _d->boundUnit());
    _particleFromGrid->uniformI("T", _t->boundUnit());
    _particleFromGrid->uniformI("U", _u->boundUnit());
    _particleFromGrid->uniformI("V", _v->boundUnit());
    _particleFromGrid->uniformI("UOld", _uTmp->boundUnit());
    _particleFromGrid->uniformI("VOld", _vTmp->boundUnit());
    _particleFromGrid->uniformI("TOld", _tTmp->boundUnit());
    _particleFromGrid->uniformI("DOld", _dTmp->boundUnit());
    particleQuad(*_particleFromGrid);

    glTextureBarrierNV();
}

void Fluid::particleExtrapolate(Texture &q, Texture &w) {
    RtAttachment att1 = _rt->attachTextureAny(q);
    RtAttachment att2 = _rt->attachTextureAny(w);

    _gather->bind();
    for (int i = 0; i < 10; i++) {
        _rt->selectAttachmentList(1, (i & 1 ? att1 : att2));
        _gather->uniformI("D", (i & 1 ? w : q).boundUnit());
        shaderQuad(*_gather, 0, 0, _width - 1, _height - 1);
    }
}

void Fluid::particleCount() {
    clear(*_histoCount[0]);
    _particlePos->bindAny();
    _histoCount[0]->bindImage(0);
    _particleHisto->bind();
    _particleHisto->uniformI("PPos", _particlePos->boundUnit());
    _particleHisto->uniformI("Counts", 0);
    _particleHisto->uniformI("PointInfo", _pTexW, _particleCount);
    glDrawArrays(GL_POINTS, 0, _particleCount);

    _histoCount[0]->bindAny();
    _rt->selectAttachmentList(1, _rt->attachTextureAny(*_histoCount[0]));
    _clampCounts->bind();
    _clampCounts->uniformI("Counts", _histoCount[0]->boundUnit());
    shaderQuad(*_clampCounts, 0, 0, _width - 1, _height - 1);
    glTextureBarrierNV();
}

void Fluid::particleBucket() {
    _rt->selectAttachmentList(0);
    _histoIndex[0]->bindAny();
    _particlePos->bindAny();
    _particleQ->bindAny();
    _particlePosTmp->bindImage(0);
    _particleQTmp->bindImage(1);
    _histoCount[0]->bindImage(2);
    _particleBucket->bind();
    _particleBucket->uniformI("ResultP", 0);
    _particleBucket->uniformI("ResultQ", 1);
    _particleBucket->uniformI("Counts", 2);
    _particleBucket->uniformI("PointInfo", _pTexW, _particleCount);
    _particleBucket->uniformI("Offsets", _histoIndex[0]->boundUnit());
    _particleBucket->uniformI("PPos", _particlePos->boundUnit());
    _particleBucket->uniformI("Q", _particleQ->boundUnit());
    glDrawArrays(GL_POINTS, 0, _particleCount);

    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    swap(_particlePos, _particlePosTmp);
    swap(_particleQ, _particleQTmp);
}

void Fluid::particleSpawn() {
    _rt->selectAttachmentList(0);
    _histoCount[0]->bindAny();
    _histoIndex[0]->bindAny();
    _particlePos->bindImage(0);
    _particleQ->bindImage(1, true, false);
    _particleSpawn->bind();
    _particleSpawn->uniformI("Counts",  _histoCount[0]->boundUnit());
    _particleSpawn->uniformI("Offsets", _histoIndex[0]->boundUnit());
    _particleSpawn->uniformI("ResultP", 0);
    _particleSpawn->uniformI("ResultQ", 1);
    _particleSpawn->uniformI("PointInfo", _pTexW, _particleCount);
    shaderQuad(*_particleSpawn, 0, 0, _width - 1, _height - 1);

    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}

void Fluid::histoPyramid() {
    _histoDownsample->bind();
    for (int i = 1; i < _histoLevels; i++) {
        _histoCount[i - 1]->bindAny();
        _histoDownsample->uniformI("Counts", _histoCount[i - 1]->boundUnit());
        _rt->selectAttachmentList(1, _rt->attachTextureAny(*_histoCount[i]));
        shaderQuad(*_histoDownsample, 0, 0, _histoCount[i]->width(), _histoCount[i]->height());
    }

    _histoUpsample->bind();
    for (int i = _histoLevels - 2; i >= 0; i--) {
        _histoIndex[i + 1]->bindAny();
        _histoCount[i]->bindAny();
        _histoUpsample->uniformI("Counts", _histoCount[i]->boundUnit());
        _histoUpsample->uniformI("Offsets", _histoIndex[i + 1]->boundUnit());
        _rt->selectAttachmentList(1, _rt->attachTextureAny(*_histoIndex[i]));
        shaderQuad(*_histoUpsample, 0, 0, _histoIndex[i]->width(), _histoIndex[i]->height());
    }
}

void Fluid::clear(Texture &a) {
    a.bindAny();
    a.copyPbo(*_blackPbo);
}

void Fluid::copy(Texture &dst, Texture &src) {
    _rt->setReadBuffer(_rt->attachTextureAny(src));
    dst.bindAny();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _width - 1, _height - 1);
}

void Fluid::setup() {
    _rt->bind();
    _rt->pushViewport(0, 0, max(_tWidth, _pTexW), max(_tHeight, _pTexH));
}

void Fluid::teardown() {
    _rt->popViewport();
    _rt->unbind();
}

void Fluid::initScene() {
    float *data1 = new float[_tWidth*_tHeight];
    float *data2 = new float[_tWidth*_tHeight];
    float *data3 = new float[_tWidth*_tHeight];
    float *data4 = new float[_tWidth*_tHeight];
    float *pData = new float[_particlePos->width()*_particlePos->height()*2];
    int   *qData = new int  [_particleQ  ->width()*_particleQ  ->height()*4];

    for (int i = 0; i < _particleMax*4; i++)
        qData[i] = 0xDEADBEEF;

    int pIdx = 0;
    for (int y = 0, idx = 0; y < _tHeight; y++) {
        for (int x = 0; x < _tWidth; x++, idx++) {
            data1[idx] = 0.0;
            data2[idx] = 0.0;
            data3[idx] = 0.0;
            data4[idx] = _tAmb;

            if (x == _tWidth - 1 || y == _tHeight - 1)
                data1[idx] = data2[idx] = data3[idx] = data4[idx] = 0.0;
            else {
                for (int i = 0; i < 4; i++) {
                    pData[pIdx++] = x + frand();
                    pData[pIdx++] = y + frand();
                }
            }
        }
    }

    _d->copy(data1);
    _u->copy(data2);
    _v->copy(data3);
    _uTmp->copy(data2);
    _vTmp->copy(data3);
    _t->copy(data4);
    _particlePos->copy(pData);
    _particleQ->copy(qData);

    delete[] data1;
    delete[] data2;
    delete[] data3;
    delete[] data4;
    delete[] pData;
    delete[] qData;

    setup();
    particleFromGrid(*_particleQ);
    teardown();
}

void Fluid::update(float timestep) {
    particleAdvect(timestep);
    particleCount();
    histoPyramid();
    particleBucket();
    particleSpawn();
    particleToGrid();

    particleExtrapolate(*_d, *_p);
    swap(_d, _p);
    particleExtrapolate(*_t, *_p);
    swap(_t, _p);
    clear(*_p);
    particleExtrapolate(*_u, *_p);
    swap(_u, _p);
    particleExtrapolate(*_v, *_p);
    swap(_v, _p);

    clear(*_p);

    _set->bind();
    _set->uniformF("Value", 0.0);
    _rt->selectAttachmentList(1, _rt->attachTextureAny(*_u));
    shaderLoop(*_set, 0, -1, _width, _height + 2);
    _rt->selectAttachmentList(1, _rt->attachTextureAny(*_v));
    shaderLoop(*_set, -1, 0, _width + 2, _height);
    _rt->selectAttachmentList(1, _rt->attachTextureAny(*_t));
    _set->uniformF("Value", 0.0);
    shaderLoop(*_set, -1, -1, _width + 1, _height + 1);

    copy(*_uTmp, *_u);
    copy(*_vTmp, *_v);
    copy(*_tTmp, *_t);
    copy(*_dTmp, *_d);

    buildVorticity(*_p);
    confineVorticity(1.0, *_p, *_z, *_r);
    addVorticity(timestep, *_z, *_r, *_s, *_p);
    swap(_u, _s);
    swap(_v, _p);

    buildHMat(timestep);
    swap(_t, _r);
    conjugateGradients(_heatIters);
    swap(_t, _p);
    addBuoyancy(timestep, *_r);
    swap(_v, _r);

    buildPRhs(*_r);
    buildPMat(timestep);

    conjugateGradients(_pressureIters);

    applyPressure(*_p, *_z, *_r, timestep);
    swap(_u, _z);
    swap(_v, _r);

    clear(*_z);
    clear(*_r);

    static int t;
    if (t++) {
        _histoCount[_histoLevels - 1]->bindAny();
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &_particleCount);
        printf("# Particles: %d ", _particleCount);
    }

    int pAdd = addInflow(0.68, 0.05, 0.4, 0.01, 5000*_width/1920, Vec4(0.0, _tAmb, 0.0, 0.0), Vec4(1.0, 200.0, 0.0, 0.0));

    particleFromGrid(*_particleQ);

    _particleCount += pAdd;
}

float Fluid::recommendedTimestep() {
    calcVelocity(*_uTmp);
    float maxU = maxReduce(*_uTmp, *_dotPTransfer[0]);

    return 2.0/maxU;
}
