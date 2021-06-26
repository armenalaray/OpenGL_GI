#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{
    if(hdr)
    {
        //Reinhard tonemapping

        //const float gamma = 2.2;
        //vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
        //vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
        //mapped = pow(mapped, vec3(1.0 / gamma));
        //FragColor = vec4(mapped, 1.0);

        const float gamma = 2.2;
        vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
        vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
        mapped = pow(mapped, vec3(1.0/gamma));
        FragColor = vec4(mapped, 1.0);
    }
    else
    {
        //const float gamma = 2.2;
        vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
        //vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
        //mapped = pow(mapped, vec3(1.0 / gamma));
        FragColor = vec4(hdrColor, 1.0);
    }
    
}  