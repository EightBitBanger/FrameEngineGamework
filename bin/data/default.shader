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
        
uniform int   u_light_count;
uniform vec3  u_light_position["+numberOfLights+"];
uniform vec3  u_light_attenuation["+numberOfLights+"];
uniform vec3  u_light_color["+numberOfLights+"];

void main() 
{
  
  vec4 vertPos = u_model * vec4(l_position, 1);
  float diff = 1.0;
        
  vec3 finalColor = m_ambient;
  for (int i=0; i<=u_light_count; i++) {
    
    float intensity    = u_light_attenuation[i].r;
    float range        = u_light_attenuation[i].g;
    float attenuation  = u_light_attenuation[i].b;
    
    vec3 lightDir = normalize(u_light_position[i] - vec3(vertPos));
    
    float dist = length( u_light_position[i] - vec3(vertPos));
    if (dist < range) {
      
      finalColor += ((diff * u_light_color[i]) * intensity) / (1.0 + (dist * attenuation));
      finalColor = clamp(finalColor, 0.0, 2.0);
      
    }
    
  }
  
  v_color = finalColor;
  v_coord = l_uv;
  
  gl_Position = u_proj * vertPos;
  
};

[end]



[begin] fragment

#version 330 core

uniform vec4 m_color;
varying vec3 v_color;
varying vec2 v_coord;

uniform sampler2D u_sampler;

out vec4 color;

void main()
{
  
  float Gamma = 2.2;
  
  vec4 vColor = vec4(v_color, 1);
  
  color = vec4( pow(vColor.rgb, vec3(1.0/Gamma)), 1) * texture(u_sampler, v_coord);
  
}

[end]
