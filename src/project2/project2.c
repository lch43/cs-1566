
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../libraries/initShader.h"
#include "../libraries/vandmlib.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef struct
{
    GLfloat x;
    GLfloat y;
} vec2;

//Landon Higinbotham's code starts

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

typedef struct
{
    int n;
    int e;
    int s;
    int w;
    int visited;
} mazeBlock;

#define mazeRows 8
#define mazeCols 8

typedef struct
{
    int walls;
    int poles;
    int wallArray[2* mazeCols* mazeRows+ mazeCols + mazeRows];
    int poleArray[(mazeCols+1)*(mazeRows+1)];
} mazeInfoBlock;

typedef struct
{
    float inset;
    float cellSideSize; 
    float wallThick;
    float wallHeight;
    float startPoint;
} mazeSettings;

typedef struct
{
    vec4 topBackLeft;
    vec4 topBackRight;
    vec4 topFrontLeft;
    vec4 topFrontRight;

    vec4 bottomBackLeft;
    vec4 bottomBackRight;
    vec4 bottomFrontLeft;
    vec4 bottomFrontRight;
} cube;

mazeBlock maze[8][8];
int solution[64];

void recursiveMazeBuilder(int row, int col)
{
    int i = 0;
    int directions[4] = {0,1,2,3};

    if (row == mazeRows-1 && col == mazeCols-1)//don't go further
    {
        return;
    }

    for (i=0; i<4; i++)
    {
        int randomInt = rand() % (4-i);
        int choice = directions[randomInt];
        if (choice == 0 && maze[row][col].n == 0 && row != 0 && maze[row-1][col].visited == 0) //Go up
        {
            maze[row][col].n = 1;
            maze[row-1][col].s = 1;
            maze[row-1][col].visited = 1;
            recursiveMazeBuilder(row-1, col);
        }
        else if (choice == 1 && maze[row][col].e == 0 && col != mazeCols-1 && maze[row][col+1].visited == 0) //Go right
        {
            maze[row][col].e = 1;
            maze[row][col+1].w = 1;
            maze[row][col+1].visited = 1;
            recursiveMazeBuilder(row, col+1);
        }
        else if (choice == 2 && maze[row][col].s == 0 && row != mazeRows-1 && maze[row+1][col].visited == 0)//Go down
        {
            maze[row][col].s = 1;
            maze[row+1][col].n = 1;
            maze[row+1][col].visited = 1;
            recursiveMazeBuilder(row+1, col);
        }
        else if (choice == 3 && maze[row][col].w == 0 && col != 0 && maze[row][col-1].visited == 0)//Go left
        {
            maze[row][col].w = 1;
            maze[row][col-1].e = 1;
            maze[row][col-1].visited = 1;
            recursiveMazeBuilder(row, col-1);
        }

        for (;randomInt<(4-i-1);randomInt++) //Shift array downwards.
        {
            directions[randomInt] = directions[randomInt+1];
        }
    }
}

int solveMazeCWRF(int row, int col, int solutionRow, int solutionCol, int index) //Solve clockwise right first.
{
    //printf("[ %d , %d ] \n", row, col);
    solution[index] = row * 8 + col;
    maze[row][col].visited = 0;
    int foundEnd = 0;

    if (row == solutionRow && col == solutionCol) //Found end
    {
        return 1;
    }

    if (maze[row][col].e == 1 && maze[row][col+1].visited == 1) //check to see if visited is 1 (we haven't yet set it back to 0 after generating which means we haven't visited)
    {
        foundEnd = solveMazeCWRF(row, col+1, solutionRow, solutionCol, index+1); //go in direction of opening.
    }

    if (maze[row][col].s == 1 && maze[row+1][col].visited == 1 && foundEnd == 0) //check to see if visited is 1 (we haven't yet set it back to 0 after generating which means we haven't visited)
    {
        foundEnd = solveMazeCWRF(row+1, col, solutionRow, solutionCol, index+1); //go in direction of opening.
    }

    if (maze[row][col].w == 1 && maze[row][col-1].visited == 1 && !(row == 0 && col == 0) && foundEnd == 0) //check to see if visited is 1 (we haven't yet set it back to 0 after generating which means we haven't visited)
    {
        foundEnd = solveMazeCWRF(row, col-1, solutionRow, solutionCol, index+1); //go in direction of opening.
    }

    if (maze[row][col].n == 1 && maze[row-1][col].visited == 1 && foundEnd == 0) //check to see if visited is 1 (we haven't yet set it back to 0 after generating which means we haven't visited)
    {
        foundEnd = solveMazeCWRF(row-1, col, solutionRow, solutionCol, index+1); //go in direction of opening.
    }

    return foundEnd;
}

mazeInfoBlock getMazeInfo()
{
    mazeInfoBlock block = {0,0};

    int i=0;
    int j=0;
    for (i=0; i<mazeRows; i++)
    {
        for (j=0; j<mazeCols; j++)
        {
            if (maze[i][j].n == 0)
            {
                block.wallArray[i*mazeCols+j] = 1;

                block.poleArray[(mazeCols+1)*i + (j*2-j)] = 1;
                block.poleArray[(mazeCols+1)*i + (j*2-j)+1] = 1;
            }
            if (maze[i][j].s == 0)
            {
                block.wallArray[(i+1)*mazeCols+j] = 1;

                block.poleArray[(mazeCols+1)*(i+1) + (j*2-j)] = 1;
                block.poleArray[(mazeCols+1)*(i+1) + (j*2-j)+1] = 1;
            }
            if (maze[i][j].e == 0)
            {
                block.wallArray[(mazeRows+1)*mazeCols+ (j*2-j)+(mazeCols+1)*i+1] = 1;

                block.poleArray[(mazeCols+1)*i + (j*2-j)+1] = 1;
                block.poleArray[(mazeCols+1)*(i+1) + (j*2-j)+1] = 1;
            }
            if (maze[i][j].w == 0)
            {
                block.wallArray[(mazeRows+1)*mazeCols+ (j*2-j)+(mazeCols+1)*i] = 1;

                block.poleArray[(mazeCols+1)*i + (j*2-j)] = 1;
                block.poleArray[(mazeCols+1)*(i+1) + (j*2-j)] = 1;
            }
        }
    } 
    for (i = 0; i<(2* mazeCols* mazeRows+ mazeCols + mazeRows); i++)
    {
        if (block.wallArray[i] == 1)
        {
            block.walls++;
        }
    }
    for (i = 0; i<((mazeCols+1)*(mazeRows+1)); i++)
    {
        if (block.poleArray[i] == 1)
        {
            block.poles++;
        }
    }

    return block;
}

void createBox(vec4 * vertices, cube cube, int offset)
{
    vec4 topBackLeft = cube.topBackLeft;
    vec4 topBackRight = cube.topBackRight;
    vec4 topFrontLeft = cube.topFrontLeft;
    vec4 topFrontRight = cube.topFrontRight;

    vec4 bottomBackLeft = cube.bottomBackLeft;
    vec4 bottomBackRight = cube.bottomBackRight;
    vec4 bottomFrontLeft = cube.bottomFrontLeft;
    vec4 bottomFrontRight = cube.bottomFrontRight;

    //Front face
    vertices[offset+0] = topFrontRight;
    vertices[offset+1] = topFrontLeft;
    vertices[offset+2] = bottomFrontLeft;

    vertices[offset+3] = topFrontRight;
    vertices[offset+4] = bottomFrontLeft;
    vertices[offset+5] = bottomFrontRight;

    //Back face
    vertices[offset+6] = topBackLeft;
    vertices[offset+7] = topBackRight;
    vertices[offset+8] = bottomBackRight;

    vertices[offset+9] = topBackLeft;
    vertices[offset+10] = bottomBackRight;
    vertices[offset+11] = bottomBackLeft;

    //Left face
    vertices[offset+12] = topFrontLeft;
    vertices[offset+13] = topBackLeft;
    vertices[offset+14] = bottomBackLeft;

    vertices[offset+15] = topFrontLeft;
    vertices[offset+16] = bottomBackLeft;
    vertices[offset+17] = bottomFrontLeft;

    //Right face
    vertices[offset+18] = topBackRight;
    vertices[offset+19] = topFrontRight;
    vertices[offset+20] = bottomFrontRight;

    vertices[offset+21] = topBackRight;
    vertices[offset+22] = bottomFrontRight;
    vertices[offset+23] = bottomBackRight;

    //Top face
    vertices[offset+24] = topBackRight;
    vertices[offset+25] = topBackLeft;
    vertices[offset+26] = topFrontLeft;

    vertices[offset+27] = topBackRight;
    vertices[offset+28] = topFrontLeft;
    vertices[offset+29] = topFrontRight;

    //Bottom face
    vertices[offset+30] = bottomFrontRight;
    vertices[offset+31] = bottomFrontLeft;
    vertices[offset+32] = bottomBackLeft;

    vertices[offset+33] = bottomFrontRight;
    vertices[offset+34] = bottomBackLeft;
    vertices[offset+35] = bottomBackRight;
}

void texture(vec2 * tex_coords, int startIndex, int endIndex, int texture)
{
    int i=0;
    vec2 tex0;
    vec2 tex1;
    vec2 tex2;
    vec2 tex3;
    vec2 tex4;
    vec2 tex5;

    if (texture == 0) //Grass
    {
        tex0 = (vec2){.5,0.5};
        tex1 = (vec2){0.0,0.5};
        tex2 = (vec2){0.0,1};
        tex3 = (vec2){.5,0.5};
        tex4 = (vec2){0.0,1};
        tex5 = (vec2){0.5,1};
    }
    else if (texture == 1) //Bricks
    {
        tex0 = (vec2){.5,0.0};
        tex1 = (vec2){0.0,0.0};
        tex2 = (vec2){0.0,0.5};
        tex3 = (vec2){.5,0.0};
        tex4 = (vec2){0.0,0.5};
        tex5 = (vec2){0.5,0.5};
    }
    else if (texture == 2) //Stones
    {
        tex0 = (vec2){1,0.0};
        tex1 = (vec2){0.5,0.0};
        tex2 = (vec2){0.5,0.5};
        tex3 = (vec2){1,0.0};
        tex4 = (vec2){0.5,0.5};
        tex5 = (vec2){1,0.5};
    }

    for (i=startIndex; i<endIndex; i++)
    {
        if ((i-startIndex)%2 == 0)
        {
            tex_coords[i] = tex0;
            i++;
            tex_coords[i] = tex1;
            i++;
            tex_coords[i] = tex2;
        }
        else
        {
            tex_coords[i] = tex3;
            i++;
            tex_coords[i] = tex4;
            i++;
            tex_coords[i] = tex5;
        }
    }
}

void createWalls(vec4 * vertices, mazeInfoBlock info, int vertOffset, float yOffset, mazeSettings settings)
{
    //Maze will be given a .1 inset from all sides of the plane.
    float cellSideSize = settings.cellSideSize; 
    float wallThick = settings.wallThick;
    float wallHeight = settings.wallHeight;
    float startPoint = settings.startPoint;
    int i=0;
    int wall=0;
    for (i=0;i<2* mazeCols* mazeRows+ mazeCols + mazeRows && wall<info.walls;i++)
    {
        int foundWall = 0;
        cube cube;
        
        if (i<(mazeRows+1)*mazeCols) //Wall is horizontal
        {
            if (info.wallArray[i] == 1)
            {
                foundWall = 1;
                int wallCol = i%mazeCols;
                int wallRow = (i-wallCol)/(mazeCols);
                
                cube.topBackLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                cube.topBackRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                cube.topFrontLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                cube.topFrontRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                
                cube.bottomBackLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                cube.bottomBackRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                cube.bottomFrontLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                cube.bottomFrontRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                
            }
        }
        else //Wall is vertical
        {
            if (info.wallArray[i] == 1)
            {
                foundWall = 1;
                int wallCol = (i-((mazeRows+1)*mazeCols))%(mazeCols+1);
                int wallRow = ((i-((mazeRows+1)*mazeCols))-wallCol)/(mazeCols+1);

                cube.topBackLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset+wallHeight,startPoint+wallRow*cellSideSize,1.0};
                cube.topBackRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset+wallHeight,startPoint+wallRow*cellSideSize,1.0};
                cube.topFrontLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset+wallHeight,startPoint+(wallRow+1)*cellSideSize,1.0};
                cube.topFrontRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset+wallHeight,startPoint+(wallRow+1)*cellSideSize,1.0};

                cube.bottomBackLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset,startPoint+wallRow*cellSideSize,1.0};
                cube.bottomBackRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset,startPoint+wallRow*cellSideSize,1.0};
                cube.bottomFrontLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset,startPoint+(wallRow+1)*cellSideSize,1.0};
                cube.bottomFrontRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset,startPoint+(wallRow+1)*cellSideSize,1.0};

            }
        }

        if (foundWall == 1)
        {
            createBox(vertices, cube, vertOffset+wall*36);
            wall++;
        }
    }
}

void createPoles(vec4 * vertices, mazeInfoBlock info, int vertOffset, float yOffset, mazeSettings settings)
{
    int poles = 0;
    int i=0;
    for (i=0; i<((mazeCols+1)*(mazeRows+1)) && poles < info.poles; i++)
    {
        cube cube;
        if (info.poleArray[i] == 1)
        {
            int col = i%(mazeCols+1);
            int row = (i-col)/(mazeCols+1);

            cube.topBackLeft = (vec4){(settings.startPoint + col*settings.cellSideSize) - .6*settings.wallThick,yOffset+settings.wallHeight+ .1 *settings.wallThick,(settings.startPoint + row*settings.cellSideSize) - .6*settings.wallThick,1.0};
            cube.topBackRight = (vec4){(settings.startPoint + col*settings.cellSideSize) + .6*settings.wallThick,yOffset+settings.wallHeight+ .1 *settings.wallThick,(settings.startPoint + row*settings.cellSideSize) - .6*settings.wallThick,1.0};
            cube.topFrontLeft = (vec4){(settings.startPoint + col*settings.cellSideSize) - .6*settings.wallThick,yOffset+settings.wallHeight+ .1 *settings.wallThick,(settings.startPoint + row*settings.cellSideSize) + .6*settings.wallThick,1.0};
            cube.topFrontRight = (vec4){(settings.startPoint + col*settings.cellSideSize) + .6*settings.wallThick,yOffset+settings.wallHeight+ .1 *settings.wallThick,(settings.startPoint + row*settings.cellSideSize) + .6*settings.wallThick,1.0};

            
            cube.bottomBackLeft = (vec4){(settings.startPoint + col*settings.cellSideSize) - .6*settings.wallThick,yOffset,(settings.startPoint + row*settings.cellSideSize) - .6*settings.wallThick,1.0};
            cube.bottomBackRight = (vec4){(settings.startPoint + col*settings.cellSideSize) + .6*settings.wallThick,yOffset,(settings.startPoint + row*settings.cellSideSize) - .6*settings.wallThick,1.0};
            cube.bottomFrontLeft = (vec4){(settings.startPoint + col*settings.cellSideSize) - .6*settings.wallThick,yOffset,(settings.startPoint + row*settings.cellSideSize) + .6*settings.wallThick,1.0};
            cube.bottomFrontRight = (vec4){(settings.startPoint + col*settings.cellSideSize) + .6*settings.wallThick,yOffset,(settings.startPoint + row*settings.cellSideSize) + .6*settings.wallThick,1.0};

            createBox(vertices, cube, vertOffset+poles*36);
            poles++;
        }
    }
}

typedef enum
{
    FLYING_TO_START = 0,
    FLYING_AROUND,
    FLYING_DOWN,
    WARK_FORWARD,
    TURN_LEFT,
    TURN_RIGHT,
} state;

int isAnimating = 0;
int currentStep = 0;
state currentState = 1;

void idle(void)
{
    if (isAnimating)
    {
        currentStep++;

        //Go through and check what state.
    }
}

//Landon Higinbotham's code ends

int num_vertices = 0;

void init(void)
{
    //Landon Higinbotham's code starts
    //Generate maze
    srand(time(NULL)); //Set the seed using time
    maze[0][0].w = 1; //Open the start gate
    maze[0][0].visited = 1; //Set the start block to be visited (this is to stop multiple paths from being created)
    recursiveMazeBuilder(0, 0); //Recursively build the maze
    maze[mazeRows-1][mazeCols-1].e = 1; //Open the end gate
    mazeInfoBlock info = getMazeInfo();// Returns info we will use in generating the maze
    mazeSettings settings;
    settings.inset = 0.22;
    settings.cellSideSize = (2 - settings.inset*2) / mazeCols; 
    settings.wallThick = settings.cellSideSize * .1;
    settings.wallHeight = .5;
    settings.startPoint = -1+settings.inset;

    num_vertices = 6 + info.walls*6*6 + info.poles*6*6;

    vec4 vertices[num_vertices];
    vec2 tex_coords[num_vertices];

    //Ground (flat-plane) Notes are from top view
    float yOffset = -0.5;
    vertices[0] = (vec4){-1.0,yOffset,-1.0,1.0}; //Top left
    vertices[1] = (vec4){-1.0,yOffset,1.0,1.0}; //Bottom left
    vertices[2] = (vec4){1.0,yOffset,-1.0,1.0}; //Top right
    tex_coords[0] = (vec2){0.0,0.5};
    tex_coords[1] = (vec2){0.0,1};
    tex_coords[2] = (vec2){0.5,0.5};

    vertices[3] = (vec4){-1.0,yOffset,1.0,1.0}; //Bottom left
    vertices[4] = (vec4){1.0,yOffset,1.0,1.0}; //Bottom right
    vertices[5] = (vec4){1.0,yOffset,-1.0,1.0}; //Top Right
    tex_coords[3] = (vec2){0.0,1};
    tex_coords[4] = (vec2){0.5,1};
    tex_coords[5] = (vec2){0.5,0.5};


    int vertOffset = 6;//Offsets the array index by the amount we already used

    createWalls(vertices, info, vertOffset, yOffset, settings); //Add walls to the vertices array

    vertOffset += info.walls*6*6;

    createPoles(vertices, info, vertOffset, yOffset, settings);

    texture(tex_coords, 6, 6+info.walls*6*6, 1);
    texture(tex_coords, 6+info.walls*6*6, 6+info.walls*6*6+info.poles*6*6, 2);

    model_view = look_at((vec4){0.0,3,0.0,1.0},(vec4){0.0,-.5,0,1.0},(vec4){0,0,-1,0});
    projection = frustum(-1.0, 1.0, -1.0, 1.0, -1.0, -5.0);

    //Solve maze
    solveMazeCWRF(0,0, mazeRows-1, mazeCols-1, 0); //This will recursively solve the maze. It checks in order of E-S-W-N
    //Landon Higinbotham's code ends

    int width = 800;
    int height = 800;
    GLubyte my_texels[width][height][3];

    FILE *fp = fopen("p2texture04.raw", "r");
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(tex_coords), tex_coords);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + sizeof(vertices));

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + (sizeof(vertices)));

    GLuint texture_location = glGetUniformLocation(program, "texture");
    glUniform1i(texture_location, 0);

    //printf("texture_location: %i\n", texture_location);

    /*Landon Higinbotham's code starts here*/
    model_view_location = glGetUniformLocation(program, "model_view_matrix");
    projection_location = glGetUniformLocation(program, "projection_matrix");
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
    /*Landon Higinbotham's code ends here*/
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
	exit(0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Landon Higinbotham - Project 2");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
