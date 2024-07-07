#include <GameEngineFramework/Renderer/components/material.h>

#define GLEW_STATIC
#include <gl/glew.h>


Material::Material() : 
    shader(nullptr),
    
    isShared(false),
    
    mDoDepthTest   (true),
    mDoBlending    (false),
    mDoFaceCulling (true),
    mDoShadowPass  (false),
    
    mShadowVolumeLength(5),
    
    mShadowVolumeIntensityLow(0),
    mShadowVolumeIntensityHigh(1),
    
    mShadowVolumeColorIntensity(0),
    mShadowVolumeAngleOfView(0),
    
    mDepthFunc(MATERIAL_DEPTH_LESS),
    
    mFaceWinding(GL_CCW),
    mFaceCullSide(GL_BACK),
    
    mBlendSource(BLEND_SRC_COLOR),
    mBlendDestination(BLEND_SRC_ALPHA),
    mBlendAlphaSource(BLEND_ONE_MINUS_SRC_COLOR),
    mBlendAlphaDestination(BLEND_ONE_MINUS_SRC_ALPHA),
    mBlendFunction(BLEND_EQUATION_ADD)
{
    ambient = Color(0, 0, 0, 1);
    diffuse = Color(1, 1, 1, 1);
    specular = Color(0, 0, 0, 1);
    mShadowVolumeColor = Color(1, 1, 1);
    
    return;
}

void Material::EnableDepthTest(void) {
    mDoDepthTest = true;
    return;
}

void Material::DisableDepthTest(void) {
    mDoDepthTest = false;
    return;
}

void Material::SetDepthFunction(GLint func) {
    mDepthFunc = func;
    return;
}

void Material::EnableCulling(void) {
    mDoFaceCulling = true;
    return;
}

void Material::DisableCulling(void) {
    mDoFaceCulling = false;
    return;
}

void Material::SetFaceCullingSide(GLint side) {
    mFaceCullSide = side;
    return;
}

void Material::SetFaceWindingOrder(GLint direction) {
    mFaceWinding = direction;
    return;
}

void Material::EnableBlending(void) {
    mDoBlending = true;
    return;
}

void Material::DisableBlending(void) {
    mDoBlending = false;
    return;
}

void Material::SetBlending(GLint src, GLint dest)  {
    mBlendSource = src;
    mBlendDestination = dest;
    return;
}

void Material::SetBlendingAlpha(GLint srca, GLint desta) {
    mBlendAlphaSource = srca;
    mBlendAlphaDestination = desta;
    return;
}

void Material::SetBlendingFunction(GLint func) {
    mBlendFunction = func;
    return;
}

void Material::EnableShadowVolumePass(void) {
    mDoShadowPass = true;
    return;
}

void Material::DisableShadowVolumePass(void) {
    mDoShadowPass = false;
    return;
}

void Material::SetShadowVolumeLength(float length) {
    mShadowVolumeLength = length;
    return;
}

void Material::SetShadowVolumeIntensityLow(float intensity) {
    mShadowVolumeIntensityLow = intensity;
    return;
}

void Material::SetShadowVolumeIntensityHigh(float intensity) {
    mShadowVolumeIntensityHigh = intensity;
    return;
}

void Material::SetShadowVolumeColorIntensity(float intensity) {
    mShadowVolumeColorIntensity = intensity;
    return;
}

void Material::SetShadowVolumeAngleOfView(float angle) {
    mShadowVolumeAngleOfView = angle;
    return;
}

void Material::SetShadowVolumeColor(Color color) {
    mShadowVolumeColor = color;
    return;
}

