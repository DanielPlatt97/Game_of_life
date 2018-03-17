#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #define CLEAR "cls"
#elif defined(unix) || defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    #define CLEAR "clear"
#else
    #define CLEAR "cls"
    #error "OS not supported."
#endif

void allocateBoardMemory();
void deallocateBoardMemory();
void readBoard();
void printBoard();
void stepBoard();
void calculateNewBoardSingleColumnOrRow(int columns, char (*newBoard)[columns]);
void calculateNewBoard(int columns, char (*newBoard)[columns]);
int countCell(int row, int column);
char classifyNewCell(char cellBefore, int numberOfNeighbours);
int countNeighboursForCentreCell(int r, int c);
int countNeighboursForCentreLeftCell(int r, int c);
int countNeighboursForCentreRightCell(int r, int c);
int countNeighboursForTopCentreCell(int r, int c);
int countNeighboursForTopLeftCell(int r, int c);
int countNeighboursForTopRightCell(int r, int c);
int countNeighboursForBottomCentreCell(int r, int c);
int countNeighboursForBottomLeftCell(int r, int c);
int countNeighboursForBottomRightCell(int r, int c);

int rows, columns; //the number of rows and columns the board is comprised of
char **board; //double pointer that is used as a global 2d array to act as the board

int main(void)
{
    system(CLEAR);
    printf("Input the number of rows and columns and the matrix in the same form as this example: \n");
    printf("3 7         \n");
    printf(".X...X.     \n");
    printf("X.X.X.X     \n");
    printf("X...X.X     \n");
    printf("Then press enter to step through each phase or type 'q' then enter to quit. \n");

    scanf("%d", &rows); scanf("%d", &columns); 
    if(rows==0 || columns==0) 
    {
        exit(1);
    }
    
    allocateBoardMemory();
    readBoard();
    
    printBoard();
    char input;
    while(input != 'q')
    {
        input = getchar();
        system(CLEAR);
        stepBoard();
        printBoard();
    }

    deallocateBoardMemory();
    return 1;
}

//allocates memory on the heap for the global array board
void allocateBoardMemory()
{
    board = (char**)calloc(rows, sizeof(char*)); //allocating memory on the heap for each row of the board (will persist till end of program)
    if(board == NULL) 
    {
        printf("There was not enough memory to create the board.");
        exit(1);
    }
    for(int r = 0; r < rows; r++)
    {
        *(board+r) = (char*)calloc(columns, sizeof(char)); //allocating memory on the heap for every column for each row of the board
        if(*(board+r) == NULL) 
        {
            printf("There was not enough memory to create the board.");
            exit(1);
        }
    }
}

//deallocates memory from the heap for the global array board so that the memory is free for the OS to use
void deallocateBoardMemory()
{
    for(int r = 0; r < rows; r++) free(board[r]); //deallocating memory used by each row
    free(board); //deallocating memory used by the board to point to each row
}

//will get input from user/file for every cell in the board (either X for live cell or . for dead cell)
void readBoard()
{
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < columns; c++)
        {
            scanf(" %c", &board[r][c]);
            //space before %c means whitespace or newline characters are ignored
        }
    }
}

//will print the char contained in every cell in the board (either X for live cell or . for dead cell)
void printBoard()
{
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < columns; c++)
        {
            printf("%c", board[r][c]);
        }
        printf("\n");
    }
}

//makes global array 'board' iterate to the next step (calculating which cells are now alive or dead)
void stepBoard()
{
    char newBoard[rows][columns];

    if(rows==1 || columns==1)
    {
        //handling special case were there is only one row or one column
        calculateNewBoardSingleColumnOrRow(columns, newBoard);
    }
    else
    {
        //calculates what the board should be after 1 step, making newBoard equal to this
        calculateNewBoard(columns, newBoard);
    }

    //making every value in the board equal to the calculated values in newBoard
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < columns; c++)
        {
            board[r][c] = newBoard[r][c];
        }
    }
}

//makes newBoard equal to board after one step. This function handles the special case where either rows or columns is 0
void calculateNewBoardSingleColumnOrRow(int columns, char (*newBoard)[columns])
{
    if(rows==1 && columns==1) //cell must be dead as no cells around it
    {
        newBoard[0][0] = '.';
    }
    else if(rows == 1)
    {
        newBoard[0][0] = '.'; //first cell in the row has one neighbour at most so must be dead
        for(int c = 1; c < columns-1; c++)
        {
            if(board[0][c-1]=='X' && board[0][c]=='X' && board[0][c+1]=='X')
            {
                newBoard[0][c] = 'X'; //only cells that were alive before and have neighbours either side can be alive
            }
            else newBoard[0][c] = '.';
        }
        newBoard[0][columns-1] = '.';
    }
    else if(columns == 1)
    {
        newBoard[0][0] = '.';
        for(int r = 1; r < rows-1; r++)
        {
            if(board[r-1][0]=='X' && board[r][0]=='X' && board[r+1][0]=='X')
            {
                newBoard[r][0] = 'X';
            }
            else newBoard[r][0] = '.';
        }
        newBoard[rows-1][0] = '.';
    }
}

//makes newBoard equal to board after one step, applies the rules to each cell in board
void calculateNewBoard(int columns, char (*newBoard)[columns])
{
    for(int r = 0; r < rows; r++)
    {
        if(r < 1) //The row we are checking only has cells bellow (they are top cells)
        {
            newBoard[r][0] = classifyNewCell(board[r][0], countNeighboursForTopLeftCell(r, 0)); //This top cell has no cells to the left
            for(int c = 1; c < columns-1; c++)
            {
                newBoard[r][c] = classifyNewCell(board[r][c], countNeighboursForTopCentreCell(r, c)); //All these top cells have cells on either side
            }
            newBoard[r][columns-1] = classifyNewCell(board[r][columns-1], countNeighboursForTopRightCell(r, columns-1)); //This top cell has no cells to the right
        }
        else if(r > rows-2) //The row we are checking only has cells above (they are bottom cells)
        {
            newBoard[r][0] = classifyNewCell(board[r][0], countNeighboursForBottomLeftCell(r, 0)); //This bottom cell has no cells to the left
            for(int c = 1; c < columns-1; c++)
            {
                newBoard[r][c] = classifyNewCell(board[r][c], countNeighboursForBottomCentreCell(r, c)); //All these bottom cells have cells on either side
            }
            newBoard[r][columns-1] = classifyNewCell(board[r][columns-1], countNeighboursForBottomRightCell(r, columns-1)); //This bottom cell has no cells to the right
        }
        else //The row we are checking has cells above and bellow (they are centre cells)
        {
            newBoard[r][0] = classifyNewCell(board[r][0], countNeighboursForCentreLeftCell(r, 0)); //This centre cell has no cells to the left
            for(int c = 1; c < columns-1; c++)
            {
                newBoard[r][c] = classifyNewCell(board[r][c], countNeighboursForCentreCell(r, c)); //All these centre cells have cells on either side
            }
            newBoard[r][columns-1] = classifyNewCell(board[r][columns-1], countNeighboursForCentreRightCell(r, columns-1)); //This centre cell has no cells to the right
        }
    }
}

//classifies a cell in board based on the 4 rules by returning . or X. uses the type of cell it was before and the number of neighbours to decide.
char classifyNewCell(char cellBefore, int numberOfNeighbours)
{
    if(cellBefore == 'X')
    {
        if(numberOfNeighbours < 2) return '.';
        if(numberOfNeighbours > 3) return '.';
        else return 'X';
    }
    else
    {
        if(numberOfNeighbours == 3) return 'X';
        else return '.';
    }
}

//checks a cell in board at the row and column specified, returns 1 if alive or 0 if dead so they can be counted
int countCell(int row, int column)
{
    if(board[row][column] == 'X') return 1;
    else return 0;
}

//the following functions will count the neighbouring cells for cells of different positions.
//these functions take into account a cells location on the board and will not count cells that are not present if a cell is on the border.
int countNeighboursForCentreCell(int r, int c)
{
    return
    countCell(r-1, c-1)+    countCell(r-1, c  )+    countCell(r-1, c+1)+
    countCell(r  , c-1)+    /*      CELL      */    countCell(r  , c+1)+
    countCell(r+1, c-1)+    countCell(r+1, c  )+    countCell(r+1, c+1);
}
int countNeighboursForCentreLeftCell(int r, int c)
{
    return
    /* OUT OF BOUNDS  */    countCell(r-1, c  )+    countCell(r-1, c+1)+
    /* OUT OF BOUNDS  */    /*      CELL      */    countCell(r  , c+1)+
    /* OUT OF BOUNDS  */    countCell(r+1, c  )+    countCell(r+1, c+1);
}
int countNeighboursForCentreRightCell(int r, int c)
{
    return
    countCell(r-1, c-1)+    countCell(r-1, c  )+    /* OUT OF BOUNDS  */
    countCell(r  , c-1)+    /*      CELL      */    /* OUT OF BOUNDS  */
    countCell(r+1, c-1)+    countCell(r+1, c  );    /* OUT OF BOUNDS  */
}
int countNeighboursForTopCentreCell(int r, int c)
{
    return
    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */
    countCell(r  , c-1)+    /*      CELL      */    countCell(r  , c+1)+
    countCell(r+1, c-1)+    countCell(r+1, c  )+    countCell(r+1, c+1);
}
int countNeighboursForTopLeftCell(int r, int c)
{
    return
    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */
    /* OUT OF BOUNDS  */    /*      CELL      */    countCell(r  , c+1)+
    /* OUT OF BOUNDS  */    countCell(r+1, c  )+    countCell(r+1, c+1);
}
int countNeighboursForTopRightCell(int r, int c)
{
    return
    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */
    countCell(r  , c-1)+    /*      CELL      */    /* OUT OF BOUNDS  */
    countCell(r+1, c-1)+    countCell(r+1, c  );    /* OUT OF BOUNDS  */
}
int countNeighboursForBottomCentreCell(int r, int c)
{
    return
    countCell(r-1, c-1)+    countCell(r-1, c  )+    countCell(r-1, c+1)+
    countCell(r  , c-1)+    /*      CELL      */    countCell(r  , c+1);
    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */
}
int countNeighboursForBottomLeftCell(int r, int c)
{
    return
    /* OUT OF BOUNDS  */    countCell(r-1, c  )+    countCell(r-1, c+1)+
    /* OUT OF BOUNDS  */    /*      CELL      */    countCell(r  , c+1);
    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */
}
int countNeighboursForBottomRightCell(int r, int c)
{
    return
    countCell(r-1, c-1)+    countCell(r-1, c  )+    /* OUT OF BOUNDS  */
    countCell(r  , c-1);    /*      CELL      */    /* OUT OF BOUNDS  */
    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */    /* OUT OF BOUNDS  */
}
