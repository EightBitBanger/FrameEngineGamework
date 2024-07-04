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
uniform vec3 u_angle;

varying vec2 v_coord;
varying vec3 v_color;
varying vec3 v_ambient;

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;

uniform int   u_light_count;
uniform vec3  u_light_position[100];
uniform vec3  u_light_direction[100];
uniform vec4  u_light_attenuation[100];
uniform vec3  u_light_color[100];

void main() {
    
    vec4 vertPos = u_model * vec4(l_position, 1);
    
    v_color = l_color;
    v_ambient = m_ambient;
    
    v_coord = l_uv;
    
    gl_Position = u_proj * vertPos;
    
    return;
}

[end]



[begin] fragment

#version 330 core

varying vec2 v_coord;
varying vec3 v_color;
varying vec3 v_ambient;

uniform sampler2D u_sampler;

out vec4 color;

void main() {
    
    vec4 texColor = texture(u_sampler, v_coord);
    
    if (texColor.a < v_ambient.r) 
        discard;
    
    color = vec4(texColor.grb, v_ambient.g) * vec4(v_color, 1);
    
    return;
}

[end]
