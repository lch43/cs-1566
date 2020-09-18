/*
 * template.c
 *
 * An OpenGL source code template.
 */

/* Modified by Landon Higinbotham to create a cone and color it. */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdio.h>
#include <math.h>

#include "../libraries/initShader.h"
#include "../libraries/vandmlib.h"


#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

/*Landon Higinbotham's code starts here*/
/* For now we can keep this constant since we will not be changing it*/
const int coneSides = 100;
/*Vertices = number of vertical sides of the cone * 2 since we must create the bottom faces, and * 3 because each triangle has 3 vertices.*/
int num_vertices = coneSides * 2 * 3;

/*
    We are given the point of the cone's Y position. Assuming it will always stay in the middle of x and z.
    We are given its height which will be the distance below the tipY that the cone will extend.
    We are given the width which will be given as the radius of the base of the cone.
    We are given the array of vertices to place the values into.
*/
void createCone(float tipY, float height, float width, vec4 * vertices)
{
    int side;
    /*Take degrees of circle because it is easy for me to understand this way.*/
    double angle = 360.0/coneSides;
    float heightOffset = tipY - height;
    for (side = 0; side<coneSides; side++)
    {
        int vertexOffset = side * 6;
        /*Gain the radians needed to create the triangle*/
        float rad1 = (angle * side) * M_PI/180.0;
        float rad2 = (angle * (side+1)) * M_PI/180.0;

        /*Using the radians, find the X and Y's of the bottom points.*/
        float base1X = (float) (width * sin(rad1));
        float base1Z = (float) (width * cos(rad1));
        float base2X = (float) (width * sin(rad2));
        float base2Z = (float) (width * cos(rad2));

        /*Vertical sides*/
        vertices[vertexOffset] = (vec4) {0.0, tipY, 0.0 , 1.0};
        vertices[vertexOffset+1] = (vec4) {base1X, heightOffset, base1Z, 1.0};
        vertices[vertexOffset+2] = (vec4) {base2X, heightOffset, base2Z, 1.0};

        /*Base sides*/
        vertices[vertexOffset+3] = (vec4) {0.0, heightOffset, 0.0, 1.0};
        vertices[vertexOffset+4] = (vec4) {base1X, heightOffset, base1Z, 1.0};
        vertices[vertexOffset+5] = (vec4) {base2X, heightOffset, base2Z, 1.0};
        
    }
}

void colorCone(vec4 * colors)
{
    int side;
    for (side = 0; side<coneSides; side++)
    {
        /*Choose random colors.*/
        float ColorR = rand() % 256 / 255.0;
        float ColorG = rand() % 256 / 255.0;
        float ColorB = rand() % 256 / 255.0;
        
        int vertexOffset = side * 6;

        /*Make vertical side colors*/
        colors[vertexOffset] = (vec4) {ColorR,ColorG,ColorB,1.0};
        colors[vertexOffset+1] = (vec4) {ColorR,ColorG,ColorB,1.0};
        colors[vertexOffset+2] = (vec4) {ColorR,ColorG,ColorB,1.0};

        /*Just set the bottom to white*/
        colors[vertexOffset+3] = (vec4) {1.0,1.0,1.0,1.0};
        colors[vertexOffset+4] = (vec4) {1.0,1.0,1.0,1.0};
        colors[vertexOffset+5] = (vec4) {1.0,1.0,1.0,1.0};
    }
}

void rainbowColorCone(vec4 * colors)
{
    int side;
    float colorVariation = 765.00 / (coneSides);
    for (side = 0; side<coneSides; side++)
    {
        float currentColor = colorVariation * side;
        float ColorR;
        float ColorG;
        float ColorB;
        if (currentColor >= 255)
        {
            if (currentColor >= 510)
            {
                ColorR = (0.0 + (currentColor-510.0))/255.0;
                ColorG = 0.0;
                ColorB = (255.0 - (currentColor-510.0))/255.0;
            }
            else
            {
                ColorR = 0.0;
                ColorG = (255.0 - (currentColor-255.0))/255.0;
                ColorB = (0.0 + (currentColor-255.0))/255.0;
            }
        }
        else
        {
            ColorR = (255.0 - currentColor)/255.0;
            ColorG = (0.0 + currentColor)/255.0;
            ColorB = 0.0;
        }
        
        int vertexOffset = side * 6;

        /*Make vertical side colors*/
        colors[vertexOffset] = (vec4) {ColorR,ColorG,ColorB,1.0};
        colors[vertexOffset+1] = (vec4) {ColorR,ColorG,ColorB,1.0};
        colors[vertexOffset+2] = (vec4) {ColorR,ColorG,ColorB,1.0};

        /*Just set the bottom to white*/
        colors[vertexOffset+3] = (vec4) {1.0,1.0,1.0,1.0};
        colors[vertexOffset+4] = (vec4) {1.0,1.0,1.0,1.0};
        colors[vertexOffset+5] = (vec4) {1.0,1.0,1.0,1.0};
    }
}
/*Landon Higinbotham's code ends here*/
void init(void)
{
    /*Landon Higinbotham's code starts here*/
    /*Create the vertices array with the appropriate size*/
    vec4 vertices[num_vertices];
    /*Populate vertices array*/
    createCone(0.5, 1, .5, vertices);
    /*Create the colors array with the appropriate size*/
    vec4 colors[num_vertices];
    /*Populate colors array*/
    colorCone(colors);
    /*Landon Higinbotham's code ends here*/

    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
    	glutLeaveMainLoop();

    //glutPostRedisplay();
}

void reshape(int width, int height)
{
    glViewport(0, 0, 512, 512);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Landon Higinbotham - Lab03 Cone");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}
