
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

void createWalls(vec4 * vertices, vec2 * tex_coords, mazeInfoBlock info, int vertOffset, int yOffset)
{
    //Maze will be given a .1 inset from all sides of the plane.
    float inset = 0.1;
    float cellSideSize = (2 - inset*2) / mazeCols; 
    float wallThick = cellSideSize * .1;
    float wallHeight = .5;
    float startPoint = -1+inset;
    int i=0;
    int wall=0;
    for (i=0;i<2* mazeCols* mazeRows+ mazeCols + mazeRows && wall<info.walls;i++)
    {
        int foundWall = 0;
        vec4 topBackLeft;
        vec4 topBackRight;
        vec4 topFrontLeft;
        vec4 topFrontRight;

        vec4 bottomBackLeft;
        vec4 bottomBackRight;
        vec4 bottomFrontLeft;
        vec4 bottomFrontRight;
        
        if (i<(mazeRows+1)*mazeCols) //Wall is horizontal
        {
            if (info.wallArray[i] == 1)
            {
                foundWall = 1;
                int wallCol = i%mazeCols;
                int wallRow = (i-wallCol)/(mazeCols);
                
                topBackLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                topBackRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                topFrontLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                topFrontRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset+wallHeight,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                
                bottomBackLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                bottomBackRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)-.5*wallThick,1.0};
                bottomFrontLeft = (vec4){startPoint+wallCol*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                bottomFrontRight = (vec4){startPoint+(wallCol+1)*cellSideSize,yOffset,(startPoint+wallRow*cellSideSize)+.5*wallThick,1.0};
                
            }
        }
        else //Wall is vertical
        {
            if (info.wallArray[i] == 1)
            {
                foundWall = 1;
                int wallCol = (i-((mazeRows+1)*mazeCols))%(mazeCols+1);
                int wallRow = ((i-((mazeRows+1)*mazeCols))-wallCol)/(mazeCols+1);

                topBackLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset+wallHeight,startPoint+wallRow*cellSideSize,1.0};
                topBackRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset+wallHeight,startPoint+wallRow*cellSideSize,1.0};
                topFrontLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset+wallHeight,startPoint+(wallRow+1)*cellSideSize,1.0};
                topFrontRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset+wallHeight,startPoint+(wallRow+1)*cellSideSize,1.0};

                bottomBackLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset,startPoint+wallRow*cellSideSize,1.0};
                bottomBackRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset,startPoint+wallRow*cellSideSize,1.0};
                bottomFrontLeft = (vec4){(startPoint+wallCol*cellSideSize)-.5*wallThick,yOffset,startPoint+(wallRow+1)*cellSideSize,1.0};
                bottomFrontRight = (vec4){(startPoint+wallCol*cellSideSize)+.5*wallThick,yOffset,startPoint+(wallRow+1)*cellSideSize,1.0};
                
            }
        }

        if (foundWall == 1)
        {
                //Front face
                vertices[vertOffset+wall*36+0] = topFrontRight;
                vertices[vertOffset+wall*36+1] = topFrontLeft;
                vertices[vertOffset+wall*36+2] = bottomFrontLeft;
                tex_coords[vertOffset+wall*36+0] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+1] = (vec2){0.0,0.0};
                tex_coords[vertOffset+wall*36+2] = (vec2){0.0,0.5};

                vertices[vertOffset+wall*36+3] = topFrontRight;
                vertices[vertOffset+wall*36+4] = bottomFrontLeft;
                vertices[vertOffset+wall*36+5] = bottomFrontRight;
                tex_coords[vertOffset+wall*36+3] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+4] = (vec2){0.0,0.5};
                tex_coords[vertOffset+wall*36+5] = (vec2){0.5,0.5};

                //Back face
                vertices[vertOffset+wall*36+6] = topBackLeft;
                vertices[vertOffset+wall*36+7] = topBackRight;
                vertices[vertOffset+wall*36+8] = bottomBackRight;
                tex_coords[vertOffset+wall*36+6] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+7] = (vec2){0.0,0.0};
                tex_coords[vertOffset+wall*36+8] = (vec2){0.0,0.5};

                vertices[vertOffset+wall*36+9] = topBackLeft;
                vertices[vertOffset+wall*36+10] = bottomBackRight;
                vertices[vertOffset+wall*36+11] = bottomBackLeft;
                tex_coords[vertOffset+wall*36+9] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+10] = (vec2){0.0,0.5};
                tex_coords[vertOffset+wall*36+11] = (vec2){0.5,0.5};

                //Left face
                vertices[vertOffset+wall*36+12] = topFrontLeft;
                vertices[vertOffset+wall*36+13] = topBackLeft;
                vertices[vertOffset+wall*36+14] = bottomBackLeft;
                tex_coords[vertOffset+wall*36+12] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+13] = (vec2){0.0,0.0};
                tex_coords[vertOffset+wall*36+14] = (vec2){0.0,0.5};

                vertices[vertOffset+wall*36+15] = topFrontLeft;
                vertices[vertOffset+wall*36+16] = bottomBackLeft;
                vertices[vertOffset+wall*36+17] = bottomFrontLeft;
                tex_coords[vertOffset+wall*36+15] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+16] = (vec2){0.0,0.5};
                tex_coords[vertOffset+wall*36+17] = (vec2){0.5,0.5};

                //Right face
                vertices[vertOffset+wall*36+18] = topBackRight;
                vertices[vertOffset+wall*36+19] = topFrontRight;
                vertices[vertOffset+wall*36+20] = bottomFrontRight;
                tex_coords[vertOffset+wall*36+18] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+19] = (vec2){0.0,0.0};
                tex_coords[vertOffset+wall*36+20] = (vec2){0.0,0.5};

                vertices[vertOffset+wall*36+21] = topBackRight;
                vertices[vertOffset+wall*36+22] = bottomFrontRight;
                vertices[vertOffset+wall*36+23] = bottomBackRight;
                tex_coords[vertOffset+wall*36+21] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+22] = (vec2){0.0,0.5};
                tex_coords[vertOffset+wall*36+23] = (vec2){0.5,0.5};

                //Top face
                vertices[vertOffset+wall*36+24] = topBackRight;
                vertices[vertOffset+wall*36+25] = topBackLeft;
                vertices[vertOffset+wall*36+26] = topFrontLeft;
                tex_coords[vertOffset+wall*36+24] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+25] = (vec2){0.0,0.0};
                tex_coords[vertOffset+wall*36+26] = (vec2){0.0,0.5};

                vertices[vertOffset+wall*36+27] = topBackRight;
                vertices[vertOffset+wall*36+28] = topFrontLeft;
                vertices[vertOffset+wall*36+29] = topFrontRight;
                tex_coords[vertOffset+wall*36+27] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+28] = (vec2){0.0,0.5};
                tex_coords[vertOffset+wall*36+29] = (vec2){0.5,0.5};

                //Bottom face
                vertices[vertOffset+wall*36+30] = bottomFrontRight;
                vertices[vertOffset+wall*36+31] = bottomFrontLeft;
                vertices[vertOffset+wall*36+32] = bottomBackLeft;
                tex_coords[vertOffset+wall*36+30] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+31] = (vec2){0.0,0.0};
                tex_coords[vertOffset+wall*36+32] = (vec2){0.0,0.5};

                vertices[vertOffset+wall*36+33] = bottomFrontRight;
                vertices[vertOffset+wall*36+34] = bottomBackLeft;
                vertices[vertOffset+wall*36+35] = bottomBackRight;
                tex_coords[vertOffset+wall*36+33] = (vec2){.5,0.0};
                tex_coords[vertOffset+wall*36+34] = (vec2){0.0,0.5};
                tex_coords[vertOffset+wall*36+35] = (vec2){0.5,0.5};
                wall++;
        }
    }
}

//Landon Higinbotham's code ends

int num_vertices = 0;

//vec2 tex_coords[6] = {{0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}};

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

    num_vertices = 6 + info.walls*6*6/* + info.poles*6*6*/;

    vec4 vertices[num_vertices];
    vec4 colors[num_vertices];
    int v = 0;
    for (v=0; v<num_vertices;v++)
    {
        colors[v] = (vec4){0,0,0,0};
    }
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

    createWalls(vertices, tex_coords, info, vertOffset, yOffset); //Add walls to the vertices array

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

    printf("texture_location: %i\n", texture_location);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glutMainLoop();

    return 0;
}
