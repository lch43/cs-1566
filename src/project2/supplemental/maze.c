#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
    int n;
    int e;
    int s;
    int w;
    int visited;
} mazeBlock;

const int mazeRows = 8;
const int mazeCols = 8;

mazeBlock maze[8][8];
int solution[64];

void printMaze()
{
    int i;
    int j;
    for (j = 0; j<mazeCols; j++)
    {
        //printf("(%d %d %d %d) ", maze[i][j].n, maze[i][j].e, maze[i][j].s, maze[i][j].w);
        if (maze[0][j].n == 0)
        {
            printf("._");
        }
        else
        {
            printf("  ");
        }
    }
    printf("\n");
    for (i = 0; i<mazeRows; i++)
    {
        for (j = 0; j<mazeCols; j++)
        {
            //printf("(%d %d %d %d) ", maze[i][j].n, maze[i][j].e, maze[i][j].s, maze[i][j].w);
            if (maze[i][j].w == 0)
            {
                printf("|");
            }
            else
            {
                printf(" ");
            }
            if (maze[i][j].s == 0)
            {
                printf("_");
            }
            else
            {
                printf(" ");
            }
            
        }
        if (maze[i][j-1].e == 0)
        {
            printf("|\n");
        }
        else
        {
            printf(" \n");
        }
        
    }
    /*for (int i = 0; i<mazeRows; i++)
    {
        for (int j = 0; j<mazeCols; j++)
        {
            printf("(%d %d %d %d) ", maze[i][j].n, maze[i][j].e, maze[i][j].s, maze[i][j].w);
        }
        printf("\n");
    }*/
}

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
        printMaze();
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
    printf("[ %d , %d ] \n", row, col);
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

void traceSolution()
{
    int i;

    for (i=0; i<mazeRows*mazeCols;i++)
    {
        int col = solution[i] % 8;
        int row = (solution[i] - col) / 8;

        printf("[ %d , %d ]\n", row, col);

        if (row == mazeRows-1 && col == mazeCols -1)
        {
            return;
        }
    }

    printf("No solution\n");
}

int main()
{
    //Generate maze
    srand(time(NULL)); //Set the seed using time
    //int startCol = rand() % 8; //Choose random start block (Deprecated since it seems project requires to start and end in corners)
    maze[0][0].w = 1;
    maze[0][0].visited = 1;
    recursiveMazeBuilder(0, 0);
    //startCol = rand() % 8;  //Choose random start block (Deprecated since it seems project requires to start and end in corners)
    maze[mazeRows-1][mazeCols-1].e = 1;
    
    printMaze();
    //Solve Maze Backtracking
    printf("Find solution\n");
    solveMazeCWRF(0,0, mazeRows-1, mazeCols-1, 0);
    //Solve using best path
    printf("Best solution\n");
    traceSolution();
    return 0;
}