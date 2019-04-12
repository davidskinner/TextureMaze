
//---------------------------------------
// Program: texture3.cpp
// Purpose: Texture map brick photograph
//          onto a cube model.
// Author:  John Gauch
// Date:    April 2011
// Notes: Thanks programming dad!
//---------------------------------------

#ifdef __linux__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include "libim/im_color.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

// Global variables
#define ROTATE 1
#define TRANSLATE 2
int xangle = 10;
int yangle = 10;
int zangle = 10;
int xpos = 0;
int ypos = 0;
int zpos = 0;
int mode = ROTATE;

int rows = 0;
int cols = 0;
int startRow = 0;
int startCol = 0;
vector<vector<char>> maze;

//---------------------------------------
// Initialize texture image
//---------------------------------------
void init_texture(char *name, unsigned char *&texture, int &xdim, int &ydim)
{
    // Read jpg image
    im_color image;
    image.ReadJpg(name);
    printf("Reading image %s\n", name);
    xdim = 1; while (xdim < image.R.Xdim) xdim*=2; xdim /=2;
    ydim = 1; while (ydim < image.R.Ydim) ydim*=2; ydim /=2;
    image.Interpolate(xdim, ydim);
    printf("Interpolating to %d by %d\n", xdim, ydim);
    
    // Copy image into texture array
    texture = (unsigned char *)malloc((unsigned int)(xdim*ydim*3));
    int index = 0;
    for (int y = 0; y < ydim; y++)
        for (int x = 0; x < xdim; x++)
        {
            texture[index++] = (unsigned char)(image.R.Data2D[y][x]);
            texture[index++] = (unsigned char)(image.G.Data2D[y][x]);
            texture[index++] = (unsigned char)(image.B.Data2D[y][x]);
        }
}

//---------------------------------------
// Function to draw 3D block
//---------------------------------------
void block(float xmin, float ymin, float zmin,
           float xmax, float ymax, float zmax)
{
    // Define 8 vertices
    float ax = xmin, ay = ymin, az = zmax;
    float bx = xmax, by = ymin, bz = zmax;
    float cx = xmax, cy = ymax, cz = zmax;
    float dx = xmin, dy = ymax, dz = zmax;
    
    float ex = xmin, ey = ymin, ez = zmin;
    float fx = xmax, fy = ymin, fz = zmin;
    float gx = xmax, gy = ymax, gz = zmin;
    float hx = xmin, hy = ymax, hz = zmin;
    
    // Draw 6 faces
    glBegin(GL_POLYGON);  // Max texture coord 1.0
    glTexCoord2f(0.0, 0.0);
    glVertex3f(ax, ay, az);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(bx, by, bz);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(cx, cy, cz);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(dx, dy, dz);
    glEnd();
    
    glBegin(GL_POLYGON);  // Max texture coord 1.0
    glTexCoord2f(0.0, 0.0);
    glVertex3f(ex, ey, ez);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(ax, ay, az);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(dx, dy, dz);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(hx, hy, hz);
    glEnd();
    
    glBegin(GL_POLYGON);  // Max texture coord 1.0
    glTexCoord2f(0.0, 0.0);
    glVertex3f(fx, fy, fz);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(ex, ey, ez);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(hx, hy, hz);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(gx, gy, gz);
    glEnd();
    
    glBegin(GL_POLYGON);  // Max texture coord 1.0
    glTexCoord2f(0.0, 0.0);
    glVertex3f(bx, by, bz);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(fx, fy, fz);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(gx, gy, gz);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(cx, cy, cz);
    glEnd();
    
    glBegin(GL_POLYGON);  // Max texture coord 3.0
    glTexCoord2f(0.0, 0.0);
    glVertex3f(ax, ay, az);
    glTexCoord2f(2.0, 0.0);
    glVertex3f(ex, ey, ez);
    glTexCoord2f(2.0, 2.0);
    glVertex3f(fx, fy, fz);
    glTexCoord2f(0.0, 2.0);
    glVertex3f(bx, by, bz);
    glEnd();
    
    glBegin(GL_POLYGON);  // Max texture coord 3.0
    glTexCoord2f(0.0, 0.0);
    glVertex3f(gx, gy, gz);
    glTexCoord2f(3.0, 0.0);
    glVertex3f(cx, cy, cz);
    glTexCoord2f(3.0, 3.0);
    glVertex3f(dx, dy, dz);
    glTexCoord2f(0.0, 3.0);
    glVertex3f(hx, hy, hz);
    glEnd();
}

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void init()
{
    // Init view
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
    glEnable(GL_DEPTH_TEST);
    
    // Init texture
    int xdim, ydim;
    unsigned char *texture;
    init_texture((char *)"textures/brick0.jpg", texture, xdim, ydim);
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xdim, ydim, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display()
{
    // Incrementally rotate objects
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(xpos / 500.0, ypos / 500.0, zpos / 500.0);
    glRotatef(xangle, 1.0, 0.0, 0.0);
    glRotatef(yangle, 0.0, 1.0, 0.0);
    glRotatef(zangle, 0.0, 0.0, 1.0);
    
    // Draw objects
    block(-1, -1, -1, 1, 1, 1);
    glFlush();
}

//---------------------------------------
// Keyboard callback for OpenGL
//---------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    // Determine if we are in ROTATE or TRANSLATE mode
    if ((key == 'r') || (key == 'R'))
    {
        printf("Type x y z to decrease or X Y Z to increase ROTATION angles.\n");
        mode = ROTATE;
    }
    else if ((key == 't') || (key == 'T'))
    {
        printf("Type x y z to decrease or X Y Z to increase TRANSLATION distance.\n");
        mode = TRANSLATE;
    }
    
    // Handle ROTATE
    if (mode == ROTATE)
    {
        if (key == 'x')
            xangle -= 5;
        else if (key == 'y')
            yangle -= 5;
        else if (key == 'z')
            zangle -= 5;
        else if (key == 'X')
            xangle += 5;
        else if (key == 'Y')
            yangle += 5;
        else if (key == 'Z')
            zangle += 5;
        glutPostRedisplay();
    }
    
    // Handle TRANSLATE
    if (mode == TRANSLATE)
    {
        if (key == 'x')
            xpos -= 5;
        else if (key == 'y')
            ypos -= 5;
        else if (key == 'z')
            zpos -= 5;
        else if (key == 'X')
            xpos += 5;
        else if (key == 'Y')
            ypos += 5;
        else if (key == 'Z')
            zpos += 5;
        glutPostRedisplay();
    }
}

//---------------------------------------
// Mouse callback for OpenGL
//---------------------------------------
void mouse(int button, int state, int x, int y)
{
    // Handle mouse down
    static int xdown, ydown;
    if (state == GLUT_DOWN)
    {
        xdown = x;
        ydown = y;
    }
    
    // Handle ROTATE
    if ((mode == ROTATE) && (state == GLUT_UP))
    {
        xangle += (y - ydown);
        yangle -= (x - xdown);
        zangle = 0;
        glutPostRedisplay();
    }
    
    // Handle TRANSLATE
    if ((mode == TRANSLATE) && (state == GLUT_UP))
    {
        xpos += (x - xdown);
        ypos -= (y - ydown);
        glutPostRedisplay();
    }
}


void ReadInMuhTextMAN(){
    
    string line, token;
    ifstream file("textYo.txt");
    
    int i=0;
    vector<int> vec;
    
    // read in first two rows
    while(i < 2)
    {
        getline(file, line);
        stringstream s(line);
        
        while (s >> token)
        {
            vec.push_back(stoi(token));
        }
        i++;
    }
    
    rows = vec[0];
    cols = vec[1];
    startCol = vec[2];
    startRow = vec[3];

    // read in maze
    for (int i = 0; i < rows; i++) {
        getline(file, line);
        vector<char> row;
        for (int j = 0; j <  cols; j++) {
            row.push_back(line[j]);
        }
        maze.push_back(row);
    }
    
    
    
    
    
    
}

    
    


//---------------------------------------
// Main program
//---------------------------------------
int main(int argc, char *argv[])
{
    // Create OpenGL window
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(250, 250);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutCreateWindow("Texture");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    init();
    ReadInMuhTextMAN();

    printf("Keyboard commands:\n");
    printf("   't' or 'T' - go to translate mode\n");
    printf("   'r' or 'R' - go to rotate mode\n");
    printf("   'x' - rotate or translate on x-axis -5\n");
    printf("   'X' - rotate or translate on x-axis +5\n");
    printf("   'y' - rotate or translate on y-axis -5\n");
    printf("   'Y' - rotate or translate on y-axis +5\n");
    printf("Mouse operations:\n");
    printf("   'mouse down' - start selecting rotation direction\n");
    printf("   'mouse up' - finish selecting rotation direction\n");
    glutMainLoop();
    return 0;
}
