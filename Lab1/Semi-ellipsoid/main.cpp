#include <GL/glut.h>
#include <cmath>
#include <vector>

constexpr double screen_dist = 1.0; // Расстояние до экрана

double v11, v12, v13, v21, v22, v23, v32, v33, v43; // элементы матрицы вида
double rho = 15.0, phi = 80.0, theta = 50.0;

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

void dw(double x1, double y1, double z1, double x2, double y2, double z2) {
    double X1, Y1, X2, Y2;

    perspective(x1, y1, z1, &X1, &Y1);
    perspective(x2, y2, z2, &X2, &Y2);

    glBegin(GL_LINES);
        glVertex2d(X1, Y1);
        glVertex2d(X2, Y2);
    glEnd();
}

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

void Display(void) {
    glClearColor(255, 255, 255, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(255, 0, 0);

    // Параметры эллипсоида
    double a = 10.0, b = 8.0, c = 6.0;
    int n = 50; // количество сегментов
	int i, j;

    // Генерация точек и рёбер эллипсоида
    for (i = 0; i < n; i++) {
        double theta1 = M_PI * i / 2 / (n - 1);  // Углы для сегментов
        double theta2 = M_PI * (i + 1) / 2 / (n - 1);

        for (j = 0; j < 2 * n; j++) {
            double phi1 = 2 * M_PI * j / (2 * n);  // Углы для сегментов
            double phi2 = 2 * M_PI * (j + 1) / (2 * n);

            // Точки для двух соседних сегментов
            double x1 = a * sin(theta1) * cos(phi1);
            double y1 = b * sin(theta1) * sin(phi1);
            double z1 = c * cos(theta1);

            double x2 = a * sin(theta1) * cos(phi2);
            double y2 = b * sin(theta1) * sin(phi2);
            double z2 = c * cos(theta1);

            double x3 = a * sin(theta2) * cos(phi2);
            double y3 = b * sin(theta2) * sin(phi2);
            double z3 = c * cos(theta2);

            double x4 = a * sin(theta2) * cos(phi1);
            double y4 = b * sin(theta2) * sin(phi1);
            double z4 = c * cos(theta2);

            // Рисуем рёбра эллипсоида
            dw(x1, y1, z1, x2, y2, z2);
            dw(x2, y2, z2, x3, y3, z3);
            dw(x3, y3, z3, x4, y4, z4);
            dw(x4, y4, z4, x1, y1, z1);
        }
    }

	for (i = 0; i < n / 10; i++)
		for (j = 0; j < 2 * n; j++) {
			double phi1 = 2 * M_PI * j / (2 * n);  // Углы для сегментов
			double phi2 = 2 * M_PI * (j + 1) / (2 * n);

			double x1 = a * i / n * cos(phi1);
			double y1 = b * i / n * sin(phi1);

			double x2 = a * cos(phi2);
			double y2 = b * sin(phi2);

			dw(x1, y1, 0, x2, y2, 0);
		}

    glFinish();
}

void Reshape(GLint w, GLint h) {
    glViewport(0, 0, w, h);
}

void ftimer(int value) {
    theta += 10;
    phi += 10; 
    coeff(-45, theta, phi);

    glutPostRedisplay();
    glutTimerFunc(100, ftimer, 1);
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Ellipsoid");

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);

    glutTimerFunc(1000, ftimer, 1);

    glutMainLoop();

    return 0;
}