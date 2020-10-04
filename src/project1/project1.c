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

const int numCylinders = 60;
const int numSidesPerCylinder = 10;
int num_vertices = (numCylinders * 2 + 2) * numSidesPerCylinder * 3;

GLuint ctm_location;
mat4 ctm = {
    (vec4) {1,0,0,0},
    (vec4) {0,1,0,0},
    (vec4) {0,0,1,0},
    (vec4) {0,0,0,1},
};

void createSpring(vec4 * vertices)
{
    const float cylinderRadius = .05;
    const float height = .5;
    const float width = .5;
    const float degreesOfRotation = 1080;

    double degreesToRadians = M_PI/180.00;

    int triangle;
    for (triangle = 0; triangle<numSidesPerCylinder*3; triangle+= 3)
    {
        double rad1 = triangle * 360/numSidesPerCylinder * degreesToRadians;
        double rad2 = (triangle + 1) * 360/numSidesPerCylinder * degreesToRadians;

        float rad1X = (float) (cylinderRadius * cos(rad1));
        float rad1Y = cylinderRadius * sin(rad1);
        float rad2X = cylinderRadius * cos(rad2);
        float rad2Y = cylinderRadius * sin(rad2);

        vertices[triangle] = (vec4) {width + 0.0, height + 0.0, 0.0, 1.0};
        vertices[triangle+1] = (vec4) {width + rad1X, height + rad1Y, 0.0, 1.0};
        vertices[triangle+2] = (vec4) {width + rad2X, height + rad2Y, 0.0, 1.0};
    }

    printf("%d\n", triangle);

    int cylinder;
    for (cylinder = 0; cylinder<numCylinders; cylinder++)
    {
        /*
        Plan is to interpolate the rotation and height values. Then create the cylinders and transform the location
        */
        double rotationRadians = -1 * cylinder * degreesOfRotation/numCylinders * degreesToRadians;
        double rotationRadians2 = -1 * (cylinder+1) * degreesOfRotation/numCylinders * degreesToRadians;

        double height1 = height - cylinder * ((height*2)/numCylinders);
        double height2 = height - (cylinder + 1) * ((height*2)/numCylinders);

        int side;
        for (side = 0; side<numSidesPerCylinder; side++)
        {
            double rad1 = side * 360/numSidesPerCylinder * degreesToRadians;
            double rad2 = (side + 1) * 360/numSidesPerCylinder * degreesToRadians;

            float rad1X = (float) (cylinderRadius * cos(rad1));
            float rad1Y = cylinderRadius * sin(rad1);
            float rad2X = cylinderRadius * cos(rad2);
            float rad2Y = cylinderRadius * sin(rad2);

            printf("%d\n", triangle+(cylinder * numSidesPerCylinder*6)+(side*6));

            vertices[triangle+(cylinder * numSidesPerCylinder*6)+(side*6)] = (vec4) {(width+rad1X)*cos(rotationRadians), height1+rad1Y, (width+rad1X)*sin(rotationRadians), 1.0}; //rad1 front
            vertices[triangle+(cylinder * numSidesPerCylinder*6)+(side*6)+1] = (vec4) {(width+rad1X)*cos(rotationRadians2), height2+rad1Y, (width+rad1X)*sin(rotationRadians2), 1.0}; //rad1 back
            vertices[triangle+(cylinder * numSidesPerCylinder*6)+(side*6)+2] = (vec4) {(width+rad2X)*cos(rotationRadians2), height2+rad2Y, (width+rad2X)*sin(rotationRadians2), 1.0}; //rad2 back

            vertices[triangle+(cylinder * numSidesPerCylinder*6)+(side*6)+3] = (vec4) {(width+rad2X)*cos(rotationRadians2), height2+rad2Y, (width+rad2X)*sin(rotationRadians2), 1.0}; //rad2 back
            vertices[triangle+(cylinder * numSidesPerCylinder*6)+(side*6)+4] = (vec4) {(width+rad2X)*cos(rotationRadians), height1+rad2Y, (width+rad2X)*sin(rotationRadians), 1.0}; //rad2 front
            vertices[triangle+(cylinder * numSidesPerCylinder*6)+(side*6)+5] = (vec4) {(width+rad1X)*cos(rotationRadians), height1+rad1Y, (width+rad1X)*sin(rotationRadians), 1.0}; //rad1 front
        }
    }

    printf("%d\n", num_vertices-numSidesPerCylinder*3);
    for (triangle = num_vertices-numSidesPerCylinder*3; triangle<num_vertices; triangle+= 3)
    {
        double rad1 = triangle * 360/numSidesPerCylinder * degreesToRadians;
        double rad2 = (triangle + 1) * 360/numSidesPerCylinder * degreesToRadians;

        float rad2X = cylinderRadius * cos(rad1);
        float rad2Y = cylinderRadius * sin(rad1);
        float rad1X = cylinderRadius * cos(rad2);
        float rad1Y = cylinderRadius * sin(rad2);

        vertices[triangle] = (vec4) {(width + 0.0)*cos(degreesOfRotation*degreesToRadians), -1 * height + 0.0, (width + 0.0)*sin(degreesOfRotation*degreesToRadians), 1.0};
        vertices[triangle+1] = (vec4) {(width + rad1X)*cos(degreesOfRotation*degreesToRadians), -1 * height + rad1Y, (width + rad1X)*sin(degreesOfRotation*degreesToRadians), 1.0};
        vertices[triangle+2] = (vec4) {(width + rad2X)*cos(degreesOfRotation*degreesToRadians), -1 * height + rad2Y, (width + rad2X)*sin(degreesOfRotation*degreesToRadians), 1.0};
    }
}

void color(vec4 * colors)
{
    int side;
    for (side = 0; side<num_vertices; side+= 3)
    {
        /*Choose random colors.*/
        float ColorR = rand() % 256 / 255.0;
        float ColorG = rand() % 256 / 255.0;
        float ColorB = rand() % 256 / 255.0;

        /*Make vertical side colors*/
        colors[side] = (vec4) {ColorR,ColorG,ColorB,1.0};
        colors[side+1] = (vec4) {ColorR,ColorG,ColorB,1.0};
        colors[side+2] = (vec4) {ColorR,ColorG,ColorB,1.0};
    }
}

int mouseDown = 0;

void mouse(int button, int state, int x, int y)
{
    if (button == 3)
    {
        ctm = mat4_mult_mat4(scale_mat4(1.02, 1.02, 1.02), ctm);
    }
    else if (button == 4)
    {
        ctm = mat4_mult_mat4(scale_mat4(1/1.02, 1/1.02, 1/1.02), ctm);
    }
    else if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_UP)
        {
            mouseDown = 0;
        }
        else if (state == GLUT_DOWN)
        {
            mouseDown = 1;
        }
    }

    glutPostRedisplay();
}

void motion(int x, int y)
{
    if (mouseDown == 1)
    {
        printf("x: %d , y:%d\n", x, y);
    }
}

/*Landon Higinbotham's code ends here*/
void init(void)
{
    /*Landon Higinbotham's code starts here*/
    /*Create the vertices array with the appropriate size*/
    vec4 vertices[num_vertices];
    /*Populate vertices array*/
    createSpring(vertices);

    /*Create the colors array with the appropriate size*/
    vec4 colors[num_vertices];
    /*Populate colors array*/
    color(colors);
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


    /*Landon Higinbotham's code starts here*/
    ctm_location = glGetUniformLocation(program, "ctm");
    /*Landon Higinbotham's code ends here*/

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*Landon Higinbotham's code starts here*/
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    /*Landon Higinbotham's code ends here*/
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
    glutCreateWindow("Landon Higinbotham - Lab04");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    /*Landon Higinbotham's code starts here*/
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    /*Landon Higinbotham's code ends here*/
    glutMainLoop();

    return 0;
}
