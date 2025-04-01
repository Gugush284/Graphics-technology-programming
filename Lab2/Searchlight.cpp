#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "glut.h"
#include "glaux.h"
#include <math.h>
#pragma comment(lib,"glaux")
#pragma comment(lib,"legacy_stdio_definitions")

GLfloat xRot = 87.0f, yRot = 90.0f;

GLfloat lightPosWorld[4] = { 0.0f, 10.0f, 0.0f, 1.0f };

GLfloat ambientLight[] = { 0.15f, 0.15f, 0.15f, 1.0f };
GLfloat lightDiff[] = { 1.5f, 1.5f, 1.5f, 1.0f };
GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };

int mode = 0;

void ComputeSpotDirection(GLfloat* newDir) {
    float xr = xRot * 3.14159265f / 180.0f;
    float yr = yRot * 3.14159265f / 180.0f;
    newDir[0] = -sin(xr) * sin(yr);
    newDir[1] = -cos(xr);
    newDir[2] = -sin(xr) * cos(yr);
}

void SetupRC() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT3);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDiff);
    glLightfv(GL_LIGHT3, GL_SPECULAR, specular);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 45.0f);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 20.0f);

    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_NORMALIZE);
}

void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLfloat newSpotDir[3];

    if (mode == 0) {
        gluLookAt(300.0, 0.0, 300.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        lightPosWorld[0] = 200.0f;
        lightPosWorld[1] = 10.0f;
        lightPosWorld[2] = 0.0f;
    }
    else {
        gluLookAt(100.0, 100.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        lightPosWorld[0] = -50.0f;
        lightPosWorld[1] = 0.0f;
        lightPosWorld[2] = 50.0f;
    }

    ComputeSpotDirection(newSpotDir);
    glLightfv(GL_LIGHT3, GL_POSITION, lightPosWorld);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, newSpotDir);

    if (mode == 0) {
        glColor3f(0.1f, 0.8f, 0.0f);
        glutSolidSphere(30.0f, 50, 50);
    }
    else {
        const int gridSize = 100;
        const float cellSize = 1.0f;
        for (int i = -gridSize / 2; i < gridSize / 2; ++i)
            for (int j = -gridSize / 2; j < gridSize / 2; ++j) {
                GLfloat centerX = i * cellSize;
                GLfloat centerZ = j * cellSize;
                glPushMatrix();
                glTranslatef(0.0f, -10.0f, 0.0f);
                glColor3f(0.7f, 0.7f, 0.7f);
                glBegin(GL_QUADS);
                //glNormal3f(0.0f, 1.0f, 0.0f);
                glVertex3f(centerX - cellSize / 2, 0.0f, centerZ - cellSize / 2);
                glVertex3f(centerX + cellSize / 2, 0.0f, centerZ - cellSize / 2);
                glVertex3f(centerX + cellSize / 2, 0.0f, centerZ + cellSize / 2);
                glVertex3f(centerX - cellSize / 2, 0.0f, centerZ + cellSize / 2);
                glEnd();
                glPopMatrix();
            }
    }

    glPushMatrix();
    glTranslatef(lightPosWorld[0], lightPosWorld[1], lightPosWorld[2]);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glColor3ub(255, 255, 0);
    glutSolidSphere(3.0f, 15, 15);
    glPopAttrib();
    glPopMatrix();
    glPopAttrib();

    glutSwapBuffers();
}


void SpecialKeys(int key, int x, int y) {
    const float step = 2.5f;
    if (key == GLUT_KEY_UP)
        xRot -= step;
    if (key == GLUT_KEY_DOWN)
        xRot += step;
    if (key == GLUT_KEY_LEFT)
        yRot -= step;
    if (key == GLUT_KEY_RIGHT)
        yRot += step;

    std::cout << xRot << ":" << yRot << std::endl;

    glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
    if (key == '1') {
        mode = (mode == 0) ? 1 : 0;
        if (mode == 0) {
            xRot = 87.0f;
            yRot = 90.0f;
        }
        else {
            xRot = -80.0f;
            yRot = 137.5f;
        }
        glutPostRedisplay();
    }
}

void resize(int w, int h) {
    if (h == 0) h = 1;
    GLfloat aspect = (GLfloat)w / (GLfloat)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 1.0f, 2000.0f);
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Прожектор и сцена");

    glutReshapeFunc(resize);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(RenderScene);

    SetupRC();
    glutMainLoop();
    return 0;
}