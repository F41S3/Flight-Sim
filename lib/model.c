/***
 * AUTHOR: Alex Pettipas
 * ID: B00813771
 * Model reading and storing from text files
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "linked_list.h"
#include "model.h"

/********************************************************************************************

	Function:		model_initialize

	Description:	Initialized a Model struct and allocated memory for it

    Parameters:     numVertices : Number of vertices to be stored
                    modelName : string representing the name of the model

*********************************************************************************************/
Model* model_initialize(int numVertices, char* modelName) {
    Model* m = malloc(sizeof(Model));
    m->model = malloc(sizeof(LinkedList));
    m->numVerts = numVertices;
    m->verts = (float**) malloc(numVertices * sizeof(float*) * 3);
    for (int i = 0; i < numVertices; i++) {
        m->verts[i] = (float*) malloc(3 * sizeof(float));
    }

    m->norms = (float**) malloc(numVertices * sizeof(float*) * 3);
    for (int i = 0; i < numVertices; i++) {
        m->norms[i] = (float*) malloc(3 * sizeof(float));
    }

    m->x = 0.0;
    m->y = 0.0;
    m->z = 0.0;
    m->name = malloc(sizeof(modelName));
    memcpy((void*) m->name, (void*) modelName, sizeof(modelName));

    return m;
}

/********************************************************************************************

	Function:		model_cpy_verts

	Description:	copies vertices from a linked list to the array stored in the model

    Parameters:     m : Model struct
                    lst : Linked list storing vertex data

*********************************************************************************************/
void model_cpy_verts(Model* m, LinkedList* lst) {
    float n[m->numVerts][3];
    memcpy((void*) &n, llist_to_array(lst), lst->size * lst->itemSize);

    for (int i = 0; i < lst->size; i++) {
        for (int j = 0; j < 3; j++) {
            memcpy((void*) &m->verts[i][j], (void*) &n[i][j], sizeof(float));
        }
    }
}

/********************************************************************************************

	Function:		model_cpy_norms

	Description:	copies normal vectors from a linked list to the array stored in the model

    Parameters:     m : Model struct
                    lst : Linked list storing normal vector data

*********************************************************************************************/
void model_cpy_norms(Model* m, LinkedList* lst) {
    float n[m->numVerts][3];
    memcpy((void*) &n, llist_to_array(lst), lst->size * lst->itemSize);

    for (int i = 0; i < lst->size; i++) {
        for (int j = 0; j < 3; j++) {
            memcpy((void*) &m->norms[i][j], (void*) &n[i][j], sizeof(float));
        }
    }
}

/********************************************************************************************

	Function:		readModel

	Description:	Reads and stores data from file in a model object

    Parameters:     filename : string representing the filename

*********************************************************************************************/
Model* readModel(char* fileName) {
    FILE* file = fopen(fileName, "r");
    int bufSize = sizeof(char) * 200;
    char* buffer = malloc(bufSize);
    LinkedList* vertices= llist_initialize(sizeof(float) * 3, "vertices");
    LinkedList* normals = llist_initialize(sizeof(float) * 3, "normals");
    LinkedList* model = llist_initialize(sizeof(LinkedList), "components");

    LinkedList* faces = llist_initialize(sizeof(LinkedList), "faces");
    LinkedList* ints;

    while (fgets(buffer, bufSize, file)) {
        char c;
        sscanf(buffer, "%c", &c);

        if (c == 'g') {
            // if there was an initialized LList, add it to the model LList
            char s[100];
            sscanf(buffer, "%c %s\n", &c, s);
            llist_add_last(model, (void*) faces);
            faces = llist_initialize(sizeof(LinkedList), s);
        }
        if (c == 'v') {
            // storing vertex data in a linked list
            float fl[3];
            int i = 0;
            char* token = strtok(buffer, " ");
            while (token != NULL) {
                if (sscanf(token, "%f", &fl[i]) == 1) {
                    i++;
                }
                token = strtok(NULL, " ");
            }
            llist_add_last(vertices, (void*) fl);

        } else if (c == 'n') {
            // storing normal vector data in linked list
            float fl[3];
            int i = 0;
            char* token = strtok(buffer, " ");
            while (token != NULL) {
                if (sscanf(token, "%f", &fl[i]) == 1) {
                    i++;
                }
                token = strtok(NULL, " ");
            }
            llist_add_last(normals, (void*) fl);
        } else {
            // initializing a new LList to store ints
            ints = llist_initialize(sizeof(int), "int");

            int i;
            char* token = strtok(buffer, " ");
            while (token != NULL) {
                if (sscanf(token, "%d", &i) == 1) {
                    //printf("%d\n", i);
                    llist_add_last(ints, (void*) &i);
                }
                token = strtok(NULL, " ");
            }
            llist_add_last(faces, (void*) ints);

        }
    }
    llist_add_last(model, (void*) faces); // adding the last left over face LL to the component LL

    fclose(file);
    free(buffer);

    Model* m = model_initialize(vertices->size, "Cessna");
    memcpy(m->model, model, sizeof(LinkedList));

    // copying vertices and normal values from the large linked lists to smaller arrays
    model_cpy_verts(m, vertices);
    model_cpy_norms(m, normals);

    return m;
}

/********************************************************************************************

	Function:		centerVerts

	Description:	Centers the vertices to be around an average point of (0, 0, 0)

    Parameters:     m : model struct holding loaded model data.

*********************************************************************************************/
void centerVerts(Model *m) {
    float avgx = 0.0f;
    float avgy = 0.0f;
    float avgz = 0.0f;
    for (int i = 0; i < m->numVerts; i++) {
        avgx += m->verts[i][0];
        avgy += m->verts[i][1];
        avgz += m->verts[i][2];
    }
    avgx /= m->numVerts;
    avgy /= m->numVerts;
    avgz /= m->numVerts;
    printf("%f, %f, %f\n", avgx, avgy, avgz);
    for (int i = 0; i < m->numVerts; i++) {
        m->verts[i][0] -= avgx;
        m->verts[i][1] -= avgy;
        m->verts[i][2] -= avgz;
    }
}
