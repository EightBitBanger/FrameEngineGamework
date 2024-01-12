#include <GameEngineFramework/Renderer/components/material.h>

#define GLEW_STATIC
#include <gl/glew.h>


Material::Material() : 
    shader(nullptr),
    
    doDepthTest(true),
    doBlending(false),
    doFaceCulling(true),
    doShadowPass(true),
    
    shadowStencilLength(5),
    
    shadowStencilIntensityLow(0),
    shadowStencilIntensityHigh(1),
    
    shadowStencilColorIntensity(0),
    shadowStencilAngleOfView(0),
    
    depthFunc(MATERIAL_DEPTH_LESS),
    
    faceWinding(GL_CCW),
    faceCullSide(GL_BACK),
    
    blendSource(BLEND_SRC_COLOR),
    blendDestination(BLEND_SRC_ALPHA),
    blendAlphaSource(BLEND_ONE_MINUS_SRC_COLOR),
    blendAlphaDestination(BLEND_ONE_MINUS_SRC_ALPHA),
    blendFunction(BLEND_EQUATION_ADD)
{
    ambient = Color(0, 0, 0, 1);
    diffuse = Color(1, 1, 1, 1);
    specular = Color(0, 0, 0, 1);
    shadowStencilColor = Color(1, 1, 1);
    
    return;
}

void Material::EnableDepthTest(void) {
    doDepthTest = true;
    return;
}

void Material::DisableDepthTest(void) {
    doDepthTest = false;
    return;
}

void Material::SetDepthFunction(GLint func) {
    depthFunc = func;
    return;
}

void Material::EnableCulling(void) {
    doFaceCulling = true;
    return;
}

void Material::DisableCulling(void) {
    doFaceCulling = false;
    return;
}

void Material::SetFaceCullingSide(GLint side) {
    faceCullSide = side;
    return;
}

void Material::SetFaceWindingOrder(GLint direction) {
    faceWinding = direction;
    return;
}

void Material::EnableBlending(void) {
    doBlending = true;
    return;
}

void Material::DisableBlending(void) {
    doBlending = false;
    return;
}

void Material::SetBlending(GLint src, GLint dest)  {
    blendSource = src;
    blendDestination = dest;
    return;
}

void Material::SetBlendingAlpha(GLint srca, GLint desta) {
    blendAlphaSource = srca;
    blendAlphaDestination = desta;
    return;
}

void Material::SetBlendingFunction(GLint func) {
    blendFunction = func;
    return;
}

void Material::EnableShadowPass(void) {
    doShadowPass = true;
    return;
}

void Material::DisableShadowPass(void) {
    doShadowPass = false;
    return;
}

void Material::SetShadowStencilLength(float length) {
    shadowStencilLength = length;
    return;
}

void Material::SetShadowStencilIntensityLow(float intensity) {
    shadowStencilIntensityLow = intensity;
    return;
}

void Material::SetShadowStencilIntensityHigh(float intensity) {
    shadowStencilIntensityHigh = intensity;
    return;
}

void Material::SetShadowStencilColorIntensity(float intensity) {
    shadowStencilColorIntensity = intensity;
    return;
}

void Material::SetShadowStencilAngleOfView(float angle) {
    shadowStencilAngleOfView = angle;
    return;
}

void Material::SetShadowStencilColor(Color color) {
    shadowStencilColor = color;
    return;
}

