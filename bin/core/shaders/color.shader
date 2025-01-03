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

uniform vec3 m_ambient;
uniform vec3 m_diffuse;
uniform vec3 m_specular;

uniform int u_light_count;
uniform vec3 u_light_position[50];
uniform vec3 u_light_direction[50];
uniform vec4 u_light_attenuation[50];
uniform vec3 u_light_color[50];

uniform int u_fog_count;
uniform vec3 u_fogStartColor[8];
uniform vec3 u_fogEndColor[8];
uniform float u_fogStart[8];
uniform float u_fogEnd[8];
uniform float u_fogDensity[8];
uniform float u_fogCutoffHeight[8];
varying float v_fogFactor[8];

void main() {
    vec4 vertPos = u_model * vec4(l_position, 1.0);
    vec3 norm = normalize(u_inv_model * l_normal);
    vec3 lightColor = m_ambient;

    for (int i = 0; i < u_light_count; i++) {
        float intensity = u_light_attenuation[i].r;
        float range = u_light_attenuation[i].g;
        float attenuation = u_light_attenuation[i].b;
        float type = u_light_attenuation[i].a;

        if (type < 1.0) {
            float dist = length(u_light_position[i] - vec3(vertPos));
            if (dist > range) continue;

            vec3 lightDir = normalize(u_light_position[i] - vec3(vertPos));
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 viewDir = normalize(u_eye - vec3(vertPos));
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0);
            vec3 specular = u_light_color[i] * (spec * m_specular);

            lightColor += ((diff * u_light_color[i]) * intensity) / (1.0 + (dist * attenuation)) + specular;
        } else if (type < 2.0) {
            vec3 lightDir = normalize(-u_light_direction[i]);
            float diff = max(dot(norm, lightDir), 0.0);
            lightColor += (diff * u_light_color[i]) * intensity;
        }
    }

    v_color = m_diffuse * l_color * lightColor;
    v_coord = l_uv;

    // Compute fog factors for each fog layer
    for (int i = 0; i < u_fog_count; i++) {
        float fogDistance = length(vertPos.xyz - u_eye);
        float fogRange = u_fogEnd[i] - u_fogStart[i];
        float fogFactor = clamp((fogDistance - u_fogStart[i]) / fogRange, 0.0, 1.0);
        v_fogFactor[i] = vertPos.y < u_fogCutoffHeight[i] ? exp(-u_fogDensity[i] * fogFactor) : -1.0;
    }

    gl_Position = u_proj * vertPos;
}
[end]

[begin] fragment
#version 330 core

varying vec3 v_color;
varying vec2 v_coord;
varying float v_fogFactor[8];

uniform sampler2D u_sampler;
uniform int u_fog_count;
uniform vec3 u_fogStartColor[8];
uniform vec3 u_fogEndColor[8];

out vec4 color;

void main() {
    float Gamma = 2.2;

    // Original color
    vec3 originalColor = pow(v_color.rgb, vec3(1.0 / Gamma));

    vec3 finalColor = originalColor;

    // Compute final fog color for each fog layer
    for (int i = 0; i < u_fog_count; i++) {
        vec3 fogColor = mix(u_fogStartColor[i], u_fogEndColor[i], v_fogFactor[i]);
        finalColor = (v_fogFactor[i] >= 0.0) ? mix(fogColor, finalColor, v_fogFactor[i]) : finalColor;
    }

    color = vec4(finalColor, 1.0);
}
[end]
