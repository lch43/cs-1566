
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../libraries/initShader.h"
#include "../libraries/vandmlib.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef struct
{
    GLfloat x;
    GLfloat y;
} vec2;

//Landon Higinbotham's code starts

typedef struct {
    float left; float right; float bottom; float top; float near; float far;
} projectionArgs; //Struct used to help keep args tidy

//Camera starting view model settings.
vec4 startEye = {0.0,2.0,5.0,1.0};
vec4 startAt = {0.0,0.0,0,1.0};
vec4 startUp = {0,1.0,0,0};

//Arguments to create the frustum for the starting view
projectionArgs startFrustum = {-.1, .1, -.1, .1, -.5, -20.0};

//The lookAt that can be modified.
vec4 lookEye;
vec4 lookAt;
vec4 lookUp;

GLuint model_view_location;
mat4 model_view = {
    (vec4) {1,0,0,0},
    (vec4) {0,1,0,0},
    (vec4) {0,0,1,0},
    (vec4) {0,0,0,1},
};
GLuint projection_location;
mat4 projection = {
    (vec4) {1,0,0,0},
    (vec4) {0,1,0,0},
    (vec4) {0,0,1,0},
    (vec4) {0,0,0,1},
};
GLuint ctm_location;
mat4 ctm = {
    (vec4) {1,0,0,0},
    (vec4) {0,1,0,0},
    (vec4) {0,0,1,0},
    (vec4) {0,0,0,1},
};

GLuint useTexture;

typedef struct
{
    float radius;
    vec4 center;
    int startIndex;
    int endIndex;
    int textureId;
    mat4 ctm;
} ball;


typedef struct 
{
    int tableRows;
    int tableColumns;
    int numBalls;
    int ballRows;
    int ballCols;
} settings;

vec4 moveEye(double x, double y, double d)
{
    double angle = 0;
    double distance = sqrt(pow(lookEye.x-lookAt.x,2) + pow(lookEye.y-lookAt.y,2) + pow(lookEye.z-lookAt.z,2));
    vec4 consider = lookEye;
    if (x != 0)
    {
        mat4 rotation = rotateY_mat4(x);
        consider = mat4_mult_v4(rotation, consider);
        consider.w = 1.0;
    }
    if (y != 0)
    {
        double angle = atan2(consider.z, consider.x)*180.0/M_PI;
        mat4 rotation = rotateY_mat4(angle);
        rotation = mat4_mult_mat4(rotation, rotateZ_mat4(y));
        rotation = mat4_mult_mat4(rotation, rotateY_mat4(angle*-1));
        consider = mat4_mult_v4(rotation, consider);
        consider.w = 1.0;
    }
    if (distance+d <.1)
    {
        d = 0;
    }
    vec4 vector = v4_sub_v4(consider, lookAt);
    vector.w = 0;
    vector = normalize_v4(vector);

    return scalar_mult_v4(distance+d, vector);
}

int createTable(vec4 * vertices, vec4 * colors, settings settings, int vertOffset)
{
    int rows = settings.tableRows;
    int cols = settings.tableColumns;

    float start = -5;
    float sizeX = 10/cols;
    float sizeZ = 10/rows;

    int i = 0;
    int j = 0;
    for (i=0; i<rows; i++)
    {
        for (j=0; j<rows; j++)
        {
            vec4 topLeft = (vec4){start+j*sizeX,0.0,start+i*sizeZ,1.0};
            vec4 topRight = (vec4){start+(j+1)*sizeX,0.0,start+i*sizeZ,1.0};
            vec4 bottomLeft = (vec4){start+j*sizeX,0.0,start+(i+1)*sizeZ,1.0};
            vec4 bottomRight = (vec4){start+(j+1)*sizeX,0.0,start+(i+1)*sizeZ,1.0};

            vertices[vertOffset] = topLeft;
            vertices[vertOffset+1] = bottomLeft;
            vertices[vertOffset+2] = bottomRight;
            vertices[vertOffset+3] = topLeft;
            vertices[vertOffset+4] = bottomRight;
            vertices[vertOffset+5] = topRight;
            vertOffset += 6;
        }
    }
    for (i=0; i<vertOffset; i++)
    {
        colors[i] = (vec4){0.0,1.0,0.0,1.0};
    }
    return vertOffset;
}

int createBall(vec4 * vertices, vec2 * tex_coords, vec4 * colors, ball * ball, settings settings, int vertOffset, int isLight)
{
    int startOffset = vertOffset;
    ball->startIndex = vertOffset;

    int i=0;
    int j=0;

    for (i=0; i<settings.ballRows; i++)
    {
        float y = ball->center.y + ball->radius * cos(M_PI*(i+1)/(settings.ballRows+2));
        float rowRadius = ball->radius * sin(M_PI*(i+1)/(settings.ballRows+2));
        if (rowRadius < 0)
        {
            rowRadius = rowRadius * -1;
        }
        for (j=0; j<settings.ballCols; j++)//move down then build up
        {
            if (i==0) //Join to the top point
            {
                vertices[vertOffset] = ball->center;
                vertices[vertOffset].y += ball->radius;
                vertOffset++;
                if (isLight == 0)
                {
                    vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                    vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                }
                else
                {
                    vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                    vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                }
                
            }
            else //Join to the previous row.
            {
                float prevy = ball->center.y + ball->radius * cos(M_PI*(i)/(settings.ballRows+2));
                float prevRadius = ball->radius  *sin(M_PI*(i)/(settings.ballRows+2));
                if (prevRadius < 0)
                {
                    prevRadius = prevRadius * -1;
                }
                vec4 topLeft = (vec4){prevRadius *sin(2*M_PI * j/settings.ballCols) + ball->center.x,prevy, prevRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                vec4 topRight = (vec4){prevRadius *sin(2*M_PI * (j+1)/settings.ballCols) + ball->center.x,prevy, prevRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                vec4 bottomLeft = (vec4){rowRadius *sin(2*M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                vec4 bottomRight = (vec4){rowRadius *sin(2*M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};

                if (isLight == 0)
                {
                    vertices[vertOffset] = topLeft;
                    vertOffset++;
                    vertices[vertOffset] = bottomLeft;
                    vertOffset++;
                    vertices[vertOffset] = bottomRight;
                    vertOffset++;

                    vertices[vertOffset] = topLeft;
                    vertOffset++;
                    vertices[vertOffset] = bottomRight;
                    vertOffset++;
                    vertices[vertOffset] = topRight;
                    vertOffset++;
                }
                else
                {
                    vertices[vertOffset] = topLeft;
                    vertOffset++;
                    vertices[vertOffset] = bottomRight;
                    vertOffset++;
                    vertices[vertOffset] = bottomLeft;
                    vertOffset++;

                    vertices[vertOffset] = topLeft;
                    vertOffset++;
                    vertices[vertOffset] = topRight;
                    vertOffset++;
                    vertices[vertOffset] = bottomRight;
                    vertOffset++;
                }
            }
            
            if (i == settings.ballRows-1)
            {
                vertices[vertOffset] = ball->center;
                vertices[vertOffset].y -= ball->radius;
                vertOffset++;
                if (isLight == 0)
                {
                    vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                    vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                }
                else
                {
                    vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                    vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                    vertOffset++;
                }
            }
        }
    }

    ball->endIndex = vertOffset;

    for(i=startOffset; i<vertOffset;i++)
    {
        vec2 topLeft;
        vec2 bottomRight;
        topLeft = (vec2){0.76, 0.7625}; //To get the compiler to shut up.
        bottomRight = (vec2){0.99, 0.99};  //To get the compiler to shut up.

        if (ball->textureId == 0){topLeft = (vec2){0.76, 0.7625};bottomRight = (vec2){0.99, 0.99};}
        else if (ball->textureId == 1){topLeft = (vec2){0.01, 0.01};bottomRight = (vec2){0.24, 0.24};}
        else if (ball->textureId == 2){topLeft = (vec2){0.26, 0.01};bottomRight = (vec2){0.49, 0.24};}
        else if (ball->textureId == 3){topLeft = (vec2){0.51, 0.01};bottomRight = (vec2){0.74, 0.24};}
        else if (ball->textureId == 4){topLeft = (vec2){0.76, 0.01};bottomRight = (vec2){0.99, 0.24};}
        else if (ball->textureId == 5){topLeft = (vec2){0.01, 0.26};bottomRight = (vec2){0.24, 0.49};}
        else if (ball->textureId == 6){topLeft = (vec2){0.26, 0.26};bottomRight = (vec2){0.49, 0.49};}
        else if (ball->textureId == 7){topLeft = (vec2){0.51, 0.26};bottomRight = (vec2){0.74, 0.49};}
        else if (ball->textureId == 8){topLeft = (vec2){0.76, 0.26};bottomRight = (vec2){0.99, 0.49};}
        else if (ball->textureId == 9){topLeft = (vec2){0.01, 0.511};bottomRight = (vec2){0.24, 0.741};}
        else if (ball->textureId == 10){topLeft = (vec2){0.26, 0.511};bottomRight = (vec2){0.49, 0.741};}
        else if (ball->textureId == 11){topLeft = (vec2){0.51, 0.511};bottomRight = (vec2){0.74, 0.741};}
        else if (ball->textureId == 12){topLeft = (vec2){0.76, 0.511};bottomRight = (vec2){0.99, 0.741};}
        else if (ball->textureId == 13){topLeft = (vec2){0.01, 0.7625};bottomRight = (vec2){0.24, 0.99};}
        else if (ball->textureId == 14){topLeft = (vec2){0.26, 0.7625};bottomRight = (vec2){0.49, 0.99};}
        else if (ball->textureId == 15){topLeft = (vec2){0.51, 0.7625};bottomRight = (vec2){0.74, 0.99};}

        float x = ((vertices[i].x - ball->center.x + ball->radius) / (2*ball->radius));
        float z = ((vertices[i].z - ball->center.z + ball->radius) / (2*ball->radius));
        tex_coords[i] = (vec2){x * (bottomRight.x-topLeft.x) + topLeft.x, z * (bottomRight.y-topLeft.y) + topLeft.y};
        if (isLight == 1)
        {
            colors[i] = (vec4){1,1,1,1};
        }
    }
    return vertOffset;
}

int num_vertices = 0;
ball poolBalls[16];
ball light;
int tableVertices = 0;
int verticesPerBall = 0;

int addPoolBalls(vec4 * vertices, vec2 * tex_coords, vec4 * colors, settings Settings,int vertOffset)
{
    int i=0;
    for (i=0;i<16;i++)
    {
        poolBalls[i].radius = .1;
        poolBalls[i].textureId = i;
        if (i==0)
        {
            poolBalls[i].ctm = translate_mat4(.1*3, 0, .1*3);
            poolBalls[i].center = (vec4){0, 0.1, 0, 1.0};
        }
        else
        {
            int col = (i-1)%4;
            int row = (i-col)/4;
            poolBalls[i].ctm = translate_mat4(.1*(-3 + 2*col), 0, .1*(-3 + 2*row));
            poolBalls[i].center = (vec4){0, 0.1, 0, 1.0};
        }
        vertOffset = createBall(vertices,tex_coords, colors, &poolBalls[i], Settings, vertOffset, 0);
    }

    return vertOffset;
}

//Landon Higinbotham's code ends

void init(void)
{
    //Landon Higinbotham's code starts

    settings Settings;
    Settings.tableColumns = 5;
    Settings.tableRows = 5;
    Settings.ballCols = 10;
    Settings.ballRows = 10;
    Settings.numBalls = 16;

    tableVertices = Settings.tableColumns * Settings.tableRows * 6;
    verticesPerBall = (2 * Settings.ballCols*3 + Settings.ballRows*(Settings.ballCols-1)*6);

    num_vertices = tableVertices + verticesPerBall * Settings.numBalls + verticesPerBall;
    vec4 vertices[num_vertices];
    vec4 colors[num_vertices];
    vec2 tex_coords[num_vertices];

    int vertOffset = 0;

    vertOffset += createTable(vertices,colors, Settings, vertOffset);

    vertOffset = addPoolBalls(vertices,tex_coords, colors, Settings, vertOffset);

    light.center = (vec4){0.0,0.1,0.0,1.0};
    light.radius = .05;
    light.ctm = translate_mat4(0,.9,0);
    vertOffset = createBall(vertices, tex_coords, colors, &light, Settings, vertOffset, 1);

    lookEye = startEye;
    lookAt = startAt;
    lookUp = startUp;
    model_view = look_at(lookEye,lookAt,lookUp);
    projection = frustum(startFrustum.left, startFrustum.right, startFrustum.bottom, startFrustum.top, startFrustum.near, startFrustum.far);
    //Landon Higinbotham's code ends

    int width = 512;
    int height = 512;
    GLubyte my_texels[width][height][3];

    FILE *fp = fopen("pb_512_512.raw", "r");
    fread(my_texels, width * height * 3, 1, fp);
    fclose(fp);

    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint mytex[1];
    glGenTextures(1, mytex);
    glBindTexture(GL_TEXTURE_2D, mytex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, my_texels);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    int param;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), sizeof(tex_coords), tex_coords);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + sizeof(vertices));

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + (sizeof(vertices) + sizeof(colors)));

    GLuint texture_location = glGetUniformLocation(program, "texture");
    glUniform1i(texture_location, 0);

    //printf("texture_location: %i\n", texture_location);

    /*Landon Higinbotham's code starts here*/
    ctm_location = glGetUniformLocation(program, "ctm");
    model_view_location = glGetUniformLocation(program, "model_view_matrix");
    projection_location = glGetUniformLocation(program, "projection_matrix");
    useTexture = glGetUniformLocation(program, "use_texture");
    glUniform1i(useTexture, 0);
    /*Landon Higinbotham's code ends here*/

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /*Landon Higinbotham's code starts here*/
    model_view = look_at(lookEye, lookAt, lookUp);
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection);
    //Draw table
    glUniform1i(useTexture, 0);
    glDrawArrays(GL_TRIANGLES, 0, tableVertices);
    //Draw pool balls
    int i=0;
    for(i=0;i<16;i++)
    {
        glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &poolBalls[i].ctm);
        glUniform1i(useTexture, 1);
        glDrawArrays(GL_TRIANGLES, poolBalls[i].startIndex, poolBalls[i].endIndex-poolBalls[i].startIndex);
    }
        glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &light.ctm);
        glUniform1i(useTexture, 0);
        glDrawArrays(GL_TRIANGLES, light.startIndex, light.endIndex-light.startIndex);
    /*Landon Higinbotham's code ends here*/
    glutSwapBuffers();
}

/*Landon Higinbotham's code starts here*/
typedef enum
{
    LINE_UP = 0,
    ORBIT,
} state;


int isAnimating = 0;
float currentStep = 0; //What step of animation are we on
int ballLineNum = 0;
vec4 ballOriginalSpot;
int tempMax = -1;
state currentState = 0; //What animation is going on

void idle(void)
{
    if (isAnimating)
    {

        //Go through and check what state.

        if (currentState == 0) //LINE_UP
        {
            if (ballLineNum <= 16)
            {
                //Use alpha to transition the eye and up vector from one to another
                int ball = ballLineNum%16;
                if (currentStep>tempMax && ballLineNum < 16)
                {
                    currentStep = 0;
                    ballLineNum++;
                    ball = ballLineNum%16;
                    ballOriginalSpot = (vec4){poolBalls[ball].ctm.w.x,poolBalls[ball].ctm.w.y,poolBalls[ball].ctm.w.z,1.0};
                    if (ball != 0)
                    {
                        tempMax = (sqrt(pow(0-poolBalls[ball].ctm.w.x, 2) +
                        pow((-3.0+((ball-1)*.2))-poolBalls[ball].ctm.w.z,2))) * 200;//For each unit take up 200 frames.
                    }
                    else if (ball == 0)
                    {
                        tempMax = (sqrt(pow(0-poolBalls[ball].ctm.w.x, 2) +
                        pow(0-poolBalls[ball].ctm.w.z,2))) * 200;//For each unit take up 200 frames.
                    }
                    if (tempMax == 0)
                    {
                        tempMax = -1;
                    }
                }
                if (currentStep<=tempMax)
                {
                    float alpha = (currentStep*1.00/tempMax);
                    vec4 goal;
                    if (ball != 0)
                    {
                        goal = (vec4){0.0,0,(-3.0+((ball-1)*.2)),1.0};
                    }
                    else
                    {
                        goal = (vec4){0.0,0,0.0,1.0};
                    }
                    
                    vec4 translation = v4_add_v4(scalar_mult_v4(alpha, v4_sub_v4(goal, ballOriginalSpot)), ballOriginalSpot);
                    poolBalls[ball].ctm = translate_mat4(translation.x, translation.y, translation.z);
                }
            }
            else
            {
                currentStep = -1;
                isAnimating = 0; //Stop the animation
            }
        }
        else if (currentState == 1) //Orbit
        {
            int i=15;
            if (currentStep == 15*1500) {currentStep = 0;};
            for (i=15;i>0;i--)
            {
                float radius = -3+.2*(i-1);
                float z = sin(M_PI/2 - (2*M_PI)*(currentStep/1500*(i/15.0))) * radius;
                float x = cos(M_PI/2 - (2*M_PI)*(currentStep/1500*(i/15.0))) * radius;
                poolBalls[i].ctm = translate_mat4(x, 0, z);
            }
            poolBalls[0].ctm = translate_mat4(0, 0, 0);
        }
        glutPostRedisplay();
        currentStep++;
    }
}
/*Landon Higinbotham's code ends here*/


void keyboard(unsigned char key, int mousex, int mousey)
{
    /*Landon Higinbotham's code starts here*/
    if (key == ' ') //Animate on space press.
    {
        ballLineNum = 0;
        currentState = 0;
        currentStep = 0;
        tempMax = -1;
        isAnimating = 1;
    }

    if (key == 'o') //Animate on space press.
    {
        currentState = 1;
        currentStep = 0;
        isAnimating = 1;
    }

    if (key == 'w') //Animate on space press.
    {
        lookEye = moveEye(0,-5,0);
        glutPostRedisplay();
    }

    if (key == 's') //Animate on space press.
    {
        lookEye = moveEye(0,5,0);
        glutPostRedisplay();
    }

    if (key == 'a') //Animate on space press.
    {
        lookEye = moveEye(5,0,0);
        glutPostRedisplay();
    }

    if (key == 'd') //Animate on space press.
    {
        lookEye = moveEye(-5,0,0);
        glutPostRedisplay();
    }

    if (key == 'e') //Animate on space press.
    {
        lookEye = moveEye(0,0,-.05);
        glutPostRedisplay();
    }

    if (key == 'q') //Animate on space press.
    {
        lookEye = moveEye(0,0,.05);
        glutPostRedisplay();
    }
    /*Landon Higinbotham's code ends here*/

    if(key == ']')
	exit(0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Landon Higinbotham - Project 3");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
