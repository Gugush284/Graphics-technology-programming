// tri1.cpp : Defines the entry point for the console application.
//


#include <stdlib.h>
#include <iostream>
#include "glut.h"
#include "glaux.h"
#include <math.h>
#pragma comment(lib,"glaux")
#pragma comment(lib,"legacy_stdio_definitions")

unsigned int texture;
AUX_RGBImageRec* image;

int i, j, sw = 0, gamma = 0;
double center[4][2] = {{0,0}, {1,1}, {1,0}, {0,1}};
int prevX = 0, prevY = 0;

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 40000);
    glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
    double g = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    glTranslated(0, 0, -10.5);

    glTranslated(center[sw][0], center[sw][1], 0);
    glRotated(gamma, 0, 0, -1);
    glTranslated(-center[sw][0], -center[sw][1], 0);

    GLfloat BlueCol[3] = { 0, 0, 1 };

    glEnable(GL_TEXTURE_2D);  // включает текстуру
    glColor3ub(190, 194, 12);
    glNormal3f(0, 0, 1);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_TRIANGLES);
    // Вершины с текстурами
    glTexCoord2d(0, 1); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2d(1, 0); glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2d(1, 1); glVertex3f(0.0, 1.0, 0.0);

    glTexCoord2d(0, 1); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2d(0, 0); glVertex3f(1.0, 0.0, 0.0);
    glTexCoord2d(1, 0); glVertex3f(1.0, 1.0, 0.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

// Обработчик движения мыши с зажатой кнопкой
void MouseMotion(int x, int y)
{
    double new_gamma = atan2(y - center[sw][1], x - center[sw][0]) * 180 / 3.14;
    if (new_gamma < 0) new_gamma += 360;
    if (new_gamma > 360) new_gamma -= 360;

    double delta_gamma = new_gamma - gamma;
    if (delta_gamma < 0) delta_gamma += 360;
    if (delta_gamma > 360) delta_gamma -= 360;

    gamma += delta_gamma;
    if (gamma < 0) gamma += 360;
    if (gamma > 360) gamma -= 360;

    glutPostRedisplay();
}

// Обработчик движения мыши без зажатой кнопки
void MousePassiveMotion(int x, int y)
{
    return;
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1':  // Вращение вокруг вершины (1, 1, 0)
        sw = (sw != 1) ? 1 : 0;
        gamma = 0;
        break;
    case '2':  // Вращение вокруг вершины (1, 0, 0)
        sw = (sw != 2) ? 2 : 0;
        gamma = 0;
        break;
    case '3':  // Вращение вокруг вершины (0, 1, 0)
        sw = (sw != 3) ? 3 : 0;
        gamma = 0;
        break;
    case VK_ESCAPE:  // Выход из программы
        exit(0);
    default:
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(50, 10);
    glutCreateWindow("Test");

    glClearColor(0, 0, 1, 1.0);
    glShadeModel(GL_FLAT);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Настройка освещения
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float pos[4] = { 0, 0, 100, 1 };
    float dir[3] = { -1, -1, -1 };
    GLfloat mat_specular[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);

    // Загрузка текстуры
    image = auxDIBImageLoad("t1.bmp");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelZoom(1, 1);
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image->sizeX, image->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image->data);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(Keyboard);

    // Подключение функции для отслеживания движения мыши
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(MousePassiveMotion);

    glutMainLoop();
    return 0;
}

