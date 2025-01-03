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
varying vec4 v_color;

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;

uniform int   u_light_count;
uniform vec3  u_light_position[1];
uniform vec3  u_light_direction[1];
uniform vec4  u_light_attenuation[1];
uniform vec3  u_light_color[1];

void main() {
    
    float shadowAngleOfView    = u_light_attenuation[0].r;
    float shadowColorIntensity = u_light_attenuation[0].g;
    float shadowIntensityHigh  = u_light_attenuation[0].b;
    float shadowIntensityLow   = u_light_attenuation[0].a;
    
    vec4 shadowPos = u_model * u_shadow * vec4(l_position, 1);
    vec4 shadowOrig = u_shadow * vec4(l_position, 1);
    
    vec3 viewDir = normalize(u_angle);
    
    vec3 lightDirection = normalize(u_light_direction[0]);
    
    // Intensify along the light angle
    float spec = min(pow(dot(viewDir, lightDirection), shadowAngleOfView), 1.0);
    
    // Apply fade based on shadowIntensityHigh and shadowIntensityLow
    float fade = mix(0.0f, shadowColorIntensity, spec);
    
    // Calculate the fade effect along the shadow's length
    float fadeOut = 1.0 - shadowOrig.z / shadowOrig.w; // Assuming z/w gives the length of the shadow
    float attenuation = fade / fadeOut;
    
    v_color = vec4(u_light_color[0], attenuation);
    
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
