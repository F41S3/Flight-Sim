/***
 * AUTHOR: Alex Pettipas
 * ID: B00813771
 */

#ifndef UNTITLED_TEXTURES_H
#define UNTITLED_TEXTURES_H
#include "GL/freeglut.h"


void getImageWH(char* fileName, int *imageWidth, int *imageHeight);
void loadImage(GLubyte *imageData, char* fileName, int *imageWidth, int *imageHeight);
void createTexture(GLuint *imageData, int* imageWidth, int* imageHeight, GLuint *texID);



#endif //UNTITLED_TEXTURES_H
