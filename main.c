/***
 * AUTHOR: Alex Pettipas
 * ID: B00813771
 * Plane sim project for CSCI 3161
 */


#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "lib/linked_list.c"
#include "lib/model.h"
#include "lib/mountain.h"
#include "lib/textures.h"
#include <time.h>

int window_width = 800;
int window_height = 600;
bool wireFrameMode = false;
unsigned int mode = GL_POLYGON;
unsigned int mountain_mode = GL_QUADS;
float theta = 0.0f; // current aircraft roll
float phi = 0.0f; // current aircraft pitch

/* Imported models */
Model* cessna;          // cessna model stuct, stores vertices, faces, normals, and position
double ces_speed = 1.0; // current speed
float propRotSpeed = 5.0f;
double propSpin = 0.0;  // current degrees rotated for propellers
double ces_dir = 0.0;   // current direction the cessna is pointing

Model* prop1;   // prop1 model stuct, stores vertices, faces, normals, and position
Model* prop2;   // prop2 model stuct, stores vertices, faces, normals, and position

// boring color array
float cessnaColors[][4] = {
        {0.0f, 0.0f, 0.0f, 1.0f}, // fuselage (corresponds to nothing)
        {1.0f, 1.0f, 0.0f, 1.0f}, // access 5 (actual fuselage)
        {1.0f, 1.0f, 0.0f, 1.0f}, // fuselage 2 (underneath nose)
        {1.0f, 1.0f, 0.0f, 1.0f}, // access 4 (lower half of fuselage)
        {1.0f, 1.0f, 0.0f, 1.0f}, // nose (exhaust panels)
        {1.0f, 1.0f, 0.0f, 1.0f}, // glass (nose tip)
        {0.0f, 0.0f, 0.0f, 1.0f}, // windows (actually the windows)
        {0.5f, 0.0f, 0.5f, 1.0f}, // trim (wings)
        {1.0f, 1.0f, 0.0f, 1.0f}, // door 2a (window outline)
        {1.0f, 1.0f, 0.0f, 1.0f}, // door 2 (bottom piece of door)
        {1.0f, 1.0f, 0.0f, 1.0f}, // door (middle piece of door)
        {1.0f, 1.0f, 0.0f, 1.0f}, // glass glass (top of door)
        {0.0f, 0.0f, 0.0f, 1.0f}, // cargo 2 (door window)
        {1.0f, 1.0f, 0.0f, 1.0f}, // cargo (rear bottom cargo door)
        {1.0f, 1.0f, 0.0f, 1.0f}, // flaps 1 (top rear cargo door)
        {0.0f, 0.0f, 1.0f, 1.0f}, // deicing (elevator / aileron for lwing)
        {0.5f, 0.0f, 0.5f, 1.0f}, // elevator (deicing patches)
        {0.0f, 0.0f, 1.0f, 1.0f}, // rflaps (elevator)
        {0.0f, 0.0f, 1.0f, 1.0f}, // flaps 2 (elevator control surface)
        {0.0f, 0.0f, 1.0f, 1.0f}, // tail (aileron / flaps for rwing)
        {0.5f, 0.0f, 0.5f, 1.0f}, // rudder (actually rudder)
        {0.0f, 0.0f, 1.0f, 1.0f}, // antenna (rudder control surface)
        {0.0f, 0.0f, 0.0f, 1.0f}, // hub 2 (antenna)
        {1.0f, 1.0f, 0.0f, 1.0f}, // shroud (rhub)
        {1.0f, 1.0f, 0.0f, 1.0f}, // engine (intake trim)
        {0.0f, 0.0f, 1.0f, 1.0f}, // access 1 (engine cowlings)
        {0.0f, 0.0f, 1.0f, 1.0f}, // access 2 (engine access)
        {0.0f, 0.0f, 1.0f, 1.0f}, // a handles (vents)
        {0.0f, 0.0f, 1.0f, 1.0f}, // g vents (engine exhaust)
        {1.0f, 1.0f, 0.0f, 1.0f}, // fins (intake)
        {0.0f, 0.0f, 0.0f, 1.0f}, // intake (intake)
        {1.0f, 1.0f, 0.0f, 1.0f}, // vents 2 (intake)
        {1.0f, 1.0f, 0.0f, 1.0f}, // hub 1 (exhaust)
        {1.0f, 1.0f, 0.0f, 1.0f} // lhub
};

// boring color array
float propColors[][4] = {
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {1.0f, 0.0f, 0.0f, 1.0f}
};



GLUquadricObj* sea; // glu quadric representing the sea disk
GLubyte * seaTex;   // glu byte to store texture data
int *seaTexW;       // integer holding sea texture width
int *seaTexH;       // integer holding sea texture height
GLuint *seaTexID;   // texture id for sea texture

GLUquadricObj* sky; // glu quadric representing the sky disk
GLubyte * skyTex;   // glu byte to store texture data
int *skyTexW;       // integer holding sky texture width
int *skyTexH;       // integer holding sky texture height
GLuint *skyTexID;   // texture id for sky texture

/// all mountain definitions stored in lib folder
Mountain *mountain1;    // first mountain object
Mountain *mountain2;    // second mountain object
Mountain *mountain3;    // third mountain object

GLubyte *mountTex;  // glu byte to store texture data
int *mountTexW;     // integer holding sky texture width
int *mountTexH;     // integer holding sky texture height
GLuint *mountTexID; // texture id for mountain texture

// default values for toggling various features
int showGrid = 0;
int fullscreen = 0;
int lighting = 1;
int fog = 0;
int showMountains = 1;
int textureMountains = 1;

// fog color, boring, lightish pink with an alpha component
GLfloat fogColor[4] = {1,0.7,0.7,0.1};

// light properties
GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat position[] = {0.0, 500.0, 0.0, 0.0};
GLfloat direction[] = {0.0, 1000.0, 0.0};

/********************************************************************************************

	Function:		init

	Description:	Initializes the OpenGL rendering context for display.

*********************************************************************************************/
void init() {

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // defining light properties
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0, (double)window_width / (double)window_height, 0.1, 5000.0);
    glMatrixMode(GL_MODELVIEW);
}

/********************************************************************************************

	Function:		drawSea

	Description:	draws, textures, and positions the glu disk representing the sea

*********************************************************************************************/
void drawSea() {
    glEnable(GL_TEXTURE_2D);
    if (fog == 0) {
        glEnable(GL_FOG); // fog toggle
    }
    glRotatef(90.0, 1.0, 0.0, 0.0); // rotating flat
    glBindTexture(GL_TEXTURE_2D, *seaTexID); // binding sea texture
    glColor3f(1.0f, 1.0f, 1.0f); // setting color to be white for texturing
    float a[] = {0.1f, 0.1f, 0.1f};
    float d[] = {1.0f, 1.0f, 1.0f};
    float s[] = {0.1f, 0.1f, 0.1f};
    float e[] = {1.0f, 1.0f, 1.0f}; // high emissive value for even coloring

    // setting material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT, GL_EMISSION, e);

    // wireframe mode toggling
    if (wireFrameMode) {
        gluQuadricDrawStyle(sea, GLU_LINE);
    } else {
        gluQuadricDrawStyle(sea, GLU_FILL);
    }

    gluDisk(sea, 0.0, 1000.0, 20, 4); // drawing the disk at the end after all toggles
    if (fog == 0) {
        glDisable(GL_FOG); // preventing fog from being drawn on other surfaces
    }
    glDisable(GL_TEXTURE_2D);
}

void drawSky() {
    glEnable(GL_TEXTURE_2D);

    glTranslatef(0.0, -10.0, 0.0); // proper position
    glRotatef(-90.0, 1.0, 0.0, 0.0); // proper rotation

    glBindTexture(GL_TEXTURE_2D, *skyTexID); // binding texture to glu cylinder
    glColor3f(1.0f, 1.0f, 1.0f);
    float a[] = {0.1f, 0.1f, 0.1f};
    float d[] = {1.0f, 1.0f, 1.0f};
    float s[] = {0.1f, 0.1f, 0.1f};
    float e[] = {1.0f, 1.0f, 1.0f}; // high emissive value for even coloring

    // setting material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT, GL_EMISSION, e);

    // wireframe toggle
    if (wireFrameMode) {
        gluQuadricDrawStyle(sky, GLU_LINE);
    } else {
        gluQuadricDrawStyle(sky, GLU_FILL);
    }

    gluCylinder(sky, 1000.0, 1000.0, 1000.0, 20, 5); // draw the cylinder
    glDisable(GL_TEXTURE_2D);

}

/********************************************************************************************

	Function:		drawCessna

	Description:	Draws and colors the cessna from the loaded file

*********************************************************************************************/
void drawCessna() {
    glDisable(GL_TEXTURE_2D); // disables texturing so we do not accidentally texture the cessna
    glTranslatef(cessna->x, cessna->y, cessna->z); // positions the aircraft based on its currently stored position
    glRotatef(ces_dir, 0.0, 1.0, 0.0); // sets the aircrafts direction
    glRotatef(phi, 0.0, 0.0, 1.0); // sets the aircrafts pitch
    glRotatef(theta, 1.0, 0.0, 0.0); // sets the aircraft roll

    /***
     * Used nested linked lists here as the length of the array sizes are variable.
     * 1) The loop first gets the face linked list for the nth component.
     * 2) Then the loop iterates through that component list, selecting the nth face
     * 3) Then the loop iterates though that face list, using the stored index values to draw the vertices.
     */
    for (int i = 0; i < cessna->model->size; i++) {
        LinkedList* tmpFace = (LinkedList*) llist_get(cessna->model, i); // select component
        GLfloat a[] = {0.02f, 0.085f, 0.08f, 1.0f};
        GLfloat s[] = {0.10f, 0.10f, 0.10f, 0.3f};
        float e[] = {0.0f, 0.0f, 0.0f}; // low emissive property for colors to look not washed out
        glMaterialfv(GL_FRONT, GL_AMBIENT, a);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, cessnaColors[i]); // stored color array
        glMaterialfv(GL_FRONT, GL_SPECULAR, s);
        glMaterialfv(GL_FRONT, GL_EMISSION, e);
        for (int j = 0; j < tmpFace->size; j++) {
            LinkedList* tmpInts = (LinkedList*) llist_get(tmpFace, j); // select face of component
            glBegin(mode);
            for (int k = 0; k < tmpInts->size; k++) {
                int ver = (*(int*) llist_get(tmpInts, k)) - 1; // select vertex index from face of component
                glColor3fv(cessnaColors[i]); // here for when lighting is disabled
                glNormal3d(cessna->norms[ver][0], cessna->norms[ver][1], cessna->norms[ver][2]);
                glVertex3d(cessna->verts[ver][0], cessna->verts[ver][1], cessna->verts[ver][2]);
            }
            glEnd();
        }
    }
    glEnable(GL_TEXTURE_2D);
}

/********************************************************************************************

	Function:		drawProps

	Description:	Draws and colors individual propellers for the cessna

*********************************************************************************************/
void drawProps() {
    glDisable(GL_TEXTURE_2D); // disables texturing so we do not accidentally texture the propellers

    glPushMatrix();
    // translations to stick to cessna
    glTranslatef(cessna->x, cessna->y, cessna->z);
    glRotatef(ces_dir, 0.0, 1.0, 0.0);
    glRotatef(theta, 1.0, 0.0, 0.0);
    glRotatef(phi, 0.0, 0.0, 1.0);

    // only difference here is that it translates the prop to the origin, rotates it, then translates it back
    glTranslatef(0.0f, 0.0f, 0.35f);
    glRotatef(propSpin,  1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -0.35f);

    /***
     * Used nested linked lists here as the length of the array sizes are variable.
     * 1) The loop first gets the face linked list for the nth component.
     * 2) Then the loop iterates through that component list, selecting the nth face
     * 3) Then the loop iterates though that face list, using the stored index values to draw the vertices.
     */
    for (int i = 0; i < prop1->model->size; i++) {
        LinkedList* tmpFace = (LinkedList*) llist_get(prop1->model, i); // select component
        for (int j = 0; j < tmpFace->size; j++) {
            LinkedList* tmpInts = (LinkedList*) llist_get(tmpFace, j); // select face of component

            glBegin(mode);
            for (int k = 0; k < tmpInts->size; k++) {
                int ver = (*(int*) llist_get(tmpInts, k)) - 1; // select vertex index from face of component
                glColor3fv(propColors[i]);
                glNormal3d(prop1->norms[ver][0], prop1->norms[ver][1], prop1->norms[ver][2]);
                glVertex3d(prop1->verts[ver][0], prop1->verts[ver][1], prop1->verts[ver][2]);
            }
            glEnd();
        }
    }
    glPopMatrix();

    /***
     * Essentially just repeated code except for on the other side of the cessna
     */
    glPushMatrix();
    glTranslatef(cessna->x, cessna->y, cessna->z);
    glRotatef(ces_dir, 0.0, 1.0, 0.0);
    glRotatef(theta, 1.0, 0.0, 0.0);
    glRotatef(phi, 0.0, 0.0, 1.0);

    glTranslatef(0.0f, 0.0f, -0.35f);
    glRotatef(propSpin,  1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.35f);


    for (int i = 0; i < prop2->model->size; i++) {
        LinkedList* tmpFace = (LinkedList*) llist_get(prop2->model, i);
        for (int j = 0; j < tmpFace->size; j++) {
            LinkedList* tmpInts = (LinkedList*) llist_get(tmpFace, j);

            glBegin(mode);
            for (int k = 0; k < tmpInts->size; k++) {
                int ver = (*(int*) llist_get(tmpInts, k)) - 1;
                glColor3fv(propColors[i]);
                glNormal3d(prop2->norms[ver][0], prop2->norms[ver][1], prop2->norms[ver][2]);
                glVertex3d(prop2->verts[ver][0], prop2->verts[ver][1], prop2->verts[ver][2] - 0.7);
            }
            glEnd();
        }
    }
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
}

/********************************************************************************************

	Function:		camFollow

	Description:	Positions the camera so that it is always behind the cessna
                    Moves further back when speed is increased, moves closer
                    when speed decreased

*********************************************************************************************/
void camFollow() {
    // Set camera position and orientation
    gluLookAt(cessna->x + 2.0 * cosf(-ces_dir * M_PI / 180.0), cessna->y + 0.5,  cessna->z + 2.0 * sinf(-ces_dir * M_PI / 180.0)
            ,cessna->x -1.0 * cosf(-ces_dir * M_PI / 180.0), cessna->y , cessna->z -1.0 * sinf(-ces_dir * M_PI / 180.0),
              0.0, 1.0, 0.0);
}


/********************************************************************************************

	Function:		drawOrigin

	Description:	Draws glusphere object as well as x, y, and z lines at the origin
                    of the scene

*********************************************************************************************/
void drawOrigin() {

    glLineWidth(5.0f); // Thickening lines

    // draws x axis line
    glPushMatrix();
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(100.0, 0.0, 0.0);
    glEnd();
    glPopMatrix();

    // draws y axis line
    glPushMatrix();
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 100.0, 0.0);
    glEnd();
    glPopMatrix();

    // draws z axis line
    glPushMatrix();
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 100.0);
    glEnd();
    glPopMatrix();

    // draws sphere
    glColor3f(1.0, 1.0, 1.0);
    glutSolidSphere(1, 10, 10);
    glLineWidth(1.0f);
}

/********************************************************************************************

	Function:		drawMountain

	Description:	Draws and textures each mountain in the scene, mountain file can be
                    expanded to store texture ids for each mountain, however this just
                    uses the single texture file provided for all mountains.

*********************************************************************************************/
void drawMountain(Mountain* m)
{
    // texture enabled or disabled toggle
    if (textureMountains) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, *mountTexID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    // translates mountain location
    glTranslatef(m->x, -25.0f * m->heightScale, m->y);

    glBegin(mountain_mode); // selects current draw mode for the mountain

    glColor3f(1.0f, 1.0f, 1.0f);
    float a[] = {0.1f, 0.1f, 0.1f};
    float d[] = {1.0f, 1.0f, 1.0f};
    float s[] = {0.1f, 0.1f, 0.1f};
    float e[] = {0.0f, 0.0f, 0.0f}; // low emissive property to show that lighting works
    for (int i = 0; i < m->mountain_width-1; i++) {
        glMaterialfv(GL_FRONT, GL_AMBIENT, a);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
        glMaterialfv(GL_FRONT, GL_SPECULAR, s);
        glMaterialfv(GL_FRONT, GL_EMISSION, e);


        for (int j = 0; j < m->mountain_height-1; j++) {
            if (textureMountains) { // texture mountain check 2 for some attempt at efficiency

                // texture coordinates to ensure even coverage, simply applies the texture as if a sheet were dropped on
                // the mountain.
                glTexCoord2f(m->verts[i][j][0] / ((float) m->mountain_width-1), m->verts[i][j][2]  / ((float) m->mountain_height-1));
                glNormal3fv(m->normals[i][j]);
                glVertex3fv(m->verts[i][j]);

                glTexCoord2f(m->verts[i+1][j][0] / ((float) m->mountain_width-1), m->verts[i][j][2]  / ((float) m->mountain_height-1));
                glNormal3fv(m->normals[i+1][j]);
                glVertex3fv(m->verts[i+1][j]);

                glTexCoord2f(m->verts[i+1][j+1][0] / ((float) m->mountain_width-1), m->verts[i][j+1][2]  / ((float) m->mountain_height-1));
                glNormal3fv(m->normals[i+1][j+1]);
                glVertex3fv(m->verts[i+1][j+1]);

                glTexCoord2f(m->verts[i][j+1][0] / ((float) m->mountain_width-1), m->verts[i][j+1][2]  / ((float) m->mountain_height-1));
                glNormal3fv(m->normals[i][j+1]);
                glVertex3fv(m->verts[i][j+1]);
            }
            else {
                if (m->verts[i][j][1] > 40.0f) {
                    glColor3f(1.0f, 0.95f, 0.95f);
                } else if (m->verts[i][j][1] > 30.0f){
                    glColor3f(0.0f, 0.5f, 0.0f);
                } else {
                    glColor3f(0.5f, 0.5f, 0.5f);

                }
                glNormal3fv(m->normals[i][j]);
                glVertex3fv(m->verts[i][j]);
                glVertex3fv(m->verts[i+1][j]);
                glVertex3fv(m->verts[i+1][j+1]);
                glVertex3fv(m->verts[i][j+1]);
            }

        }
    }
    glEnd();
    if (textureMountains) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
    }
}

/********************************************************************************************

	Function:		drawHud

	Description:	Was trying to figure out how to draw a hud overlay, was close but no time

*********************************************************************************************/
void drawHud() {
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_POLYGON);
    glVertex2d(0.0f, 0.0f);
    glVertex2d(100.0f,100.0f);
    glVertex2d(0.0f, 100.0f);
    glEnd();

    glEnable(GL_DEPTH_TEST);
}

/********************************************************************************************

	Function:		display

	Description:	Display callback, clears frame buffer and depth buffer, positions the
                    camera and draws all scene objects.

*********************************************************************************************/
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Point the camera at a point ahead of the enterprise
    /* Draw cessna */
    glPushMatrix();
        drawCessna();
    glPopMatrix();

    glPushMatrix();
        drawProps();
    glPopMatrix();



    if (showGrid == 0) {
        glPushMatrix();
        glColor3f(1,1,1);
        drawSea();
        glPopMatrix();

        glPushMatrix();
        glColor3f(1, 1, 1);
        drawSky();
        glPopMatrix();

        if (showMountains) {
            glPushMatrix();
            glColor3f(1.0, 1.0, 1.0);
            drawMountain(mountain1);
            glPopMatrix();

            glPushMatrix();
            glColor3f(1.0, 1.0, 1.0);
            drawMountain(mountain2);
            glPopMatrix();

            glPushMatrix();
            glColor3f(1.0, 1.0, 1.0);
            drawMountain(mountain3);
            glPopMatrix();
        }
    } else {
        glPushMatrix();
        drawOrigin();
        glPopMatrix();
    }


    glLoadIdentity();
    camFollow();
    //drawHud();
    glutSwapBuffers();
}

/********************************************************************************************

	Function:		updateAngle

	Description:	Updates and locks angle to within 360 degrees, only works for increasing
                    angles

                    Note: stolen from A2 submission

*********************************************************************************************/
double updateAngle(double curAngle, double propSpeed) {
    curAngle += propSpeed;
    if (curAngle > 360.0) {
        curAngle -= 360.0;
    }
    return curAngle;
}

/********************************************************************************************

	Function:		updateValues

	Description:	Updates values scene objects

*********************************************************************************************/
void updateValues() {
    theta = updateAngle(theta, 0.0);
    ces_dir = updateAngle(ces_dir, theta * 0.1);
    cessna->x -= ces_speed * cosf(ces_dir * M_PI / 180.0);
    cessna->z += ces_speed * sinf(ces_dir * M_PI / 180.0);
    cessna->y -= ces_speed * sinf(phi * M_PI / 180.0);
    glutPostRedisplay();
    propSpin = updateAngle(propSpin, propRotSpeed);

    /***
     * URL: https://www.opengl.org/resources/libraries/glut/spec3/node64.html
     * AUTHOR: Mark Kilgard
     * DATE ACCESSED: Oct 19th, 2023
     */
    glutTimerFunc(1000 / 60, updateValues, 0); // 60 FPS
}



/********************************************************************************************

	Function:		keyboardDown

	Description:	Handles all keyboard button inputs excluding special keys

    Parameters:     key: unsigned char value representing the ascii value for normal keys

                    x : int value representing the mouse x position, unused

                    y : int value representing the mouse y position, unused

*********************************************************************************************/
void keyboardDown(unsigned char key, int x, int y) {

    switch (key) {
        case 27:
            exit(0);
            break;
        case 'w':
            if (wireFrameMode) {
                wireFrameMode = false;
                mode = GL_POLYGON;
                mountain_mode = GL_QUADS;
            } else {
                wireFrameMode = true;
                mode = GL_LINE_LOOP;
                mountain_mode = GL_LINE_LOOP;
            }
            break;
        case 'q':
            glutLeaveMainLoop();
            break;
        case 'f':
            if (fullscreen == 0) {
                fullscreen = 1;
                glutFullScreen();
            } else {
                fullscreen = 0;
                glutReshapeWindow(window_width, window_height);
                glutPositionWindow(100, 100);
            }
            break;
        case 's':
            if (showGrid == 0) {
                showGrid = 1;
            } else {
                showGrid = 0;
            }
            break;
        case 'b':
            if (fog == 0) {
                fog = 1;
            } else {
                fog = 0;
            }
            break;
        case 'd':
            if (lighting == 0) {
                lighting = 1;
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
                glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            } else {
                lighting = 0;
                glDisable(GL_LIGHTING);
                glDisable(GL_LIGHT0);
            }
            break;
        case 'm':
            if (showMountains == 0) {
                showMountains = 1;
            } else {
                showMountains = 0;
            }
            break;
        case 't':
            if (textureMountains == 0) {
                textureMountains = 1;
            } else {
                textureMountains = 0;
            }
            break;
    }
}

/********************************************************************************************

	Function:		keyboardUp

	Description:	Handles all keyboard keys being released

    Parameters:     key: unsigned char value representing the ascii value for normal keys

                    x : int value representing the mouse x position, unused

                    y : int value representing the mouse y position, unused

*********************************************************************************************/
void keyboardUp(unsigned char key, int x, int y) {
    switch (key)
    {
        case 27:
            exit (0);
            break;
        case 'z':
            break;
    }
}

/********************************************************************************************

	Function:		increaseSpeed

	Description:	Updates the speed within hardcoded values

*********************************************************************************************/
void increaseSpeed() {
    if (ces_speed < 1.5f) {
        propRotSpeed += 0.5f;
        ces_speed += 0.1f;
    }
}

/********************************************************************************************

	Function:		decreaseSpeed

	Description:	Updates the speed within hardcoded values

*********************************************************************************************/
void decreaseSpeed() {
    if (ces_speed > 0.5f) {
        propRotSpeed -= 0.5f;
        ces_speed -= 0.1f;
    }
}

/********************************************************************************************

	Function:		specialDown

	Description:	Handles all special keyboard keys that do not have ascii values.

    Parameters:     key: integer value representing the glut defined special keys

                    x : int value representing the mouse x position, unused

                    y : int value representing the mouse y position, unused

*********************************************************************************************/
void specialDown(int key, int x, int y) {
    switch (key)
    {
        case GLUT_KEY_UP:
            break;
        case GLUT_KEY_DOWN:
            break;
        case GLUT_KEY_LEFT:
            break;
        case GLUT_KEY_RIGHT:
            break;
        case GLUT_KEY_PAGE_UP:
            increaseSpeed();
            break;
        case GLUT_KEY_PAGE_DOWN:
            decreaseSpeed();
            break;
    }
}

/********************************************************************************************

	Function:		specialUp

	Description:	Handles the release of special keyboard keys that do not have ascii values.

    Parameters:     key: integer value representing the glut defined special keys

                    x : int value representing the mouse x position, unused

                    y : int value representing the mouse y position, unused

*********************************************************************************************/
void specialUp(int key, int x, int y) {
    switch (key)
    {
        case GLUT_KEY_UP:
            break;
        case GLUT_KEY_DOWN:
            break;
        case GLUT_KEY_LEFT:
            break;
        case GLUT_KEY_RIGHT:
            break;
        case GLUT_KEY_PAGE_UP:
            break;
        case GLUT_KEY_PAGE_DOWN:
            break;
    }
}

/********************************************************************************************

	Function:		passiveMouse

	Description:	Handles capture of x and y position of the mouse

    Parameters:     x : int value representing the mouse x position, used to calculate roll

                    y : int value representing the mouse y position, used to calculate pitch

*********************************************************************************************/
void passiveMouse(int x, int y)
{
    float correctY = (float) fabs(window_height / 1.2);
    float correctX = (float) ((window_width / 2.0) - x);

    theta = atanf(correctX / correctY);
    theta = (float) (theta * 180.0f / M_PI);

    phi = atanf( (float) (y)  / (float) window_height);
    phi = (phi * 180.0f / M_PI) - 22.5f;
}

/********************************************************************************************

	Function:		resize

	Description:	Window resize handling so that the window continues updating the scene

    Parameters:     x : int value representing the window x size, used to calculate width

                    y : int value representing the window y size, used to calculate height

    Author:         S. Brooks

    Date Accessed:  Nov 29th, 2023

*********************************************************************************************/
void resize(int x, int y) {
    window_height = y;
    window_width = x;
    glViewport (0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0, (double)window_width / (double)window_height, 0.1, 5000.0);
    glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char** argv) {
    // read file in
    cessna = readModel("../cessna.txt");
    prop1 = readModel("../propeller.txt");
    centerVerts(prop1);

    centerVerts(cessna);
    prop2 = readModel("../propeller.txt");
    centerVerts(prop2);
    time_t t;
    srand((unsigned) time(&t)); // setting pseudo random seed so that mountains change every time

    // initializing mountain struct
    mountain1 = initMountain(200, 200, rand() % 500 - 250, rand() % 500 - 250, 1.0f);
    genMountain(mountain1, mountain1->mountain_width, mountain1->mountain_height);
    calculateNormals(mountain1->verts, mountain1->normals, mountain1->mountain_width, mountain1->mountain_height);

    mountain2 = initMountain(150, 150, -(rand() % 375 + 100), rand() % 750 - 375, 0.5f);
    genMountain(mountain2, mountain2->mountain_width, mountain2->mountain_height);
    calculateNormals(mountain2->verts, mountain2->normals, mountain2->mountain_width, mountain2->mountain_height);

    mountain3 = initMountain(100, 100, rand() % 750 - 375, -(rand() % 375 + 100), 0.25f);
    genMountain(mountain3, mountain3->mountain_width, mountain3->mountain_height);
    calculateNormals(mountain3->verts, mountain3->normals, mountain3->mountain_width, mountain3->mountain_height);


    // enabling display requirements
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Plane Sim");
    glutDisplayFunc(display);
    glutTimerFunc(0, updateValues, 0); // framerate limiting to 60 (1000 ms / 60 f)
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutPassiveMotionFunc(passiveMouse);
    glutReshapeFunc(resize);
    init();

    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_MODE, GL_EXP);
    glFogf(GL_FOG_DENSITY, 0.002);

    seaTexW = malloc(sizeof(int));
    seaTexH = malloc(sizeof(int));
    getImageWH("../sea02.ppm", seaTexW, seaTexH);
    seaTex = malloc(sizeof(GLubyte) * 3 * *seaTexW * *seaTexH);
    loadImage(seaTex, "../sea02.ppm", seaTexW, seaTexH);
    seaTexID = malloc(sizeof(GLuint));
    createTexture(seaTex, seaTexW, seaTexH, seaTexID);
    sea = gluNewQuadric();
    glShadeModel(GL_SMOOTH);
    gluQuadricNormals(sea, GLU_SMOOTH);
    gluQuadricTexture(sea, GL_TRUE);


    // allocating sky texture sizes
    skyTexW = malloc(sizeof(int));
    skyTexH = malloc(sizeof(int));

    // reading image width/height
    getImageWH("../sky08.ppm", skyTexW, skyTexH);

    // allocating image size
    skyTex = malloc(sizeof(GLubyte) * 3 * *skyTexW * *skyTexH);

    // loading the image to initialized value
    loadImage(skyTex, "../sky08.ppm", skyTexW, skyTexH);

    // allocating texture id
    skyTexID = malloc(sizeof(GLuint));

    // creating texture
    createTexture(skyTex, skyTexW, skyTexH, skyTexID);

    // initializing quadric
    sky = gluNewQuadric();

    // setting shading mode for quadric
    glShadeModel(GL_SMOOTH);

    // compute normals per vertex
    gluQuadricNormals(sky, GLU_SMOOTH);

    // setting textures to be active for this quadric
    gluQuadricTexture(sky, GL_TRUE);


    // allocating mountain texture sizes
    mountTexW = malloc(sizeof(int));
    mountTexH = malloc(sizeof(int));

    // reading image width/height
    getImageWH("../mount03.ppm", mountTexW, mountTexH);

    // allocating image size
    mountTex = malloc(sizeof(GLubyte) * 3 * *mountTexW * *mountTexH);

    // loading the image to initialized value
    loadImage(mountTex, "../mount03.ppm", mountTexW, mountTexH);

    // allocating texture id
    mountTexID = malloc(sizeof(GLuint));

    // creating texture
    createTexture(mountTex, mountTexW, mountTexH, mountTexID);




    cessna->y = 100;
    printf("Scene Controls\n");
    printf("----------------\n\n");
    printf("q: quit program\n");                // done
    printf("w: toggle wireframe\n");            // done
    printf("f: toggle fullscreen\n");           // done
    printf("d: enable/disable lighting\n");     // done
    printf("b: toggle fog\n");                  // done
    printf("t: toggle mountain texture\n");     // done
    printf("m: toggle mountain\n\n");           // done
    printf("s: show grid\n\n");                 // done
    printf("Camera Controls\n");
    printf("----------------\n\n");
    printf("PAGE UP: increase speed\n");        // done
    printf("PAGE DOWN: decrease speed\n");      // done
    printf("Move mouse left or right to turn aircraft\n");
    printf("Move mouse up or down to control altitude\n");

    glutMainLoop();

    return 0;
}
