[begin] vertex

#version 330 core

layout(location = 0) in vec3 l_position;
layout(location = 1) in vec3 l_color;
layout(location = 2) in vec2 l_uv;

uniform mat4 u_proj;
uniform mat4 u_model;

varying vec2 v_coord;
varying vec3 v_color;

void main() 
{
  
  v_color = l_color;
  v_coord = l_uv;
  
  gl_Position = u_proj * u_model * vec4(l_position, 1);
  
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
