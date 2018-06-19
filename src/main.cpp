#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"
#include "matrices.h"
#include <iostream>
#include <vector>
#include "Carro.h"
#include <tiny_obj_loader.h>

using namespace std;

struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando modelo \"%s\"... ", filename);

        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        printf("OK.\n");
    }
};

GLuint BuildCubo(); // Constrói triângulos para renderização
GLuint BuildCar(); // Constrói triângulos para renderização
GLuint BuildChao(); // Constrói triângulos para renderização
GLuint BuildPista(); // Constrói triângulos para renderização
GLuint BuildCow(); // Constrói triângulos para renderização
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU

void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

struct SceneObject
{
    const char*  name;        // Nome do objeto
    void*        first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
};

std::map<const char*, SceneObject> g_VirtualScene;

float g_ScreenRatio = 1.0f;

float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

bool g_LeftMouseButtonPressed = false;

float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 2.5f; // Distância da câmera para a origem

bool g_UsePerspectiveProjection = true;

bool camera_lookat = true;

glm::vec4 camera_position_c; // Ponto "c", centro da câmera
glm::vec4 camera_lookat_l; // Ponto "l", para onde a câmera (look-at) estará sempre olhando
glm::vec4 camera_view_vector; // Vetor "view", sentido para onde a câmera está virada
glm::vec4 camera_up_vector; // Vetor "up" fixado para apontar para o "céu" (eito Y global)

bool g_ShowInfoText = true;

Carro car;

void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<GLushort> &elements)
{
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Não pode abrir o arquivo: " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            v.w = 1.0f;
            vertices.push_back(v);
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));

            GLushort a,b,c, d;
            char ch;
            int n;
            s >> a;
            s >> ch;
            s >>ch;
            s >> n;
            s >> b;
            s >> ch;
            s >>ch;
            s >> n;
            s >> c;
            s >> ch;
            s >>ch;
            s >> n;
            s >> d;
            a--;
            b--;
            c--;
            d--;
            elements.push_back(a);
            elements.push_back(b);
            elements.push_back(c);
            elements.push_back(a);
            elements.push_back(c);
            elements.push_back(d);
        }
    }
    printf("Worked\n");

}

void load_obj2(const char* filename, vector<glm::vec4> &vertices, vector<GLushort> &elements)
{
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Não pode abrir o arquivo: " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v;
            s >> v.x;
            s >> v.y;
            s >> v.z;
            v.w = 1.0f;
            vertices.push_back(v);
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));

            GLushort a,b,c, d;
            char ch;
            int n;
            s >> a;
            s >> b;
            s >> c;
            a--;
            b--;
            c--;
            elements.push_back(a);
            elements.push_back(b);
            elements.push_back(c);
        }
    }

}

int main()
{
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(ErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(800, 800, "INF01047 - 00274704 - Matheus Alan Bergmann - 00274719 - Henrique Soares Goetz", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 800); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    GLuint program_id = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    GLuint vertex_array_object_id = BuildCar();
    GLuint vertex_array_object_id2 = BuildChao();
    GLuint vertex_array_object_id3 = BuildPista();
    GLuint vertex_array_object_id4 = BuildCubo();
    GLuint vertex_array_object_id5 = BuildCow();

    //TextRendering_Init();

    GLint model_uniform           = glGetUniformLocation(program_id, "model"); // Variável da matriz "model"
    GLint view_uniform            = glGetUniformLocation(program_id, "view"); // Variável da matriz "view" em shader_vertex.glsl
    GLint projection_uniform      = glGetUniformLocation(program_id, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    GLint render_as_black_uniform = glGetUniformLocation(program_id, "render_as_black"); // Variável booleana em shader_vertex.glsl

    glEnable(GL_DEPTH_TEST);

    glm::mat4 the_projection;
    glm::mat4 the_model;
    glm::mat4 the_view;

    float r = g_CameraDistance;
    float y = r*sin(g_CameraPhi);
    float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
    float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

    camera_position_c  = glm::vec4(0,0,0,1.0f); // Ponto "c", centro da câmera
    camera_lookat_l    = glm::vec4(0.0f,0.0f,1.0f,1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
    camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a câmera está virada
    camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)

    while (!glfwWindowShouldClose(window))
    {
        if(camera_lookat){
            camera_position_c = car.getCameraPosition();
            camera_view_vector = car.getCameraView();
        }
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program_id);

        glBindVertexArray(vertex_array_object_id);

        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);

        glm::mat4 projection;

        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -40.0f; // Posição do "far plane"

        float field_of_view = 3.141592 / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));
        glm::mat4 model;

        model = car.getMatrix();
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["carro"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["carro"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["carro"].first_index
        );


        glBindVertexArray(vertex_array_object_id2);

        model = Matrix_Translate(0,0,5);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["chao"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["chao"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["chao"].first_index
        );

        glBindVertexArray(vertex_array_object_id3);

        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        model = Matrix_Translate(0,0,5);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glDrawElements(
            g_VirtualScene["pista"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["pista"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["pista"].first_index
        );

        ///////////
        glBindVertexArray(vertex_array_object_id4);

        model = model
            *Matrix_Translate(0,0.5,-9.5)
            *Matrix_Scale(20,1,1);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );

        /////////////
         glBindVertexArray(vertex_array_object_id4);

        model = Matrix_Identity()
            *Matrix_Translate(0,0.5,14.5)
            *Matrix_Scale(20,1,1);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );

        /////////////
         glBindVertexArray(vertex_array_object_id4);

        model = Matrix_Identity()
            *Matrix_Translate(-9.5,0.5,5)
            *Matrix_Scale(1,1,18);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );

        /////////////

        glBindVertexArray(vertex_array_object_id4);

        model = Matrix_Identity()
            *Matrix_Translate(+9.5,0.5,5)
            *Matrix_Scale(1,1,18);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );

        /////////////
        //Internas

        glBindVertexArray(vertex_array_object_id4);

        model = Matrix_Identity()
            *Matrix_Translate(0,0.5,0.5)
            *Matrix_Scale(10,1,1);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );

        /////////////

        glBindVertexArray(vertex_array_object_id4);

        model = Matrix_Identity()
            *Matrix_Translate(0,0.5,9.5)
            *Matrix_Scale(10,1,1);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );

        /////////////
        glBindVertexArray(vertex_array_object_id4);

        model = Matrix_Identity()
            *Matrix_Translate(-4.5,0.5,5)
            *Matrix_Scale(1,1,8);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );

        /////////////
        glBindVertexArray(vertex_array_object_id4);

        model = Matrix_Identity()
            *Matrix_Translate(4.5,0.5,5)
            *Matrix_Scale(1,1,8);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cubo"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cubo"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cubo"].first_index
        );
        /////////////
        //COW
        glBindVertexArray(vertex_array_object_id5);

        model = Matrix_Identity()
            *Matrix_Translate(0,0.5,5);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glUniform1i(render_as_black_uniform, false);

        glDrawElements(
            g_VirtualScene["cow"].rendering_mode, // Veja slide 160 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["cow"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["cow"].first_index
        );
        /////////////


        model = Matrix_Identity();

        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(0);

        glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();

    return 0;
}

void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gourad, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice.

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            // PREENCHA AQUI o cálculo da normal de um triângulo cujos vértices
            // estão nos pontos "a", "b", e "c", definidos no sentido anti-horário.
            const glm::vec4  n = crossproduct(b-a, c-b);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3*i + 0] = n.x;
        model->attrib.normals[3*i + 1] = n.y;
        model->attrib.normals[3*i + 2] = n.z;
    }
}

GLuint BuildCubo()
{
    // Primeiro, definimos os atributos de cada vértice.

    // A posição de cada vértice é definida por coeficientes em um sistema de
    // coordenadas local de cada modelo geométrico. Note o uso de coordenadas
    // homogêneas.  Veja as seguintes referências:
    //
    //  - slide 32  do documento "Aula_08_Sistemas_de_Coordenadas.pdf";
    //  - slide 147 do documento "Aula_08_Sistemas_de_Coordenadas.pdf";
    //  - slide 144 do documento "Aula_09_Projecoes.pdf".
    //
    // Este vetor "model_coefficients" define a GEOMETRIA (veja slide 112 do
    // documento "Aula_04_Modelagem_Geometrica_3D.pdf").
    //
    GLfloat model_coefficients[] =
    {
        // Vértices de um cubo
        //    X      Y     Z     W
        -0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 0
        -0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 1
        0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 2
        0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 3
        -0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 4
        -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 5
        0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 6
        0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 7
        // Vértices para desenhar o eixo X
        //    X      Y     Z     W
        0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 8
        1.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 9
        // Vértices para desenhar o eixo Y
        //    X      Y     Z     W
        0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 10
        0.0f,  1.0f,  0.0f, 1.0f, // posição do vértice 110.0f, 0.0f
        // Vértices para desenhar o eixo Z
        //    X      Y     Z     W
        0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 12
        0.0f,  0.0f,  1.0f, 1.0f, // posição do vértice 13
    };

    // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
    // um buffer de memória que irá conter os valores de um certo atributo de
    // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
    // de textura.  Neste exemplo utilizaremos vários VBOs, um para cada tipo de atributo.
    // Agora criamos um VBO para armazenarmos um atributo: posição.
    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
    // contém a definição de vários atributos de um certo conjunto de vértices;
    // isto é, um VAO irá conter ponteiros para vários VBOs.
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
    // está contido na variável "vertex_array_object_id".
    glBindVertexArray(vertex_array_object_id);

    // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
    // variável VBO_model_coefficients_id será modificado a seguir. A
    // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
    // irá conter atributos de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
    // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
    // para alocar um número de bytes exatamente igual ao tamanho ("size")
    // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
    // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
    // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
    // também dizemos que tais dados serão utilizados para renderizar ou
    // desenhar ("DRAW").  Pense que:
    //
    //            glBufferData()  ==  malloc() do C  ==  new do C++.
    //
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    // Finalmente, copiamos os valores do array model_coefficients para dentro do
    // VBO "ligado" acima.  Pense que:
    //
    //            glBufferSubData()  ==  memcpy() do C.
    //
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    // Precisamos então informar um índice de "local" ("location"), o qual será
    // utilizado no shader "shader_vertex.glsl" para acessar os valores
    // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
    // coeficientes) destes atributos. Como em nosso caso são pontos em coordenadas
    // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isso define
    // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
    // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
    // flutuante com 32 bits (GL_FLOAT).
    // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
    // está dentro do VAO "ligado" acima por glBindVertexArray().
    // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    // "Ativamos" os atributos. Informamos que os atributos com índice de local
    // definido acima, na variável "location", deve ser utilizado durante o
    // rendering.
    glEnableVertexAttribArray(location);

    // "Desligamos" o VBO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Agora repetimos todos os passos acima para atribuir um novo atributo a
    // cada vértice: uma cor (veja slide 113 do documento "Aula_04_Modelagem_Geometrica_3D.pdf").
    // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
    // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
    // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
    GLfloat color_coefficients[] =
    {
        // Cores dos vértices do cubo
        //  R     G     B     A
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 0
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 1
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 2
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 3
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 4
        1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 5
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 6
        0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 7
        // Cores para desenhar o eixo X
        1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 8
        1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 9
        // Cores para desenhar o eixo Y
        0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 10
        0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 11
        // Cores para desenhar o eixo Z
        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 12
        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 13
    };
    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vamos então definir polígonos utilizando os vértices do array
    // model_coefficients.
    //
    // Para referência sobre os modos de renderização, veja slide 160 do
    // documento "Aula_04_Modelagem_Geometrica_3D.pdf".
    //
    // Este vetor "indices" define a TOPOLOGIA (veja slide 112 do documento
    // "Aula_04_Modelagem_Geometrica_3D.pdf").
    //
    GLuint indices[] =
    {
        // Definimos os índices dos vértices que definem as FACES de um cubo
        // através de 12 triângulos que serão desenhados com o modo de renderização
        // GL_TRIANGLES.
        0, 1, 2, // triângulo 1
        7, 6, 5, // triângulo 2
        3, 2, 6, // triângulo 3
        4, 0, 3, // triângulo 4
        4, 5, 1, // triângulo 5
        1, 5, 6, // triângulo 6
        0, 2, 3, // triângulo 7
        7, 5, 4, // triângulo 8
        3, 6, 7, // triângulo 9
        4, 3, 7, // triângulo 10
        4, 1, 0, // triângulo 11
        1, 6, 2, // triângulo 12
        // Definimos os índices dos vértices que definem as ARESTAS de um cubo
        // através de 12 linhas que serão desenhadas com o modo de renderização
        // GL_LINES.
        0, 1, // linha 1
        1, 2, // linha 2
        2, 3, // linha 3
        3, 0, // linha 4
        0, 4, // linha 5
        4, 7, // linha 6
        7, 6, // linha 7
        6, 2, // linha 8
        6, 5, // linha 9
        5, 4, // linha 10
        5, 1, // linha 11
        7, 3, // linha 12
        // Definimos os índices dos vértices que definem as linhas dos eixos X, Y,
        // Z, que serão desenhados com o modo GL_LINES.
        8, 9,   // linha 1
        10, 11, // linha 2
        12, 13  // linha 3
    };

    // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
    // coloridas do cubo.
    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = 36;       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cubo"] = cube_faces;

    // Criamos um segundo objeto virtual (SceneObject) que se refere às arestas
    // pretas do cubo.
    SceneObject cube_edges;
    cube_edges.name           = "Cubo (arestas pretas)";
    cube_edges.first_index    = (void*)(36*sizeof(GLuint)); // Primeiro índice está em indices[36]
    cube_edges.num_indices    = 24; // Último índice está em indices[59]; total de 24 índices.
    cube_edges.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cube_edges"] = cube_edges;

    // Criamos um terceiro objeto virtual (SceneObject) que se refere aos eixos XYZ.
    SceneObject axes;
    axes.name           = "Eixos XYZ";
    axes.first_index    = (void*)(60*sizeof(GLuint)); // Primeiro índice está em indices[60]
    axes.num_indices    = 6; // Último índice está em indices[65]; total de 6 índices.
    axes.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES.
    g_VirtualScene["axes"] = axes;

    // Criamos um buffer OpenGL para armazenar os índices acima
    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

    // Alocamos memória para o buffer.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

    // Copiamos os valores do array indices[] para dentro do buffer.
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

    // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
    // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
    // caso contrário o VAO irá perder a informação sobre os índices.
    //
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);

    // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
    // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
    return vertex_array_object_id;
}



GLuint BuildPista()
{

    GLfloat model_coefficients[] =
    {
        // Vértices do chao
        //    X      Y     Z     W
        5.0f, 0.1f, 5.0f, 1.0f, // posição do vértice 1
        9.0f, 0.1f, 5.0f,1.0f,
        5.0f, 0.1f, -5.0f,1.0f,
        9.0f, 0.1f, -5.0f,1.0f,
        -5.0f, 0.1f, 5.0f, 1.0f, // posição do vértice 1
        -9.0f, 0.1f, 5.0f,1.0f,
        -5.0f, 0.1f, -5.0f,1.0f,
        -9.0f, 0.1f, -5.0f,1.0f,
        9.0f,0.1f,9.0f,1.0f,
        -9.0f,0.1f,9.0f,1.0f,
        9.0f,0.1f,-9.0f,1.0f,
        -9.0f,0.1f,-9.0f,1.0f
    };

    GLfloat color_coefficients[]=
    {
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1 // cor do vértice 2
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1 // cor do vértice 3
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1 // cor do vértice 2
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1
        0.2f, 0.2f, 0.2f, 1.0f,
        0.2f, 0.2f, 0.2f, 1.0f,
        0.2f, 0.2f, 0.2f, 1.0f, // cor do vértice 1
        0.2f, 0.2f, 0.2f, 1.0f,
        0.2f, 0.2f, 0.2f, 1.0f
    };

    GLuint indices[]=
    {
        0,1,2,1,2,3,4,5,6,5,6,7,1,8,9,1,9,5,3,10,11,3,11,7

    };

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);

    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);

    glBindVertexArray(vertex_array_object_id);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(location);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = 24;       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["pista"] = cube_faces;

    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
    glBindVertexArray(0);

    return vertex_array_object_id;

}

GLuint BuildChao()
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    GLfloat model_coefficients[] =
    {
        // Vértices do chao
        //    X      Y     Z     W
        10.0f,  0.0f, 10.0f, 1.0f, // posição do vértice 1
        -10.0f, 0.0f,  10.0f, 1.0f, // posição do vértice 2
        10.0f, 0.0f,  -10.0f, 1.0f, // posição do vértice 3
        -10.0f,  0.0f,  -10.0f, 1.0f // posição do vértice 4
    };

    GLfloat color_coefficients[]=
    {
        0.5f, 0.5f, 0.5f, 1.0f, // cor do vértice 1
        0.5f, 0.5f, 0.5f, 1.0f, // cor do vértice 2
        0.5f, 0.5f, 0.5f, 1.0f, // cor do vértice 3
        0.5f, 0.5f, 0.5f, 1.0f
    };

    GLuint indices[]=
    {
        0,1,2,1,2,3
    };

    float normal_coefficients[]=
    {
        0,1,0,0,
        0,1,0,0,
        0,1,0,0,
        0,1,0,0,
    };



    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO_normal_coefficients_id;
    glGenBuffers(1, &VBO_normal_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_coefficients), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normal_coefficients), normal_coefficients);
    location = 2; // "(location = 2)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = 6;       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["chao"] = cube_faces;

    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
    glBindVertexArray(0);

    return vertex_array_object_id;

}

GLuint BuildCar()
{

    char* filename = "../../utilities/Car.obj";
    ObjModel model(filename);

    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  color_coefficients;

    //ComputeNormals(&model);

    for (size_t shape = 0; shape < model.shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model.shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model.shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model.shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model.attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model.attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model.attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model.attrib.normals[3*idx.normal_index + 0];
                    const float ny = model.attrib.normals[3*idx.normal_index + 1];
                    const float nz = model.attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }
                color_coefficients.push_back(1);
                color_coefficients.push_back(0);
                color_coefficients.push_back(0);
                color_coefficients.push_back(1);
            }
        }
    }

    cout << sizeof(indices) << endl;

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, color_coefficients.size()*sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, color_coefficients.size()*sizeof(float), color_coefficients.data());
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO_normal_coefficients_id;
    glGenBuffers(1, &VBO_normal_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
    location = 2; // "(location = 2)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = indices.size();       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

// Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["carro"] = cube_faces;

    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size()*sizeof(GLuint), indices.data());
    glBindVertexArray(0);

    return vertex_array_object_id;
}

GLuint BuildCow()
{
    char* filename = "../../utilities/cow.obj";
    ObjModel model(filename);

    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  color_coefficients;

    ComputeNormals(&model);

    for (size_t shape = 0; shape < model.shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model.shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model.shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model.shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model.attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model.attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model.attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model.attrib.normals[3*idx.normal_index + 0];
                    const float ny = model.attrib.normals[3*idx.normal_index + 1];
                    const float nz = model.attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }
                color_coefficients.push_back(1);
                color_coefficients.push_back(0);
                color_coefficients.push_back(0);
                color_coefficients.push_back(1);
            }
        }
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO_color_coefficients_id;
    glGenBuffers(1, &VBO_color_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, color_coefficients.size()*sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, color_coefficients.size()*sizeof(float), color_coefficients.data());
    location = 1; // "(location = 1)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO_normal_coefficients_id;
    glGenBuffers(1, &VBO_normal_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
    location = 2; // "(location = 2)" em "shader_vertex.glsl"
    number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    SceneObject cube_faces;
    cube_faces.name           = "Cubo (faces coloridas)";
    cube_faces.first_index    = (void*)0; // Primeiro índice está em indices[0]
    cube_faces.num_indices    = indices.size();       // Último índice está em indices[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

// Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
    g_VirtualScene["cow"] = cube_faces;

    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size()*sizeof(GLuint), indices.data());
    glBindVertexArray(0);

    return vertex_array_object_id;
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try
    {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    }
    catch ( std::exception& e )
    {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retornamos o ID gerado acima
    return program_id;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    g_ScreenRatio = (float)width / height;
}

double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        g_LeftMouseButtonPressed = false;
    }
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{

    if (!g_LeftMouseButtonPressed)
        return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    // Atualizamos parâmetros da câmera com os deslocamentos
    g_CameraTheta -= 0.01f*dx;
    g_CameraPhi   += 0.01f*dy;

    camera_view_vector = Matrix_Rotate_Y(0.005f*dx) * camera_view_vector;
    camera_view_vector = Matrix_Rotate_X(0.005f*dy) * camera_view_vector;

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    g_CameraDistance -= 0.1f*yoffset;

    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (g_CameraDistance < verysmallnumber)
        g_CameraDistance = verysmallnumber;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    float delta = 3.141592 / 16; // 22.5 graus, em radianos.

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        car.moveCarro(glfwGetTime());
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        car.moveCarBack();
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        car.turnLeft();
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        car.turnRight();
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        camera_lookat = !camera_lookat;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        camera_position_c += camera_view_vector;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        camera_position_c -= camera_view_vector;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        glm::mat4 rotation = Matrix_Rotate_Y(0.3);
        camera_view_vector = rotation * camera_view_vector;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        glm::mat4 rotation = Matrix_Rotate_Y(-0.3);
        camera_view_vector = rotation * camera_view_vector;
    }

}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
)
{
    if ( !g_ShowInfoText )
        return;

    glm::vec4 p_world = model*p_model;
    glm::vec4 p_camera = view*p_world;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     World", -1.0f, 1.0f-pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f-2*pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     Camera", -1.0f, 1.0f-7*pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f-8*pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                   NDC", -1.0f, 1.0f-13*pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f-14*pad, 1.0f);
}

// Escrevemos na tela os ângulos de Euler definidos nas variáveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f+pad/10, -1.0f+2*pad/10, 1.0f);
}

// Escrevemos na tela qual matriz de projeção está sendo utilizada.
void TextRendering_ShowProjection(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if ( g_UsePerspectiveProjection )
        TextRendering_PrintString(window, "Perspective", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
    else
        TextRendering_PrintString(window, "Orthographic", 1.0f-13*charwidth, -1.0f+2*lineheight/10, 1.0f);
}

// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    if ( !g_ShowInfoText )
        return;

    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}
