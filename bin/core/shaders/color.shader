[begin] vertex

#version 330 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_color;
layout(location = 2) in vec3 l_normal;
layout(location = 3) in vec2 l_uv;

uniform mat4 u_proj;
uniform mat4 u_model;
uniform vec3 u_eye;

varying vec2 v_coord;
varying vec3 v_color;

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;

uniform int   u_light_count;
uniform vec3  u_light_position[100];
uniform vec3  u_light_attenuation[100];
uniform vec3  u_light_color[100];

void main() {
    
    vec4 vertPos = u_model * vec4(l_position, 1);
    
    vec3 norm = normalize(l_normal);
    
    vec3 lightColor = vec3(0);
    
    for (int i=0; i<=u_light_count; i++) {
        
        float intensity    = u_light_attenuation[i].r;
        float range        = u_light_attenuation[i].g;
        float attenuation  = u_light_attenuation[i].b;
        
        vec3 lightDir = normalize(u_light_position[i] - vec3(vertPos));
        
        float diff = max(dot(norm, lightDir), 0.0);
        float dist = length( u_light_position[i] - vec3(vertPos));
        
        // Specular
        vec3 viewDir = normalize(u_eye - vec3(vertPos));
        vec3 reflectDir = reflect(-lightDir, norm);  
        float shininess = 1;
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = u_light_color[i] * (spec * m_specular);
        
        if (dist > range) 
            continue;
        
        lightColor += ((diff * u_light_color[i]) * intensity) / (1.0 + (dist * attenuation)) + specular;
	lightColor = clamp(lightColor, 0.0, 2.0);
	
        continue;
    }
    
    v_color = ((m_ambient + m_diffuse) * l_color) + lightColor;
    v_coord = l_uv;
    
    gl_Position = u_proj * vertPos;
    
    return;
};

[end]



[begin] fragment

#version 330 core

varying vec3 v_color;
varying vec2 v_coord;

uniform sampler2D u_sampler;

out vec4 color;

void main() {
    
    float Gamma = 2.2;
    
    vec4 vColor = vec4(v_color, 1);
    
    color = vec4( pow(vColor.rgb, vec3(1.0/Gamma)), 1);
    
    return;
}

[end]
