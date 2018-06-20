#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da cor de cada vértice, definidas em "shader_vertex.glsl" e
// "main.cpp" (array color_coefficients).
in vec4 cor_interpolada_pelo_rasterizador;
in vec4 position_world;
in vec4 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int isGourard;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

uniform sampler2D TextureImage;

void main()
{
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 p = position_world;

    vec4 n = normalize(normal);

    vec4 l = normalize(camera_position - p);

    vec4 v = normalize(camera_position - p);

    vec4 r = normalize(2*dot(n,l)*n-l);

    vec4 Kd; // Refletância difusa
    vec4 Ks; // Refletância especular
    float q; // Expoente especular para o modelo de iluminação de Phong

    Kd = cor_interpolada_pelo_rasterizador;
    Ks = vec4(0.8,0.8,0.8,0);
    q = 32.0;
    if(cor_interpolada_pelo_rasterizador == vec4(0.2,0.2,0.2,1.0)){
        float minx = -9;
        float maxx = 9;

        float miny = 0.1;
        float maxy = 0.1;

        float minz = -4;
        float maxz = 14;

        float U =(p[0] -minx)/(maxx-minx);
        float V =(p[2] -minz)/(maxz-minz);

        Kd = texture(TextureImage, vec2(U,V)).rgba;
    }

    vec4 lambert_diffuse_term = Kd*max(dot(n,l),0);
    vec4 phong_specular_term  = Ks*pow(max(dot(r,v),0),q); // PREENCH AQUI o termo especular de Phong

    color = lambert_diffuse_term + phong_specular_term;
    //color = Kd;
    //color = n;

    if(isGourard == 1){
        color = cor_interpolada_pelo_rasterizador;
    }
    color = pow(color, vec4(1.0,1.0,1.0,1.0)/2.2);
}
