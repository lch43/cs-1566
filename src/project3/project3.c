
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
vec4 startEye = {0.0,2,0.0,1.0};
vec4 startAt = {0.0,-.5,0,1.0};
vec4 startUp = {0,0,-1,0};

//Arguments to create the frustum for outside and inside the maze.
projectionArgs frustumOutside = {-1.0, 1.0, -1.0, 1.0, -1, -15.0};
projectionArgs frustumInside = {-.2, .2, -.2, .2, -0.1, -15.0};

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

int createBall(vec4 * vertices, vec2 * tex_coords, ball * ball, settings settings, int vertOffset)
{

    //ball->ballVariable = Blank
    //to call you do method(&ball)
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
                vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                vertOffset++;
                vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                vertOffset++;
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
            
            if (i == settings.ballRows-1)
            {
                vertices[vertOffset] = ball->center;
                vertices[vertOffset].y -= ball->radius;
                vertOffset++;
                vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                vertOffset++;
                vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                vertOffset++;
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

        if (ball->textureId == 0)
        {
            topLeft = (vec2){0.76, 0.7625};
            bottomRight = (vec2){0.99, 0.99};
        }
        else if (ball->textureId == 1)
        {
            topLeft = (vec2){0.01, 0.01};
            bottomRight = (vec2){0.24, 0.24};
        }
        else if (ball->textureId == 2)
        {
            topLeft = (vec2){0.26, 0.01};
            bottomRight = (vec2){0.49, 0.24};
        }
        else if (ball->textureId == 3)
        {
            topLeft = (vec2){0.51, 0.01};
            bottomRight = (vec2){0.74, 0.24};
        }
        else if (ball->textureId == 4)
        {
            topLeft = (vec2){0.76, 0.01};
            bottomRight = (vec2){0.99, 0.24};
        }
        else if (ball->textureId == 5)
        {
            topLeft = (vec2){0.01, 0.26};
            bottomRight = (vec2){0.24, 0.49};
        }
        else if (ball->textureId == 6)
        {
            topLeft = (vec2){0.26, 0.26};
            bottomRight = (vec2){0.49, 0.49};
        }
        else if (ball->textureId == 7)
        {
            topLeft = (vec2){0.51, 0.26};
            bottomRight = (vec2){0.74, 0.49};
        }
        else if (ball->textureId == 8)
        {
            topLeft = (vec2){0.76, 0.26};
            bottomRight = (vec2){0.99, 0.49};
        }
        else if (ball->textureId == 9)
        {
            topLeft = (vec2){0.01, 0.511};
            bottomRight = (vec2){0.24, 0.741};
        }
        else if (ball->textureId == 10)
        {
            topLeft = (vec2){0.26, 0.511};
            bottomRight = (vec2){0.49, 0.741};
        }
        else if (ball->textureId == 11)
        {
            topLeft = (vec2){0.51, 0.511};
            bottomRight = (vec2){0.74, 0.741};
        }
        else if (ball->textureId == 12)
        {
            topLeft = (vec2){0.76, 0.511};
            bottomRight = (vec2){0.99, 0.741};
        }
        else if (ball->textureId == 13)
        {
            topLeft = (vec2){0.01, 0.7625};
            bottomRight = (vec2){0.24, 0.99};
        }
        else if (ball->textureId == 14)
        {
            topLeft = (vec2){0.26, 0.7625};
            bottomRight = (vec2){0.49, 0.99};
        }
        else if (ball->textureId == 15)
        {
            topLeft = (vec2){0.51, 0.7625};
            bottomRight = (vec2){0.74, 0.99};
        }

        float x = ((vertices[i].x - ball->center.x + ball->radius) / (2*ball->radius));
        float z = ((vertices[i].z - ball->center.z + ball->radius) / (2*ball->radius));
        tex_coords[i] = (vec2){x * (bottomRight.x-topLeft.x) + topLeft.x, z * (bottomRight.y-topLeft.y) + topLeft.y};
    }

    return vertOffset;
}

int num_vertices = 0;
ball poolBalls[16];
int tableVertices = 0;
int verticesPerBall = 0;

int addPoolBalls(vec4 * vertices, vec2 * tex_coords,settings Settings,int vertOffset)
{
    int i=0;
    for (i=0;i<16;i++)
    {
        poolBalls[i].radius = .1;
        poolBalls[i].ctm = identity();
        poolBalls[i].textureId = i;
        if (i==0)
        {
            poolBalls[i].center = (vec4){.1*3, 0.1, .1*3, 1.0};
        }
        else
        {
            int col = (i-1)%4;
            int row = (i-col)/4;
            poolBalls[i].center = (vec4){.1*(-3 + 2*col), 0.1, .1*(-3 + 2*row), 1.0};
        }
        vertOffset = createBall(vertices,tex_coords,&poolBalls[i], Settings, vertOffset);
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
    verticesPerBall = (2 * Settings.ballCols*3 + Settings.ballRows*Settings.ballCols*6);

    num_vertices = tableVertices + verticesPerBall * Settings.numBalls;
    vec4 vertices[num_vertices];
    vec4 colors[num_vertices];
    vec2 tex_coords[num_vertices];

    int vertOffset = 0;

    vertOffset += createTable(vertices,colors, Settings, vertOffset);

    vertOffset = addPoolBalls(vertices,tex_coords,Settings, vertOffset);

    model_view = look_at(startEye,startAt,startUp);
    projection = frustum(frustumOutside.left, frustumOutside.right, frustumOutside.bottom, frustumOutside.top, frustumOutside.near, frustumOutside.far);
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
    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection);
    //Draw table
    glUniform1i(useTexture, 0);
    glDrawArrays(GL_TRIANGLES, 0, tableVertices);
    //Draw cue ball
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[0].startIndex, poolBalls[0].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[1].startIndex, poolBalls[1].endIndex);
    //Draw ball 2
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[2].startIndex, poolBalls[2].endIndex);
    //Draw ball 3
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[3].startIndex, poolBalls[3].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[4].startIndex, poolBalls[4].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[5].startIndex, poolBalls[5].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[6].startIndex, poolBalls[6].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[7].startIndex, poolBalls[7].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[8].startIndex, poolBalls[8].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[9].startIndex, poolBalls[9].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[10].startIndex, poolBalls[10].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[11].startIndex, poolBalls[11].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[12].startIndex, poolBalls[12].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[13].startIndex, poolBalls[13].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[14].startIndex, poolBalls[14].endIndex);
    //Draw ball 1
    glUniform1i(useTexture, 1);
    glDrawArrays(GL_TRIANGLES, poolBalls[15].startIndex, poolBalls[15].endIndex);
    /*Landon Higinbotham's code ends here*/
    glutSwapBuffers();
}

/*Landon Higinbotham's code starts here*/
typedef enum
{
    FLYING_TO_CIRCLE = 0,
    FLYING_AROUND,
    FLYING_DOWN,
    WALK_FORWARD,
    TURN_LEFT,
    TURN_RIGHT,
} state;


int isAnimating = 0;
int currentStep = 0; //What step of animation are we on
state currentState = 1; //What animation is going on

vec4 refEye = {};
vec4 refAt = {};
vec4 refUp = {};

vec4 currEye = {};
vec4 currAt = {};
vec4 currUp = {};

void idle(void)
{
    if (isAnimating)
    {
        currentStep++;

        //Go through and check what state.

        if (currentState == 0) //FLYING_TO_CIRCLE
        {
            float maxStep = 800.0;
            if (currentStep <= maxStep)
            {
                //Use alpha to transition the eye and up vector from one to another
                float alpha = (currentStep/maxStep);
                vec4 moveEye = v4_add_v4(scalar_mult_v4(alpha, v4_sub_v4((vec4){-5.0,startEye.y,-5.0,1.0}, startEye)), startEye);
                vec4 moveUp = v4_add_v4(scalar_mult_v4(alpha, v4_sub_v4((vec4){0.0,1.0,0.0,0.0}, startUp)), startUp);
                moveEye.w = 1.0;
                moveUp.w = 0.0;
                model_view = look_at(moveEye, startAt, moveUp);
                projection = frustum(frustumOutside.left, frustumOutside.right, frustumOutside.bottom, frustumOutside.top, frustumOutside.near, frustumOutside.far);
                currEye = moveEye;
                currAt = startAt;
                currUp = moveUp;
                if (currentStep == maxStep)
                {
                    refEye = currEye;
                    refAt = currAt;
                    refUp = currAt;
                }
            }
            else
            {
                currentStep = 0;
                currentState = 1; //Go to fly around animation
            }
        }
        glutPostRedisplay();
    }
}
/*Landon Higinbotham's code ends here*/


void keyboard(unsigned char key, int mousex, int mousey)
{
    /*Landon Higinbotham's code starts here*/
    if (key == ' ') //Animate on space press.
    {
        currentState = 0;
        currentStep = 0;
        isAnimating = 1;
    }
    /*Landon Higinbotham's code ends here*/

    if(key == 'q')
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
