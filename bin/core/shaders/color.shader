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
uniform vec3 u_light_position[100];
uniform vec3 u_light_direction[100];
uniform vec4 u_light_attenuation[100];
uniform vec3 u_light_color[100];

uniform vec3 u_fogStartColor1;
uniform vec3 u_fogEndColor1;
uniform float u_fogStart1;
uniform float u_fogEnd1;
uniform float u_fogDensity1;
uniform float u_fogCutoffHeight1;
varying float v_fogFactor1;

uniform vec3 u_fogStartColor2;
uniform vec3 u_fogEndColor2;
uniform float u_fogStart2;
uniform float u_fogEnd2;
uniform float u_fogDensity2;
uniform float u_fogCutoffHeight2;
varying float v_fogFactor2;

uniform vec3 u_fogStartColor3;
uniform vec3 u_fogEndColor3;
uniform float u_fogStart3;
uniform float u_fogEnd3;
uniform float u_fogDensity3;
uniform float u_fogCutoffHeight3;
varying float v_fogFactor3;

uniform vec3 u_fogStartColor4;
uniform vec3 u_fogEndColor4;
uniform float u_fogStart4;
uniform float u_fogEnd4;
uniform float u_fogDensity4;
uniform float u_fogCutoffHeight4;
varying float v_fogFactor4;

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

    // Compute fog factor for the first fog layer
    float fogDistance1 = length(vertPos.xyz - u_eye);
    float fogRange1 = u_fogEnd1 - u_fogStart1;
    float fogFactor1 = clamp((fogDistance1 - u_fogStart1) / fogRange1, 0.0, 1.0);
    v_fogFactor1 = vertPos.y < u_fogCutoffHeight1 ? exp(-u_fogDensity1 * fogFactor1) : -1.0;

    // Compute fog factor for the second fog layer
    float fogDistance2 = length(vertPos.xyz - u_eye);
    float fogRange2 = u_fogEnd2 - u_fogStart2;
    float fogFactor2 = clamp((fogDistance2 - u_fogStart2) / fogRange2, 0.0, 1.0);
    v_fogFactor2 = vertPos.y < u_fogCutoffHeight2 ? exp(-u_fogDensity2 * fogFactor2) : -1.0;

    // Compute fog factor for the third fog layer
    float fogDistance3 = length(vertPos.xyz - u_eye);
    float fogRange3 = u_fogEnd3 - u_fogStart3;
    float fogFactor3 = clamp((fogDistance3 - u_fogStart3) / fogRange3, 0.0, 1.0);
    v_fogFactor3 = vertPos.y < u_fogCutoffHeight3 ? exp(-u_fogDensity3 * fogFactor3) : -1.0;

    // Compute fog factor for the fourth fog layer
    float fogDistance4 = length(vertPos.xyz - u_eye);
    float fogRange4 = u_fogEnd4 - u_fogStart4;
    float fogFactor4 = clamp((fogDistance4 - u_fogStart4) / fogRange4, 0.0, 1.0);
    v_fogFactor4 = vertPos.y < u_fogCutoffHeight4 ? exp(-u_fogDensity4 * fogFactor4) : -1.0;

    gl_Position = u_proj * vertPos;
}
[end]


[begin] fragment
#version 330 core

varying vec3 v_color;
varying vec2 v_coord;
varying float v_fogFactor1;
varying float v_fogFactor2;
varying float v_fogFactor3;
varying float v_fogFactor4;

uniform sampler2D u_sampler;
uniform vec3 u_fogStartColor1;
uniform vec3 u_fogEndColor1;
uniform vec3 u_fogStartColor2;
uniform vec3 u_fogEndColor2;
uniform vec3 u_fogStartColor3;
uniform vec3 u_fogEndColor3;
uniform vec3 u_fogStartColor4;
uniform vec3 u_fogEndColor4;

out vec4 color;

void main() {
    float Gamma = 2.2;

    // Original color
    vec3 originalColor = pow(v_color.rgb, vec3(1.0 / Gamma));

    // Compute final fog color for the first fog layer
    vec3 fogColor1 = mix(u_fogStartColor1, u_fogEndColor1, v_fogFactor1);
    vec3 colorWithFog1 = (v_fogFactor1 >= 0.0) ? mix(fogColor1, originalColor, v_fogFactor1) : originalColor;

    // Compute final fog color for the second fog layer
    vec3 fogColor2 = mix(u_fogStartColor2, u_fogEndColor2, v_fogFactor2);
    vec3 colorWithFog2 = (v_fogFactor2 >= 0.0) ? mix(fogColor2, colorWithFog1, v_fogFactor2) : colorWithFog1;

    // Compute final fog color for the third fog layer
    vec3 fogColor3 = mix(u_fogStartColor3, u_fogEndColor3, v_fogFactor3);
    vec3 colorWithFog3 = (v_fogFactor3 >= 0.0) ? mix(fogColor3, colorWithFog2, v_fogFactor3) : colorWithFog2;

    // Compute final fog color for the fourth fog layer
    vec3 fogColor4 = mix(u_fogStartColor4, u_fogEndColor4, v_fogFactor4);
    vec3 finalColor = (v_fogFactor4 >= 0.0) ? mix(fogColor4, colorWithFog3, v_fogFactor4) : colorWithFog3;

    color = vec4(finalColor, 1.0);
}
[end]
