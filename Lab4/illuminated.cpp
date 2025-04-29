#include <glad/gl.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "gladd.lib")

#include <iostream>

// Шейдеры для света:
auto lvs = R"( // вершинный
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;

    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main(){
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;  
    
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

auto lfs = R"(  // фрагментный
    #version 330 core
    out vec4 FragColor;

    in vec3 Normal;   // передан из вершинного шейдера
    in vec3 FragPos; // передан из вершинного шейдера

    uniform vec3 lightPos; 
    uniform vec3 viewPos; 
    uniform vec3 lightColor;
    uniform vec3 objectColor;
    void main() {
          // ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;
          // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
            // specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;  
        
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }  
)";

auto vs = R"(  // вершинный шейдер куба
 #version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 mvp;
void main() {
  gl_Position = mvp * vec4(aPos, 1.0);
 }
)";

auto fs = R"( // фрагментный  шейдер куба
 #version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0); // делаем белым
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

static glm::mat4 rotationMatrix = glm::mat4(1.0f); // Матрица накопленного поворота
static float lastX = 512.0f, lastY = 384.0f;
static bool mouseState = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static const float sensitivity = 0.005f;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        mouseState = true;
        return;
    }

    if (mouseState) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        mouseState = false;
        return;
    }

    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(ypos - lastY);
    lastX = (float)xpos;
    lastY = (float)ypos;

    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), -xoffset * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), -yoffset * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = rotY * rotX * rotationMatrix;
}

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
    glfwSetCursorPosCallback(window, mouse_callback);

    // 2. Инициализация glad
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // 3. Массив вершин с позициями и нормалями
    float vertices[] = {
        // позиции          // нормали
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f
    };

    // 4. Создаем VAO и VBO
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // позиция (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // нормали (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 5. Создаем шейдеры
    sh1 lightingShader(lvs, lfs);
    sh1 lampShader(vs, fs);

    // 6. Настройка камеры и проекции
    glm::vec3 cameraPos = glm::vec3(1.5f, 1.5f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);

    // 7. Позиция источника света
    glm::vec3 lightPos(1.5f, 0.0f, 1.0f);

    glm::vec3 white(1.0f);
    glm::vec3 colorCube(1.0f, 0.5f, 0.31f);

    // 8. Цикл рендеринга
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        // Обработка событий
        glfwPollEvents();

        // Очистка экрана
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- Рендер освещенного куба ---
        lightingShader.use();
        glUniform3fv(glGetUniformLocation(lightingShader.ID, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.ID, "viewPos"), 1, &cameraPos[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.ID, "lightColor"), 1, &white[0]);
        glUniform3fv(glGetUniformLocation(lightingShader.ID, "objectColor"), 1, &colorCube[0]);

        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::scale(modelCube, glm::vec3(0.8f));
        modelCube = rotationMatrix * modelCube;
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelCube));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // --- Рендер куба-лампы ---
        lampShader.use();
        glm::mat4 modelLamp = glm::mat4(1.0f);
        modelLamp = glm::translate(modelLamp, lightPos);
        modelLamp = glm::scale(modelLamp, glm::vec3(0.2f)); // маленький куб лампы
        glm::mat4 mvpLamp = projection * view * modelLamp;
        glUniformMatrix4fv(glGetUniformLocation(lampShader.ID, "mvp"), 1, GL_FALSE, glm::value_ptr(mvpLamp));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Обмен буферов
        glfwSwapBuffers(window);
    }

    // Очистка ресурсов
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}