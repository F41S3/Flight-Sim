//
// Created by false on 04/12/23.
//

#ifndef UNTITLED_MOUNTAIN_H
#define UNTITLED_MOUNTAIN_H

typedef struct _Mountain {
    float ***verts;
    float ***normals;
    int mountain_width;
    int mountain_height;
    float x;
    float y;
    float heightScale;
} Mountain;

Mountain *initMountain(int width, int height, float xpos, float ypos, float heightScale);
float **genVectors(int width, int height);
void genMountain(Mountain *m, int width, int height);
void calculateNormals(float*** plane, float ***normals, int width, int height);
#endif //UNTITLED_MOUNTAIN_H
