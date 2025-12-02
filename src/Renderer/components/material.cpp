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
    
    mBlendSource(BLEND_SRC_ALPHA),
    mBlendDestination(BLEND_ONE_MINUS_SRC_ALPHA),
    mBlendAlphaSource(BLEND_ONE_MINUS_SRC_COLOR),
    mBlendAlphaDestination(BLEND_ONE_MINUS_SRC_ALPHA),
    mBlendFunction(BLEND_EQUATION_ADD)
{
    ambient = Color(0, 0, 0, 1);
    diffuse = Color(1, 1, 1, 1);
    specular = Color(0, 0, 0, 1);
    mShadowVolumeColor = Color(1, 1, 1);
}

void Material::EnableDepthTest(void) {
    mDoDepthTest = true;
}

void Material::DisableDepthTest(void) {
    mDoDepthTest = false;
}

void Material::SetDepthFunction(GLint func) {
    mDepthFunc = func;
}

void Material::EnableCulling(void) {
    mDoFaceCulling = true;
}

void Material::DisableCulling(void) {
    mDoFaceCulling = false;
}

void Material::SetFaceCullingSide(GLint side) {
    mFaceCullSide = side;
}

void Material::SetFaceWindingOrder(GLint direction) {
    mFaceWinding = direction;
}

void Material::EnableBlending(void) {
    mDoBlending = true;
}

void Material::DisableBlending(void) {
    mDoBlending = false;
}

void Material::SetBlending(GLint src, GLint dest)  {
    mBlendSource = src;
    mBlendDestination = dest;
}

void Material::SetBlendingAlpha(GLint srca, GLint desta) {
    mBlendAlphaSource = srca;
    mBlendAlphaDestination = desta;
}

void Material::SetBlendingFunction(GLint func) {
    mBlendFunction = func;
}

void Material::EnableShadowVolumePass(void) {
    mDoShadowPass = true;
}

void Material::DisableShadowVolumePass(void) {
    mDoShadowPass = false;
}

void Material::SetShadowVolumeLength(float length) {
    mShadowVolumeLength = length;
}

void Material::SetShadowVolumeIntensityLow(float intensity) {
    mShadowVolumeIntensityLow = intensity;
}

void Material::SetShadowVolumeIntensityHigh(float intensity) {
    mShadowVolumeIntensityHigh = intensity;
}

void Material::SetShadowVolumeColorIntensity(float intensity) {
    mShadowVolumeColorIntensity = intensity;
}

void Material::SetShadowVolumeAngleOfView(float angle) {
    mShadowVolumeAngleOfView = angle;
}

void Material::SetShadowVolumeColor(Color color) {
    mShadowVolumeColor = color;
}
