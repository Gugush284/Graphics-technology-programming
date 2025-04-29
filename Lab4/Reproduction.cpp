#include <glad/gl.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "gladd.lib")

#include <iostream>

//  вершинный  шейдер
const char* vs = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

// геометрический шейдер
const char* gs = R"(
        #version 330 core
        layout (points) in;
        layout (triangle_strip, max_vertices = 3) out;
        void main() {
            vec4 pos = gl_in[0].gl_Position;
            gl_Position = pos + vec4(-0.1, 0.1, 0.0, 0.0); EmitVertex();
            gl_Position = pos + vec4(0.1, 0.1, 0.0, 0.0); EmitVertex();
            gl_Position = pos + vec4(0.0, -0.1, 0.0, 0.0); EmitVertex();
            EndPrimitive();
        }
    )";

// фрагментный шейдер
const char* fs = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";


struct  sh1 {
    unsigned int ID;
    sh1(const char* vs, const char* fs, const char* gs = nullptr) {
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;

        vertexCode = vs;
        fragmentCode = fs;

        if (gs != nullptr)
        {
            geometryCode = gs;
        }

        // ..  Продолжение
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // 2. Компилируем шейдеры
        unsigned int vertex, fragment;

        // Вершинный шейдер
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Фрагментный шейдер
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        unsigned int geometry;
        if (gs != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }

        // 3…  теперь - Шейдерная программа

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (gs != nullptr)
            glAttachShader(ID, geometry);

        glLinkProgram(ID);          // компоновка
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (gs != nullptr)
            glDeleteShader(geometry);
    } 			// конец конструктора sh1

    void use() const
    {
        glUseProgram(ID); // применение программы
    }

    // проверка на ошибки
    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;         GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                    << type << "\n" << infoLog << "\n" << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
                    << type << "\n" << infoLog << "\n" << std::endl;
            }
        }
    }

}; // конец     class  sh1

int main() {
    // 1. Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Illuminated cube and lamp", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 2. Инициализация glad
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    sh1 mySH(vs, fs, gs);

    float vertices[] = { // единственная исходная вершина
        0.0f, 0.0f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glfwSetKeyCallback(window, [](GLFWwindow*, int key, int, int, int) {
        if (key == GLFW_KEY_ESCAPE)
            exit(1);
    });

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        mySH.use(); // использовать программу
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 1); // передача данных в буфер – одна вершина

        // но за счет размножения в геометрическом шейдере нарисуется треугольник

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    return 0;
}
