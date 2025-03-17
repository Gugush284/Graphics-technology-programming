#include <GL/glut.h>

#include "ellipsoid.h"

constexpr double screen_dist = 1.0; // Расстояние до экрана

struct Point {
    double x, y, z;
};

double v11, v12, v13, v21, v22, v23, v32, v33, v43; // элементы матрицы вида
double rho = 15.0, phi = 80.0, theta = 50.0;
std::vector<Point> eps;
std::vector<Point> edge;

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

void LoadData(const char* filename, std::vector<Point> * points) {
    std::ifstream file(filename);
    if (!file) {
        exit(1);
    }

    Point p;
    while (file >> p.x >> p.y >> p.z) {
        (* points).push_back(p);
    }

    file.close();
}

void Display(void) {
    glClearColor(255, 255, 255, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(255, 0, 0);

	int i;

    // Генерация точек и рёбер эллипсоида
    for (i = 0; i < eps.size(); i += 4) {
            // Рисуем рёбра эллипсоида
            dw(eps[i].x, eps[i].y, eps[i].z, eps[i+1].x, eps[i+1].y, eps[i+1].z);
            dw(eps[i+1].x, eps[i+1].y, eps[i+1].z, eps[i+2].x, eps[i+2].y, eps[i+2].z);
            dw(eps[i+2].x, eps[i+2].y, eps[i+2].z, eps[i+3].x, eps[i+3].y, eps[i+3].z);
            dw(eps[i+3].x, eps[i+3].y, eps[i+3].z, eps[i].x, eps[i].y, eps[i].z);
    }

	for (i = 1; i < edge.size(); i ++)
        dw(edge[i].x, edge[i].y, edge[i].z, edge[0].x, edge[0].y, edge[0].z);

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
    LoadData("ellipsoid_data.txt", &eps);
    LoadData("edge_data.txt", &edge);

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