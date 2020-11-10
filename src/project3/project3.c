
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

//Used to set which ball to follow
int camMode = -1;

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

GLuint eye_position;
GLuint light_position;
GLuint useTexture;
GLuint isShadow;

typedef struct
{
    float radius;
    vec4 center;
    int startIndex;
    int endIndex;
    int textureId;
    mat4 ctm;
    double rotateX;
    double rotateY;
    double rotateZ;
    vec4 lightsource[1];
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

int createBall(vec4 * vertices, vec2 * tex_coords, vec4 * colors, vec4 * normals, ball * ball, settings settings, int vertOffset, int isLight)
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
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                vertices[vertOffset] = (vec4){rowRadius *sin(2* M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                if (isLight == 1)
                {
                    normals[vertOffset-1] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-1]));
                    normals[vertOffset-2] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-2]));
                    normals[vertOffset-3] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-3]));
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

                vertices[vertOffset] = topLeft;
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                vertices[vertOffset] = bottomLeft;
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                vertices[vertOffset] = bottomRight;
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;

                vertices[vertOffset] = topLeft;
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                vertices[vertOffset] = bottomRight;
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                vertices[vertOffset] = topRight;
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                if (isLight == 1)
                {
                    normals[vertOffset-1] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-1]));
                    normals[vertOffset-2] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-2]));
                    normals[vertOffset-3] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-3]));
                    normals[vertOffset-4] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-4]));
                    normals[vertOffset-5] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-5]));
                    normals[vertOffset-6] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-6]));
                }
            }
            
            if (i == settings.ballRows-1)
            {
                vertices[vertOffset] = ball->center;
                vertices[vertOffset].y -= ball->radius;
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                
                vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * (j+1)/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * (j+1)/settings.ballCols) + ball->center.z, 1.0};
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                vertices[vertOffset] = (vec4){rowRadius *sin(2*M_PI * j/settings.ballCols) + ball->center.x,y, rowRadius *cos(2*M_PI * j/settings.ballCols) + ball->center.z, 1.0};
                normals[vertOffset] = normalize_v4(v4_sub_v4(vertices[vertOffset], ball->center));
                vertOffset++;
                if (isLight == 1)
                {
                    normals[vertOffset-1] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-1]));
                    normals[vertOffset-2] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-2]));
                    normals[vertOffset-3] = normalize_v4(v4_sub_v4(ball->center, vertices[vertOffset-3]));
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
            colors[i] = (vec4){1.0,1.0,1.0,1.0};
        }
    }
    return vertOffset;
}

int num_vertices = 0;
ball poolBalls[16];
ball light;
int tableVertices = 0;
int verticesPerBall = 0;

int addPoolBalls(vec4 * vertices, vec2 * tex_coords, vec4 * colors, vec4 * normals, settings Settings,int vertOffset)
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
        vertOffset = createBall(vertices,tex_coords, colors, normals, &poolBalls[i], Settings, vertOffset, 0);
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
    vec4 normals[num_vertices];
    vec2 tex_coords[num_vertices];

    int vertOffset = 0;

    vertOffset += createTable(vertices,colors, Settings, vertOffset);

    vertOffset = addPoolBalls(vertices,tex_coords, colors, normals, Settings, vertOffset);

    light.center = (vec4){0.0,0.1,0.0,1.0};
    light.radius = .05;
    light.ctm = translate_mat4(0,.9,0);
    vertOffset = createBall(vertices, tex_coords, colors, normals, &light, Settings, vertOffset, 1);

    lookEye = startEye;
    lookAt = startAt;
    lookUp = startUp;
    model_view = look_at(lookEye,lookAt,lookUp);
    projection = frustum(startFrustum.left, startFrustum.right, startFrustum.bottom, startFrustum.top, startFrustum.near, startFrustum.far);

    int i=0;
    for (i=0; i<Settings.tableColumns * Settings.tableRows * 6; i++)
    {
        vec4 vert1 = vertices[i];
        i++;
        vec4 vert2 = vertices[i];
        i++;
        vec4 vert3 = vertices[i];

        vec4 vector1 = v4_sub_v4(vert2, vert1);
        vec4 vector2 = v4_sub_v4(vert3, vert2);
        normals[i-2] = normalize_v4(cross_prod_v4(vector1, vector2));
        normals[i-1] = normals[i-2];
        normals[i] = normals[i-1];
    }

    printf("Controls:\n\n");
    printf("Move Camera:\n");
    printf("Up: W\n");
    printf("Down: S\n");
    printf("Left: A\n");
    printf("Right: D\n");
    printf("In: Q\n");
    printf("Out: E\n\n");
    printf("Move Light:\n");
    printf("Up: O\n");
    printf("Down: U\n");
    printf("Forward: I\n");
    printf("Backward: K\n");
    printf("Left: J\n");
    printf("Right: L\n\n");
    printf("Animations:\n");
    printf("Line up: SPACE (Can only be played once)\n");
    printf("Circle around cue: SPACE (After balls lined up)\n\n");
    printf("Camera options:\n");
    printf("Reset: 0\n");
    printf("Balls 1-9: 1-9\n");
    printf("Balls 10: R\n");
    printf("Balls 11: T\n");
    printf("Balls 12: Y\n");
    printf("Balls 13: F\n");
    printf("Balls 14: G\n");
    printf("Balls 15: H\n");
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors) + sizeof(tex_coords) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), sizeof(tex_coords), tex_coords);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors) + sizeof(tex_coords), sizeof(normals), normals);

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
    GLuint shaderNormals = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(shaderNormals);
    glVertexAttribPointer(shaderNormals, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + (sizeof(vertices) + sizeof(colors) + sizeof(tex_coords)));

    ctm_location = glGetUniformLocation(program, "ctm");
    model_view_location = glGetUniformLocation(program, "model_view_matrix");
    projection_location = glGetUniformLocation(program, "projection_matrix");
    light_position = glGetUniformLocation(program, "light_position");
    eye_position = glGetUniformLocation(program, "eye_position");
    useTexture = glGetUniformLocation(program, "use_texture");
    isShadow = glGetUniformLocation(program, "is_shadow");
    glUniform1i(useTexture, 0);
    glUniform1i(isShadow, 0);
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
    light.lightsource[0] = mat4_mult_v4(light.ctm, light.center);
    glUniform4fv(light_position, 1, (GLfloat *) &light.lightsource[0]);
    glUniform4fv(eye_position, 1, (GLfloat *) &lookEye);


    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection);
    //Draw table
    glUniform1i(useTexture, 0);
    glUniform1i(isShadow, 0);
    glDrawArrays(GL_TRIANGLES, 0, tableVertices);
    //Draw pool balls
    int i=0;
    for(i=0;i<16;i++)
    {
        glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &poolBalls[i].ctm);
        glUniform1i(useTexture, 1);
        glUniform1i(isShadow, 0);
        glDrawArrays(GL_TRIANGLES, poolBalls[i].startIndex, poolBalls[i].endIndex-poolBalls[i].startIndex);
        glUniform1i(isShadow, 1);
        glDrawArrays(GL_TRIANGLES, poolBalls[i].startIndex, poolBalls[i].endIndex-poolBalls[i].startIndex);
    }
        glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &light.ctm);
        glUniform1i(isShadow, 0);
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

                    vec4 subGoalOriginal = v4_sub_v4(goal, ballOriginalSpot);
                    
                    vec4 translation = v4_add_v4(scalar_mult_v4(alpha, subGoalOriginal), ballOriginalSpot);
                    mat4 translationMat = translate_mat4(translation.x, translation.y, translation.z);

                    //First we need the X and Z that it is heading
                    /*double direction = atan2(subGoalOriginal.z, subGoalOriginal.x) *-1;
                    printf("%f\n", direction*180/M_PI);
                    double amountY = sin(direction) * mag_v4(subGoalOriginal)*alpha;
                    double amountX = cos(direction) * mag_v4(subGoalOriginal)*alpha;

                    poolBalls[ball].rotateY = (amountY/.1)*180/M_PI;
                    poolBalls[ball].rotateX = (amountX/.1)*180/M_PI;

                    mat4 rotationMat = translate_mat4(0,-.1,0);
                    rotationMat = mat4_mult_mat4(rotateX_mat4(poolBalls[ball].rotateX), rotationMat);
                    rotationMat = mat4_mult_mat4(rotateY_mat4(poolBalls[ball].rotateY), rotationMat);
                    rotationMat = mat4_mult_mat4(translate_mat4(0,.1,0), rotationMat);*/
                    vec4 arbVector = cross_prod_v4((vec4){0,1,0,0}, normalize_v4(subGoalOriginal));
                    float d = sqrt(arbVector.z*arbVector.z + arbVector.y*arbVector.y);
                    mat4 rotationMat = translate_mat4(0,-.1,0);
                    //Align with X-Z plane
                    rotationMat = mat4_mult_mat4(rotateX_mat4_arb(-1*arbVector.y, arbVector.z, d), rotationMat); //Rotate X
                    //Align wih Z axis
                    rotationMat = mat4_mult_mat4(trans_mat4(rotateY_mat4_arb(-1*arbVector.x, d)), rotationMat); //Rotate Y
                    //Rotate Z
                    rotationMat = mat4_mult_mat4(rotateZ_mat4(mag_v4(subGoalOriginal)*alpha/.1*-180/M_PI), rotationMat);
                    //Reverse Y 
                    rotationMat = mat4_mult_mat4(rotateY_mat4_arb(-1*arbVector.x, d), rotationMat); //Reverse Y
                    //Reverse X
                    rotationMat = mat4_mult_mat4(trans_mat4(rotateX_mat4_arb(-1*arbVector.y, arbVector.z, d)), rotationMat); //Reverse X
                    rotationMat = mat4_mult_mat4(translate_mat4(0,.1,0), rotationMat);
                    poolBalls[ball].ctm = mat4_mult_mat4(translationMat, rotationMat);
                }
                else if(ballLineNum == 16)
                {
                    ballLineNum++;
                }
            }
            else
            {
                currentStep = -1;
                isAnimating = 0; //Stop the animation
                currentState = 1;
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
            if (camMode >= 0)
            {
                i = camMode;
                float radius = -3+.2*(i-1);
                float zCurr = sin(M_PI/2 - (2*M_PI)*(currentStep/1500*(i/15.0))) * radius;
                float xCurr = cos(M_PI/2 - (2*M_PI)*(currentStep/1500*(i/15.0))) * radius;
                //Take current position
                vec4 curr = {xCurr, 0.1, zCurr, 1.0};
                //Take center
                vec4 center = {0, 0.1, 0, 1.0};
                vec4 centerOffset = v4_sub_v4(center, curr);
                //Translate curr to the center then find the vector between the two
                vec4 vector = v4_sub_v4(center, centerOffset);
                vector.w = 0;
                vector = normalize_v4(vector);
                //Rotate the vector 90 on the y
                vector = mat4_mult_v4(rotateY_mat4(90), vector);
                vector = normalize_v4(vector);
                //Set position behind the ball
                lookEye = scalar_mult_v4(2, vector);
                //Move the camera back
                lookEye = v4_add_v4(lookEye, curr);
                lookEye.w = 1.0;
                lookEye.y +=.2;
                lookAt = curr;
            }
        }
        glutPostRedisplay();
        currentStep++;
    }
}
/*Landon Higinbotham's code ends here*/


void keyboard(unsigned char key, int mousex, int mousey)
{
    /*Landon Higinbotham's code starts here*/
    if (key == ' ' && currentState == 0) //Animate on space press.
    {
        ballLineNum = 0;
        currentState = 0;
        currentStep = 0;
        tempMax = -1;
        isAnimating = 1;
    }
    if (key == ' ' && currentState == 1 && isAnimating == 0) //Animate on space press.
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
    if (key >= 49 && key <= 57) //Animate on space press.
    {
        camMode = key-48;
        glutPostRedisplay();
    }
    if (key  == 'r') //Animate on space press.
    {
        camMode = 10;
        glutPostRedisplay();
    }
    if (key  == 't') //Animate on space press.
    {
        camMode = 11;
        glutPostRedisplay();
    }
    if (key  == 'y') //Animate on space press.
    {
        camMode = 12;
        glutPostRedisplay();
    }
    if (key  == 'f') //Animate on space press.
    {
        camMode = 13;
        glutPostRedisplay();
    }
    if (key  == 'g') //Animate on space press.
    {
        camMode = 14;
        glutPostRedisplay();
    }
    if (key  == 'h') //Animate on space press.
    {
        camMode = 15;
        glutPostRedisplay();
    }
    if (key == '0')
    {
        camMode = -1;
        lookEye = startEye;
        lookAt = startAt;
        lookUp = startUp;
    }

    //Move light
    if (key == 'i')//Forward
    {
        light.ctm.w.z -= .1;
        glutPostRedisplay();
    }
    if (key == 'k')//Backwards
    {
        light.ctm.w.z += .1;
        glutPostRedisplay();
    }
    if (key == 'u')//Down
    {
        light.ctm.w.y -= .1;
        glutPostRedisplay();
    }
    if (key == 'o')//Up
    {
        light.ctm.w.y += .1;
        glutPostRedisplay();
    }
    if (key == 'j')//Left
    {
        light.ctm.w.x -= .1;
        glutPostRedisplay();
    }
    if (key == 'l')//Right
    {
        light.ctm.w.x += .1;
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
