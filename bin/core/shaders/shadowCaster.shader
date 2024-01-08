[begin] vertex

#version 330 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_color;
layout(location = 2) in vec3 l_normal;
layout(location = 3) in vec2 l_uv;

uniform mat4 u_proj;
uniform mat4 u_model;
uniform mat4 u_shadow;
uniform mat3 u_inv_model;
uniform vec3 u_eye;

varying vec2 v_coord;
varying vec4 v_color;

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;

uniform int   u_light_count;
uniform vec3  u_light_position[100];
uniform vec3  u_light_direction[100];
uniform vec4  u_light_attenuation[100];
uniform vec3  u_light_color[100];

void main() {
    
    float shadowColorHigh      = u_light_attenuation[0].r;
    float shadowColorLow       = u_light_attenuation[0].g;
    float shadowIntensityHigh  = u_light_attenuation[0].b;
    float shadowIntensityLow   = u_light_attenuation[0].a;
    
    vec3 shadowColor = u_light_color[0];
    
    v_color = vec4( shadowColor, shadowIntensityLow );
    
    vec4 shadowPos = u_model * u_shadow * vec4(l_position, 1);
    
    // Fade off height difference
    vec4 vertexPos = u_model * vec4(l_position, 1);
    
    if (shadowPos.y > (vertexPos.y - 3)) 
        v_color = vec4( vec3(shadowColorLow,  shadowColorLow,  shadowColorLow), shadowIntensityHigh );
    
    gl_Position = vec4(u_proj * shadowPos);
    
    return;
};

[end]



[begin] fragment

#version 330 core

varying vec4 v_color;
varying vec2 v_coord;

uniform sampler2D u_sampler;

out vec4 color;

void main() {
    
    color = v_color;
    
    return;
}

[end]
