/***
 * AUTHOR: Alex Pettipas
 * ID: B00813771
 * Randomized mountain generation using pseudo perlin noise
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mountain.h"

/********************************************************************************************

	Function:		genVectors

	Description:	Generates a layered perlin noise approximation using random values

    Parameters:     width : int value representing the width of the noise image

                    height : int value representing the height of the noise image

                    xpos : x position of the mountain

                    ypos : y position of the mountain

                    heightScale : scaling the height of the vertices of the mountain, needed
                                   for correct normal value computation.

*********************************************************************************************/
Mountain *initMountain(int width, int height, float xpos, float ypos, float heightScale) {
    Mountain *m = malloc(sizeof(Mountain));
    m->mountain_height = height;
    m->mountain_width = width;
    m->x = xpos;
    m->y = ypos;
    m->heightScale = heightScale;

    // memory allocation
    m->verts = (float***) malloc(sizeof(float*) * width * height * 2);
    m->normals = (float***) malloc(sizeof(float*) * width * height * 2);
    for (int i = 0; i < width; i++) {
        m->verts[i] = (float**) malloc(sizeof(float*) * height * 2);
        m->normals[i] = (float**) malloc(sizeof(float*) * height * 2);
        for (int j = 0; j < height; j++) {
            m->verts[i][j] = (float*) malloc(sizeof(float*) * 3);
            m->normals[i][j] = (float*) malloc(sizeof(float*) * 3);
        }
    }

    return m;
}

/********************************************************************************************

	Function:		genVectors

	Description:	Generates a layered perlin noise approximation using random values

    Parameters:     width : int value representing the width of the noise image

                    height : int value representing the height of the noise image

    Author:         S. Lague

    Date Accessed:  Dec 1st, 2023

    URL:            https://www.youtube.com/watch?v=wbpMiKiSKm8&list=PLFt_AvWsXl0eBW2EiBtl_sxmDtSgZBxB3

    Note:           Tutorial series was helpful in understanding but I ended up creating a different
                    approximation, it instead generates two degrees and uses the cross product formula.

*********************************************************************************************/
float **genVectors(int width, int height) {
    float perlinHigh[width][height];
    float perlinMed[width][height];
    float perlinLow[width][height];
    float perlinLower[width][height];


    /***
     * high frequency noise, generates a new value for each point on the plane
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i == 0 || i == width-1 || j == 0 || j == height-1) {
                perlinHigh[i][j] = 0.0f;
            } else {
                perlinHigh[i][j] = 0.5f * cosf(fabs((float) ((rand() % 360) * (M_PI / 180)) - (float) ((rand() % 360) * (M_PI / 180)))) + 1;
            }
        }
    }

    /***
     * med frequency noise, generates a new value for every 5th point on the plane and interpolates between them
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j+=5) {
            float u0;
            if (j==0) {
                u0 = 0.0f;
            } else {
                u0 = perlinMed[i][j];
            }
            float u1;
            if (j >= height-6) {
                u1 = 0.0f;
            } else {
                u1 = 0.5f * cosf(fabs((float) ((rand() % 360) * (M_PI / 180)) - (float) ((rand() % 360) * (M_PI / 180)))) + 1;
            }
            for (int k = 0; k < 5; k++) {
                perlinMed[i][j+k] = (1 - ((float) k / 5.0f)) * u0 + ((float) k / 5.0f) * u1;
            }
        }
    }
    for (int i = 0; i < width; i+=5) {
        for (int j = 0; j < height; j++) {
            float u0 = perlinMed[i][j];
            float u1 = perlinMed[i+5][j];
            for (int k = 0; k < 5; k++) {
                perlinMed[i+k][j] = (1 - ((float) k / 5.0f)) * u0 + ((float) k / 5.0f) * u1;
            }
        }
    }

    /***
     * low frequency noise, generates a new value for every 10th point on the plane and interpolates between them
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j+=10) {
            float u0;
            if (j==0) {
                u0 = 0.0f;
            } else {
                u0 = perlinLow[i][j];
            }
            float u1;
            if (j >= height-11) {
                u1 = 0.0f;
            } else {
                u1 = 0.5f * cosf(fabs((float) ((rand() % 360) * (M_PI / 180)) - (float) ((rand() % 360) * (M_PI / 180)))) + 1;
            }
            for (int k = 0; k <= 10; k++) {
                perlinLow[i][j+k] = (1 - ((float) k / 10.0f)) * u0 + ((float) k / 10.0f) * u1;
            }
        }
    }
    for (int i = 0; i < width; i+=10) {
        for (int j = 0; j < height; j++) {
            float u0 = perlinLow[i][j];
            float u1 = perlinLow[i+10][j];
            for (int k = 0; k < 10; k++) {
                perlinLow[i+k][j] = (1 - ((float) k / 10.0f)) * u0 + ((float) k / 10.0f) * u1;
            }
        }
    }

    /***
     * lowest frequency noise, generates a new value for every 20th point on the plane and interpolates between them
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j+=20) {
            float u0;
            if (j==0) {
                u0 = 0.0f;
            } else {
                u0 = perlinLower[i][j];
            }
            float u1;
            if (j >= height-21) {
                u1 = 0.0f;
            } else {
                u1 = 0.5f * cosf(fabs((float) ((rand() % 360) * (M_PI / 180)) - (float) ((rand() % 360) * (M_PI / 180)))) + 1;
            }
            for (int k = 0; k <= 20; k++) {
                perlinLower[i][j+k] = (1 - ((float) k / 20.0f)) * u0 + ((float) k / 20.0f) * u1;
            }
        }
    }
    for (int i = 0; i < width; i+=20) {
        for (int j = 0; j < height; j++) {
            float u0;
            if (i == 0) {
                u0 = 0.0f;
            } else {
                u0 = perlinLower[i][j];
            }
            float u1;
            if (i > width-21) {
                u1 = 0.0f;
            } else {
                u1 = perlinLower[i+20][j];
            }
            for (int k = 0; k < 20; k++) {
                perlinLower[i+k][j] = (1 - ((float) k / 20.0f)) * u0 + ((float) k / 20.0f) * u1;
            }
        }
    }

    /***
     * initializing memory space for final pointer to return
     */
    float **perlinNoise = (float**) malloc(sizeof(float*) * width * height);
    for (int i = 0; i < width; i++) {
        perlinNoise[i] = (float*) malloc(sizeof(float*) * height);
        for (int j = 0; j < height; j++) {
            perlinNoise[i][j] = 0;
        }
    }

    /***
     * adding together the four perlin noise maps to obtain a varied noise image, scaled based on trial/error
     */
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            perlinNoise[i][j] = 0.0f;
            perlinNoise[i][j] += perlinHigh[i][j];
            perlinNoise[i][j] += perlinMed[i][j]*2;
            perlinNoise[i][j] += perlinLow[i][j]*2;
            perlinNoise[i][j] +=  perlinLower[i][j] * 30;
        }
    }
    return perlinNoise;
}

/********************************************************************************************

	Function:		genMountain

	Description:	Generates a mountain terrain based on a generated perlin noise array

    Parameters:     m : initialized mountain pointer storing the various data components

                    width : int value representing the width of the noise image

                    height : int value representing the height of the noise image

    Author:         S. Lague

    Date Accessed:  Dec 1st, 2023

    URL:            https://www.youtube.com/watch?v=wbpMiKiSKm8&list=PLFt_AvWsXl0eBW2EiBtl_sxmDtSgZBxB3

*********************************************************************************************/
void genMountain(Mountain* m, int width, int height) {
    float **perlinNoise = (float**) malloc(sizeof(float*) * width * height);
    for (int i = 0; i < width; i++) {
        perlinNoise[i] = (float*) malloc(sizeof(float*) * height);
        for (int j = 0; j < height; j++) {
            perlinNoise[i][j] = 0;
        }
    }
    perlinNoise = genVectors(width, height);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            m->verts[i][j][0] = (float) i;
            m->verts[i][j][1] = 0.0f;
            m->verts[i][j][2] = (float) j;
        }
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            m->verts[i][j][1] += perlinNoise[i][j] * m->heightScale;
        }
    }
}

/********************************************************************************************

	Function:		calculateNormals

	Description:	Calculates normal vectors based on an input vertex plane

    Parameters:     plane : vertex array representing an arbitrary size plane

                    normals : vector array to store computed normal values

                    width : int value representing the width of the plane

                    height : int value representing the height of the plane

    Author:         Libretexts

    Date Accessed:  Dec 4th, 2023

    URL:            https://math.libretexts.org/Bookshelves/Calculus/Supplemental_Modules_(Calculus)/Vector_Calculus/2%3A_Vector-Valued_Functions_and_Motion_in_Space/2.3%3A_Curvature_and_Normal_Vectors_of_a_Curve

*********************************************************************************************/
void calculateNormals(float*** plane, float ***normals, int width, int height){
    float dir = -1.0f; // mixed up vector direction, quick and dirty fix lol
    for (int i = 0; i < width-1; i++) {
        for (int j = 0; j < height-1; j++) {
            float u[] = {plane[i+1][j][0] - plane[i][j][0],
                         plane[i+1][j][1] - plane[i][j][1],
                         plane[i+1][j][2] - plane[i][j][2]};

            float v[] = {plane[i][j+1][0] - plane[i][j][0],
                         plane[i][j+1][1] - plane[i][j][1],
                         plane[i][j+1][2] - plane[i][j][2]};
            normals[i][j][0] = dir * (u[1]*v[2] - u[2]*v[1]);
            normals[i][j][1] = dir * -(u[0]*v[2]-u[2]*v[0]);
            normals[i][j][2] = dir * (u[0]*v[1] - u[1]*v[0]);
        }
    }
    for (int i = 1; i < width; i++) {
        float u[] = {plane[i-1][height-1][0] - plane[i][height-1][0],
                     plane[i-1][height-1][1] - plane[i][height-1][1],
                     plane[i-1][height-1][2] - plane[i][height-1][2]};

        float v[] = {plane[i][height-2][0] - plane[i][height-1][0],
                     plane[i][height-2][1] - plane[i][height-1][1],
                     plane[i][height-2][2] - plane[i][height-1][2]};
        normals[i][height-1][0] = dir * (u[1]*v[2] - u[2]*v[1]);
        normals[i][height-1][1] = dir * -(u[0]*v[2]-u[2]*v[0]);
        normals[i][height-1][2] = dir * (u[0]*v[1] - u[1]*v[0]);
    }
    for (int i = 1; i < height; i++) {
        float u[] = {plane[width-2][i][0] - plane[width-1][i][0],
                     plane[width-2][i][1] - plane[width-1][i][1],
                     plane[width-2][i][2] - plane[width-1][i][2]};

        float v[] = {plane[width-1][i-1][0] - plane[width-1][i][0],
                     plane[width-1][i-1][1] - plane[width-1][i][1],
                     plane[width-1][i-1][2] - plane[width-1][i][2]};
        normals[width-1][i][0] = dir * (u[1]*v[2] - u[2]*v[1]);
        normals[width-1][i][1] = dir * -(u[0]*v[2]-u[2]*v[0]);
        normals[width-1][i][2] = dir * (u[0]*v[1] - u[1]*v[0]);
    }

    float u[] = {plane[width-2][height-1][0] - plane[width-1][height-1][0],
                 plane[width-2][height-1][1] - plane[width-1][height-1][1],
                 plane[width-2][height-1][2] - plane[width-1][height-1][2]};

    float v[] = {plane[width-1][height-2][0] - plane[width-1][height-1][0],
                 plane[width-1][height-2][1] - plane[width-1][height-1][1],
                 plane[width-1][height-2][2] - plane[width-1][height-1][2]};
    normals[width-1][height-1][0] = dir * (u[1]*v[2] - u[2]*v[1]);
    normals[width-1][height-1][1] = dir * -(u[0]*v[2]-u[2]*v[0]);
    normals[width-1][height-1][2] = dir * (u[0]*v[1] - u[1]*v[0]);


    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            float length = sqrtf(powf(normals[i][j][0],2.0f)+
                                 powf(normals[i][j][1],2.0f)+
                                 powf(normals[i][j][2],2.0f));

            normals[i][j][0] = normals[i][j][0] / length;
            normals[i][j][1] = normals[i][j][1] / length;
            normals[i][j][2] = normals[i][j][2] / length;
        }
    }
}