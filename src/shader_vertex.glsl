#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 color_coefficients;
layout (location = 2) in vec4 normal_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int isGourard;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 normal;
out vec4 cor_interpolada_pelo_rasterizador;

void main()
{

    gl_Position = projection * view * model * model_coefficients;
    position_world = model * model_coefficients;
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    //gourard = isGourard;

    if(isGourard == 1){
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;

        vec4 p = position_world;

        vec4 n = normalize(normal);

        vec4 l = normalize(camera_position - p);

        vec4 v = normalize(camera_position - p);

        vec4 r = normalize(2*dot(n,l)*n-l);

        vec4 Kd = color_coefficients;
        vec4 Ks = vec4(0.8,0.8,0.8,0);
        float q = 32.0;

        vec4 lambert_diffuse_term = Kd*max(dot(n,l),0);
        //vec4 phong_specular_term  = Ks*pow(max(dot(r,v),0),q); // PREENCH AQUI o termo especular de Phong

        vec4 color = lambert_diffuse_term;
        //color = pow(color, vec4(1.0,1.0,1.0,1.0)/2.2);


        cor_interpolada_pelo_rasterizador = color;
    }else{
        cor_interpolada_pelo_rasterizador = color_coefficients;
    }
}
