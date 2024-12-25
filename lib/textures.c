/***
 * AUTHOR: Alex Pettipas
 * ID: B00813771
 * Texture reading and storing
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include "textures.h"


/********************************************************************************************

	Function:		loadImage

	Description:	Obtains the width and height values for the image.

    Parameters:     filename : string representing the filename
                    imageWidth : width of the image in pixels
                    imageHeight : height of the image in pixels

    Author:         S. Brooks

    Date Accessed:  Nov 25th, 2023

*********************************************************************************************/
void getImageWH(char* fileName, int *imageWidth, int *imageHeight) {
    FILE *fileID; // the ID of the image file
    int  maxValue; // maxValue
    char tempChar; // temporary character
    char headerLine[100]; // array for reading in header information
    fileID = fopen(fileName, "r"); // open the image file for reading

    // read in the first header line
    //    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
    //    - so we are just reading everything up to the first line break
    fscanf(fileID,"%[^\n] ", headerLine);

    // make sure that the image begins with 'P3', which signifies a PPM file
    if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
    {
        printf("This is not a PPM file!\n");
        exit(0);
    }

    fscanf(fileID, "%c", &tempChar); // read in the first character of the next line

    while(tempChar == '#') // while we still have comment lines (which begin with #)
    {
        // read in the comment
        fscanf(fileID, "%[^\n] ", headerLine);

        // print the comment
        printf("%s\n", headerLine);

        // read in the first character of the next line
        fscanf(fileID, "%c",&tempChar);
    }

    // the last one was not a comment character '#', so we need to put it back into the file stream (undo)
    ungetc(tempChar, fileID);
    // read in the image height, width and the maximum value
    fscanf(fileID, "%d %d %d", imageWidth, imageHeight, &maxValue);

    // print out the information about the image file
    printf("%d rows  %d columns  max value= %d\n", *imageHeight, *imageWidth, maxValue);

    fclose(fileID);
}

/********************************************************************************************

	Function:		loadImage

	Description:	Loads texture from .ppm file

    Parameters:     filename : string representing the filename

    Author:         S. Brooks

    Date Accessed:  Nov 25th, 2023

*********************************************************************************************/
void loadImage(GLubyte *imageData, char* fileName, int *imageWidth, int *imageHeight)
{
    FILE *fileID; // the ID of the image file
    int  maxValue; // maxValue
    int  totalPixels; // total number of pixels in the image
    char tempChar; // temporary character
    int i; // counter variable for the current pixel in the image
    char headerLine[100]; // array for reading in header information
    float RGBScaling; // if the original values are larger than 255
    int red, green, blue; // temporary variables for reading in the red, green and blue data of each pixel
    fileID = fopen(fileName, "r"); // open the image file for reading

    // read in the first header line
    //    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
    //    - so we are just reading everything up to the first line break
    fscanf(fileID,"%[^\n] ", headerLine);

    // make sure that the image begins with 'P3', which signifies a PPM file
    if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
    {
        printf("This is not a PPM file!\n");
        exit(0);
    }

    // read in the first character of the next line
    fscanf(fileID, "%c", &tempChar);


    while(tempChar == '#') // while we still have comment lines (which begin with #)
    {
        fscanf(fileID, "%[^\n] ", headerLine); // read in the comment
        printf("%s\n", headerLine); // print the comment
        fscanf(fileID, "%c",&tempChar); // read in the first character of the next line
    }

    // the last one was not a comment character '#', so we need to put it back into the file stream (undo)
    ungetc(tempChar, fileID);

    // read in the image hieght, width and the maximum value
    fscanf(fileID, "%d %d %d", imageWidth, imageHeight, &maxValue);

    // print out the information about the image file
    printf("%d rows  %d columns  max value= %d\n", *imageHeight, *imageWidth, maxValue);

    totalPixels = *imageWidth * *imageHeight; // compute the total number of pixels in the image
    RGBScaling = 255.0 / maxValue; // determine the scaling for RGB values

    // if the maxValue is 255 then we do not need to scale the
    //    image data values to be in the range or 0 to 255
    if (maxValue == 255)
    {
        for(i = 0; i < totalPixels; i++)
        {
            // read in the current pixel from the file
            fscanf(fileID,"%d %d %d",&red, &green, &blue );

            // store the red, green and blue data of the current pixel in the data array
            imageData[3*totalPixels - 3*i - 3] = red;
            imageData[3*totalPixels - 3*i - 2] = green;
            imageData[3*totalPixels - 3*i - 1] = blue;
        }
    }
    else  // need to scale up the data values
    {
        for(i = 0; i < totalPixels; i++)
        {
            // read in the current pixel from the file
            fscanf(fileID,"%d %d %d",&red, &green, &blue );

            // store the red, green and blue data of the current pixel in the data array
            imageData[3*totalPixels - 3*i - 3] = red   * RGBScaling;
            imageData[3*totalPixels - 3*i - 2] = green * RGBScaling;
            imageData[3*totalPixels - 3*i - 1] = blue  * RGBScaling;
        }
    }

    fclose(fileID); // close the image file
}


void createTexture(GLuint *imageData, int* imageWidth, int* imageHeight, GLuint *texID)
{
    // enable texture mapping
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, texID);

    glBindTexture(GL_TEXTURE_2D, *texID);
    // set the current texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, *imageWidth, *imageHeight, GL_RGB,
                      GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
}