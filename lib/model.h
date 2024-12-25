/***
 * AUTHOR: Alex Pettipas
 * ID: B00813771
 */


#ifndef UNTITLED_MODEL_H
#define UNTITLED_MODEL_H
#include "linked_list.h"

typedef struct _model {
    LinkedList* model; // misleading, contains a LList for components, which contains a LList for faces, which contains
    // an LList for vertices containing indexes for the verts and norms 2d arrays.
    float** verts;
    float** norms;
    int numVerts;
    double x;
    double y;
    double z;
    char* name;
} Model;

// size of vertices/normals and name
Model* model_initialize(int, char*);
void model_cpy_verts(Model*, LinkedList*);
void model_cpy_norms(Model*, LinkedList*);
Model* readModel(char* fileName);
void centerVerts(Model*);



#endif //UNTITLED_MODEL_H
