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

#ifndef FLUID_HPP_
#define FLUID_HPP_

#include "math/Vec4.hpp"

class BufferObject;
class RenderTarget;
class Texture;
class Shader;

class Fluid {
    RenderTarget *_rt;
    BufferObject *_blackPbo;

    Shader *_matVecProduct, *_addSub, *_scaledAdd, *_advect, *_applyP;
    Shader *_buildPRhs, *_buildPMat, *_precon, *_divide, *_addReduce;
    Shader *_buildVorticity, *_confineV, *_addVorticity, *_buildHMat;
    Shader *_addBuoyancy, *_fastSweep, *_gather, *_clampCounts;
    Shader *_particleAdvect, *_particleFromGrid, *_particleToGrid, *_particleRender;
    Shader *_particleHisto, *_particleBucket, *_histoDownsample, *_histoUpsample;
    Shader *_particleSpawn, *_set, *_maxReduce, *_calcVelocity, *_inflow, *_spawnInflow;

    Texture *_dotPTransfer[2];
    Texture *_u, *_v, *_d, *_t, *_aDiag, *_aPlusX, *_aPlusY;
    Texture *_p, *_r, *_z, *_s, *_tmp1, *_tmp2, *_uTmp, *_vTmp, *_tTmp, *_dTmp;
    Texture *_particlePos, *_particleQ;
    Texture *_particlePosTmp, *_particleQTmp;
    Texture **_histoCount, **_histoIndex;

    int _histoLevels;

    int _width, _height;
    int _tWidth, _tHeight;

    int _pTexW, _pTexH;
    int _particleCount;
    int _particleMax;

    int _heatIters;
    int _pressureIters;

    float _hX;
    float _density;
    float _diffusion;
    float _gravity;
    float _tAmb;

    void makePreamble(const char *src, const char *dst);

    void particleQuad(Shader &s);
    void shaderQuad(Shader &s, int x, int y, int w, int h);
    void shaderLoop(Shader &s, int x, int y, int w, int h);
    void shaderQuad(Shader &s, int x, int y, int w, int h, int vx, int vy, int vw, int vh, bool filled = true);

    void parallelReduce(Shader &s, Texture &src, Texture &target, int subdiv);
    void addReduce(Texture &src, Texture &target);
    float maxReduce(Texture &src, Texture &target);


    void advect(Texture &src, Texture &dst, float timestep, float offX, float offY, int w, int h);

    void buildPRhs(Texture &rhs);
    void buildPMat(float timestep);
    void buildHMat(float timestep);

    void matVecProduct(Texture &aDiag, Texture &aPlusX, Texture &aPlusY, Texture &b, Texture &result, Texture &ab);
    void addSub(Texture &subA, Texture &subB, Texture &addA, Texture &addB, Texture &dstSub, Texture &dstAdd, Texture &alpha, Texture &beta);
    void scaledAdd(Texture &addA, Texture &addB, Texture &dst, Texture &alpha, Texture &beta);
    void applyPreconditioner(Texture &r, Texture &z, Texture &ab);
    void conjugateGradients(int &iters);

    void applyPressure(Texture &p, Texture &dstU, Texture &dstV, float timestep);

    void calcVelocity(Texture &target);
    void buildVorticity(Texture &dst);
    void confineVorticity(float epsilon, Texture &src, Texture &dstU, Texture &dstV);
    void addVorticity(float timestep, Texture &srcU, Texture &srcV, Texture &dstU, Texture &dstV);
    void addBuoyancy(float timestep, Texture &dstV);

    int addInflow(float x, float y, float w, float h, int pAmount, const Vec4 &qMin, const Vec4 &qVal);

    void particleAdvect(float timestep);
    void particleToGrid();
    void particleFromGrid(Texture &q);
    void particleExtrapolate(Texture &q, Texture &w);
    void particleCount();
    void particleBucket();
    void particleSpawn();

    void histoPyramid();

    void clear(Texture &a);
    void copy(Texture &dst, Texture &src);

public:
    Fluid(int width, int height);

    void initScene();
    void update(float timestep);
    float recommendedTimestep();

    void setup();
    void teardown();

    Texture *density() {
        return _d;
    }

    Texture *u() {
        return _u;
    }

    Texture *v() {
        return _v;
    }

    Texture *t() {
        return _t;
    }

    Texture *p() {
        return _p;
    }
};

#endif /* FLUID_HPP_ */
