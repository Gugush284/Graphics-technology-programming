#include <GL/glut.h>
#include <cmath>

constexpr double h = 5.0; // размер куба
constexpr double screen_dist = 1.0; // расстояние до экрана
constexpr double speed = 0.5; // скорость вращения
constexpr int fps = 60; // расстояние до экрана

double rotation = 0.0;

double v11, v12, v13, v21, v22, v23, v32, v33, v43; // элементы матрицы вида

// Функция для преобразования в перспективу
void perspective(double x, double y, double z, double *pX, double *pY) {
    double xe, ye, ze;

    // Координаты глаза
    xe = v11 * x + v21 * y;
    ye = v12 * x + v22 * y + v32 * z;
    ze = v13 * x + v23 * y + v33 * z + v43;

    // Экранные координаты
    *pX = screen_dist * xe / ze;
    *pY = screen_dist * ye / ze;
}

// Функция для рисования линии
void dw(double x1, double y1, double z1, double x2, double y2, double z2) {
    double X1, Y1, X2, Y2;

    perspective(x1, y1, z1, &X1, &Y1);
    perspective(x2, y2, z2, &X2, &Y2);

    glBegin(GL_LINES);
        glVertex2d(X1, Y1);
        glVertex2d(X2, Y2);
    glEnd();
}

// Функция для вычисления матрицы преобразования
void coeff(double rho, double theta, double phi) {
    double th, ph, costh, sinth, cosph, sinph, factor;
    factor = atan(1.0) / 45.0; // коэффициент для перевода в радианы
    th = theta * factor; 
    ph = phi * factor;

    costh = cos(th); 
    sinth = sin(th);
    cosph = cos(ph); 
    sinph = sin(ph);

    // элементы матрицы видового преобразования V
    v11 = -sinth;
    v12 = -cosph * costh;
    v13 = -sinph * costh;
    v21 = costh;
    v22 = -cosph * sinth;
    v23 = -sinph * sinth;
    v32 = sinph;
    v33 = -cosph;
    v43 = rho;
}

// Функция отображения
void Display(void) {
    const double rho = 15.0, phi = 80.0;
    double theta = 50.0 + rotation;
    coeff(rho, theta, phi);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(0, 255, 0);

    // Рисование рёбер куба
    dw(h, -h, -h, h, h, -h); // ребро
    dw(h, h, -h, -h, h, -h); 
    dw(-h, h, -h, -h, -h, -h); 
    dw(-h, -h, -h, h, -h, -h); 
    dw(h, -h, h, h, h, h); 
    dw(h, h, h, -h, h, h);
    dw(-h, h, h, -h, -h, h); 
    dw(-h, -h, h, h, -h, h); 
    dw(h, -h, -h, h, -h, h); 
    dw(h, h, -h, h, h, h); 
    dw(-h, h, -h, -h, h, h); 
    dw(-h, -h, -h, -h, -h, h);

    glFinish();
    glutSwapBuffers();
    glutPostRedisplay();
}

// Функция для обработки изменения размера окна
void Reshape(GLint w, GLint h) {
    glViewport(0, 0, w, h);
}

void Timer(int value){
	rotation += speed;
	if (rotation > 360)
		rotation -= 360;

	glutPostRedisplay();
	glutTimerFunc(1000 / fps, Timer, 0);
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv); // инициализация GLUT
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowPosition(50, 10);
    glutInitWindowSize(640, 480); // создаем окно
    glutCreateWindow("Cube");

    glutDisplayFunc(Display); // функция отображения
    glutReshapeFunc(Reshape); // функция изменения размера
    glutTimerFunc(0, Timer, 0);

    glutMainLoop(); // основной цикл GLUT

    return 0;
}
