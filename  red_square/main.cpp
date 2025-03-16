#include <GL/glut.h>
#include <cmath>
#include <vector>

constexpr int fps = 60;
constexpr double speed = 0.01;

double rotationAngle = 0.0;

void Display(void) {
	std::vector<std::vector<double>> vertices{
		{-0.5, -0.5},
		{-0.5, 0.5},
		{0.5, 0.5},
		{0.5, -0.5}
	};

	// Очистка экрана
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Задаем цвет квадрата
	glColor3ub(255,0,0);

	glBegin(GL_QUADS); // Начинаем отрисовку квадрата

	double cosRotation = cos(rotationAngle);
	double sinRotation = sin(rotationAngle);

	// Для каждой вершины задаем коордмнаты с учетом угла вращения
	for (const auto& vert : vertices)
		glVertex2f(
			vert[0]*cosRotation - vert[1]*sinRotation,
			vert[0]*sinRotation + vert[1]*cosRotation
		);

	glEnd();

	glFinish();
}


void Reshape(GLint w, GLint h){
    glViewport(0,0,w,h);
}

void Timer(int value){
	rotationAngle += speed;
	if (rotationAngle > 2 * M_PI)
		rotationAngle -= 2 * M_PI;

	glutPostRedisplay();
	glutTimerFunc(1000 / fps, Timer, 0);
}
    
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("The red square");

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
	glutTimerFunc(0, Timer, 0);

    glutMainLoop();
}
    