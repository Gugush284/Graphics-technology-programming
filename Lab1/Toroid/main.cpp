#include <GL/glut.h>
#include <cmath>
#include <fstream>
#include <vector>

constexpr double screen_dist = 1.0; // расстояние до экрана

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
    std::ifstream fp("torus.dat");
    if (!fp) {
        exit(1);
    }

    glClearColor(255, 255, 255, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(0, 0, 0);

    int n, k, l1, l2, l3, l4;
    
    fp >> n;

    std::vector<double> X(n * n), Y(n * n), Z(n * n);

    for (int i = 0; i < n * n; i++) {
        fp >> k >> X[i] >> Y[i] >> Z[i];
    }

    for (int j = 0; j < n * n; j++) {
        fp >> l1 >> l2 >> l3 >> l4;
        dw(X[l1], Y[l1], Z[l1], X[l2], Y[l2], Z[l2]);
        dw(X[l2], Y[l2], Z[l2], X[l3], Y[l3], Z[l3]);
        dw(X[l3], Y[l3], Z[l3], X[l4], Y[l4], Z[l4]);
        dw(X[l4], Y[l4], Z[l4], X[l1], Y[l1], Z[l1]);
    }

    fp.close(); // Закрываем поток (не обязательно, так как ifstream автоматически закроет файл при выходе из области видимости)

    glFinish();
}

void Reshape(GLint w, GLint h){
    /* Устанавливаем размеры области отображения */
    glViewport(0,0,w,h);
}

void ftimer (int value){
    theta += 10;
    phi += 10; 
    coeff(-45, theta, phi);

    glutPostRedisplay();
    glutTimerFunc(100, ftimer, 1);
}

int main(int argc, char *argv [ ]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Torus");

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	glutTimerFunc(1000, ftimer , 1);

    /* Главный цикл приложения */
	glutMainLoop();

    return 0;
}
