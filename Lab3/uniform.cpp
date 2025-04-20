#include <glad/gl.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "gladd.lib")

#include <iostream>

auto vs = R"(   #version 460 core
layout(location=0)  in vec3 p;
uniform float cf;
out vec4 c;
 void main (){
    gl_PointSize=40.;
   gl_Position = vec4(p,1.);
   c=vec4( 0 , cf, 0,1.);
 }
)";


auto fs = R"(   #version 460 core
 in vec4 c;
out vec4 FragColor;
void main (){
   FragColor = c; 
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
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);

    /////// создание окна
    GLFWwindow* window = glfwCreateWindow(1800, 1200, "test", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(  // обработка нажатия на клавишу при помощи лямбды
        window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    );

    // обработка изменения размера окна
    glfwSetFramebufferSizeCallback(
        window,
        [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    );

    glfwSetCursorPosCallback(window,  // реакция на движение мыши
        [](GLFWwindow* window, double xpos, double ypos)
        {
            static float lastX = 1800. / 2.0f;
            static float lastY = 1200. / 2.0f;
            static  bool firstMouse = true;
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;

            // перевернуто, так как y-координаты идут снизу вверх
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;
        }
    );

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);   // включаем  тест глубины

    // включение управления  размером точки
    glEnable(GL_PROGRAM_POINT_SIZE);

    // белый фон
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    sh1 sh(vs, fs);

    GLuint vao;     // создание массива вершин
    glCreateVertexArrays(1, &vao);  // ver. 4.5
    glBindVertexArray(vao);

    // создание одного буфера
    GLuint vbo;
    glCreateBuffers(1, &vbo); // ver. 4.5

    glm::vec3 v[] = { {-.7,-0.3,0.} , { .7, 0.3,0.},  { 0.7, -0.7,0.} };
    GLuint index[] = { 0,1 ,2 };
    int szv = sizeof(v);
    int szi = sizeof(index);
    // Добавим толщину линии
    glLineWidth(10);

    // Выделяем память для общего хранилища
    glNamedBufferStorage(vbo, szv + szi, nullptr, GL_DYNAMIC_STORAGE_BIT);

    // размещаем данные в памяти
        // в начало буфера кладем координаты вершин
    glNamedBufferSubData(vbo, 0, szv, v);

    // затем смещаемся на   szv    и кладем индексы вершин
    glNamedBufferSubData(vbo, szv, szi, index);
    glVertexArrayElementBuffer(vao, vbo);  // связываем массив и буфер

    // привязка к идентификатору 0, начало данных - 0, шаг - sizeof(glm::vec3) 
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec3));

    // Необходимо связать массив вершин с буфером, чтобы нарисовать 
        // связываем вершины (вершина= float+float+float)
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    glEnableVertexArrayAttrib(vao, 1); // связываем индексы

    // сдвиг в буффере составляет размер вершины = sizeof(glm::vec3)
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
    glVertexArrayAttribBinding(vao, 1, 0);

    GLint color;

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        color = glGetUniformLocation(sh.ID, "cf");
        glUniform1f(color, 0.3);

        sh.use();

        glDrawElements(GL_POINTS, szi / sizeof(GLint),
        GL_UNSIGNED_INT, (void*)szv);

        glDrawElements(GL_TRIANGLES, szi / sizeof(GLint),
        GL_UNSIGNED_INT, (void*)szv);

        glfwSwapBuffers(window);  // обмен буферов
        glfwPollEvents();      // обработать сообщения окна из очереди
    }

    glDeleteBuffers(1, &vbo);  // освободить буфер(ы)
    glDeleteVertexArrays(1, &vao); // освободить массив(ы)
    glfwDestroyWindow(window);
    glfwTerminate(); // очистить ресурсы
}
