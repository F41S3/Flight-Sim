#include <stdlib.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "lib/model.h"
#include "lib/linked_list.c"

Model *prop1;
Model *prop2;
Model *cessna;
int windowWidth;
int windowHeight;
// Variables to track mouse movement
int lastMouseX, lastMouseY;
bool mouseDown = false;

// Camera parameters
float cameraAngleX = 45.0f;
float cameraAngleY = 45.0f;
float cameraDistance = 5.0f;
float cameraPanSpeed = 0.1f;
float cameraZoomSpeed = 0.1f;

float theta = 0.0f;
float propSpin = 10.0f;

void myIdle()
{
    glutPostRedisplay();
}

void passive_mouse(int x, int y)
{
    if (mouseDown) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        cameraAngleX += deltaX * cameraPanSpeed;
        cameraAngleY += deltaY * cameraPanSpeed;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }

    //printf("lean amount = %f\n", theta);
}

void active_mouse(int op, int state, int x, int y)
{
    if (op == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDown = true;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            mouseDown = false;
        }
    }
}

void initializeGL(void)
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);

    // load the identity matrix into the projection matrix
    glLoadIdentity();

    // set window mode to 2D orthographic
    gluPerspective(90.0, windowWidth / windowHeight, 0.1, 1000.0);

    // change into model-view mode so that we can change the object positions
    glMatrixMode(GL_MODELVIEW);
}


void drawProp1() {
    // center right hub -1.039327, -0.057309, -1.385320
    // center left hub  -1.045702, -0.057650, 1.381654
    //glTranslatef(-1.039327f*0.0f, -0.057309f*0.5f, -1.385320f*0.5f);

    glTranslatef(0.0f, 0.0f,  0.35f);
    glRotatef(propSpin,  1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f,  -0.35f);
    for (int i = 0; i < prop1->model->size; i++) {
        LinkedList* tmpFace = (LinkedList*) llist_get(prop1->model, i);
        for (int j = 0; j < tmpFace->size; j++) {
            LinkedList* tmpInts = (LinkedList*) llist_get(tmpFace, j);

            glBegin(GL_POLYGON);
            for (int k = 0; k < tmpInts->size; k++) {
                int ver = (*(int*) llist_get(tmpInts, k)) - 1;
                glColor3f(0.8f, 1.0f, 0.8f);
                glNormal3d(prop1->norms[ver][0], prop1->norms[ver][1], prop1->norms[ver][2]);
                glVertex3d(prop1->verts[ver][0], prop1->verts[ver][1], prop1->verts[ver][2]);
            }
            glEnd();
        }
    }
    glPopMatrix();
}

void drawProp2() {
    glTranslatef(0.0f, -0.16f, 0.35f);
    glRotatef(propSpin,  1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.16f,  - 0.35f);
    for (int i = 0; i < prop2->model->size; i++) {
        LinkedList* tmpFace = (LinkedList*) llist_get(prop2->model, i);
        for (int j = 0; j < tmpFace->size; j++) {
            LinkedList* tmpInts = (LinkedList*) llist_get(tmpFace, j);

            glBegin(GL_POLYGON);
            for (int k = 0; k < tmpInts->size; k++) {
                int ver = (*(int*) llist_get(tmpInts, k)) - 1;
                glColor3f(0.5f, 0.5f, 0.5f);
                glNormal3d(prop2->norms[ver][0], prop2->norms[ver][1], prop2->norms[ver][2]);
                glVertex3d(prop2->verts[ver][0], prop2->verts[ver][1], prop2->verts[ver][2]);
            }
            glEnd();
        }
    }
    glPopMatrix();
}

void drawCessna() {
    for (int i = 0; i < cessna->model->size; i++) {
        LinkedList* tmpFace = (LinkedList*) llist_get(cessna->model, i);
        float avgx = 0.0f;
        float avgy = 0.0f;
        float avgz = 0.0f;

        for (int j = 0; j < tmpFace->size; j++) {
            LinkedList* tmpInts = (LinkedList*) llist_get(tmpFace, j);

            glBegin(GL_POLYGON);
            for (int k = 0; k < tmpInts->size; k++) {
                int ver = (*(int*) llist_get(tmpInts, k)) - 1;
                if (i == 33) {
                    avgx += cessna->verts[ver][0];
                    avgy += cessna->verts[ver][1];
                    avgz += cessna->verts[ver][2];

                }
                glColor3f(1.0f, 1.0f, 1.0f);
                glNormal3d(cessna->norms[ver][0], cessna->norms[ver][1], cessna->norms[ver][2]);
                glVertex3d(cessna->verts[ver][0], cessna->verts[ver][1], cessna->verts[ver][2]);
            }
            glEnd();
        }
        if (i == 33) {
            printf("%f, %f, %f\n", avgx / (float) tmpFace->size, avgy / (float) tmpFace->size, avgz / (float) tmpFace->size);
        }
    }
    glPopMatrix();
}

void myDisplay(void)
{
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
            cameraDistance * cos(cameraAngleY) * sin(cameraAngleX),
            cameraDistance * sin(cameraAngleY),
            cameraDistance * cos(cameraAngleY) * cos(cameraAngleX),
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0
    );

    glPushMatrix();
    gluSphere(gluNewQuadric(), 0.1f, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    drawProp1();
    glPopMatrix();

    glPushMatrix();
    drawProp2();
    glPopMatrix();

    glPushMatrix();
    drawCessna();
    glPopMatrix();
    // swap the drawing buffers
    glutSwapBuffers();
}


void resize(int x, int y) {
    windowHeight = y;
    windowWidth = x;
    glViewport (0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(30.0, (double)windowWidth / (double)windowHeight, 0.1, 5000.0);
    glMatrixMode(GL_MODELVIEW);
}

double updateAngle(double curAngle, double propSpeed) {
    curAngle += propSpeed;
    if (curAngle > 360.0) {
        curAngle -= 360.0;
    }
    return curAngle;
}

void updateValues() {
    theta = updateAngle(theta, 0.0);
    propSpin = updateAngle(propSpin, 5.0);
    /***
     * URL: https://www.opengl.org/resources/libraries/glut/spec3/node64.html
     * AUTHOR: Mark Kilgard
     * DATE ACCESSED: Oct 19th, 2023
     */
    glutTimerFunc(1000 / 60, updateValues, 0); // 60 FPS
}




int main(int argc, char** argv) {
    prop1 = readModel("../propeller.txt");
    prop2 = readModel("../propeller.txt");
    cessna = readModel("../cessna.txt");
    centerVerts(prop1);
    centerVerts(cessna);



    glutInit(&argc, argv);
    // set display mode
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    // set window size
    windowWidth = 500;
    windowHeight = 500;
    glutInitWindowSize(windowWidth,windowHeight);
    // set window position on screen
    glutInitWindowPosition(100, 150);
    // open the screen window
    glutCreateWindow("colorcube");

    // register redraw function
    glutDisplayFunc(myDisplay);
    // register the idle function
    glutIdleFunc(myIdle);
    // register the mouse button function
    glutMotionFunc(passive_mouse);
    glutMouseFunc(active_mouse);
    glutReshapeFunc(resize);
    glutTimerFunc(0,  updateValues, 0);

    //initialize the rendering context
    initializeGL();
    // go into a perpetual loop
    glutMainLoop();
    return 0;
}
