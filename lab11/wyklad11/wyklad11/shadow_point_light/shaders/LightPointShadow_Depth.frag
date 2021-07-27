// Reference: https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
#version 330 core
in vec4 FragPos;

uniform vec3  LightPosition;
uniform float FarPlane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - LightPosition);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / FarPlane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
