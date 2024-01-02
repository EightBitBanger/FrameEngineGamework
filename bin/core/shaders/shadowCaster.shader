[begin] vertex

#version 440 core

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
    
    vec3 norm = u_inv_model * normalize(l_normal);
    
    vec3 lightColor = vec3(0);
    
    for (int i=0; i < u_light_count; i++) {
        
        float intensity    = u_light_attenuation[i].r;
        float range        = u_light_attenuation[i].g;
        float attenuation  = u_light_attenuation[i].b;
        float type         = u_light_attenuation[i].a;
        
        // Ignore point lights
        if (type < 1) 
            continue;
        
        
        // Directional light shadows
        if (type < 2) {
            
            float shadowOffset     = 1.1;
            float shadowColor      = 0.03;
            float shadowIntensity  = 0.8;
            float shadowLength     = 1;
            
            //mat4 shadowScale;
            
            vec3 lightDir = normalize(-u_light_direction[i]);
            
            v_color = vec4(shadowColor, shadowColor, shadowColor, shadowIntensity);
            
            vec4 vertPos = u_model * u_shadow * vec4(l_position, 1);
            
            gl_Position = vec4(u_proj * vertPos);
            
            return;
        }
        
        continue;
    }
    
    gl_Position = vec4(0, 0, 0, 0);
    
    return;
};

[end]



[begin] fragment

#version 440 core

varying vec4 v_color;
varying vec2 v_coord;

uniform sampler2D u_sampler;

out vec4 color;

void main() {
    
    color = v_color;
    
    return;
}

[end]
