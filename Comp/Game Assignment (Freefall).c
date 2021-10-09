// Author: George Bai z5254903
// Date: 23/06/2020
// UNSW Freefall assignment.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 15
#define EMPTY 0
#define STONE 1
#define MARCHINGSTONE 2
#define MINTNT 3
#define MAXTNT 9

#define ABOVESIZE 150
#define RIGHT 1
#define LEFT -1
#define MINBOUND 0
#define MAXBOUND 14
#define MAXBOUNDABOVE 149
#define MOVEPLAYER 1
#define FIRELASER 2
#define SHIFTDOWN 3
#define VERTICALFLIP 4
#define GRAVITYSTORM 5
#define MAXSTONESSHOT 3

#define MAXDIRECTIONS 3

#define GRAVITYLEFT 1
#define GRAVITYTOPLEFT 2
#define GRAVITYTOPRIGHT 3
#define GRAVITYRIGHT 4

#define MARCHDOWNONE 0
#define MARCHRIGHT 1
#define MARCHDOWNTWO 2
#define MARCHLEFT 3

// Map of Stones
void printMap(int map[SIZE][SIZE], int playerX);

// Stone movement
void addStones(int map[SIZE][SIZE], int mapAbove[ABOVESIZE][SIZE], int row, int column, int length, int value);
void shootStones(int map[SIZE][SIZE], int playerX);
void shiftStones(int map[SIZE][SIZE], int mapAbove[ABOVESIZE][SIZE]);
void shiftStonesAbove(int mapAbove[ABOVESIZE][SIZE]);
void marchStones(int map[SIZE][SIZE], int direction);
void explodeStones(int map[SIZE][SIZE], int centreX, int centreY, double radius);
void verticalFlipStones(int map[SIZE][SIZE]);
void changeMapGravity(int map[SIZE][SIZE], int gravityDirection);
void marchRight(int map[SIZE][SIZE], int row, int column);
void marchLeft(int map[SIZE][SIZE], int row, int column);

// TNT detonation Range
void checkWithinRadius(int map[SIZE][SIZE], int centreX, int centreY, double radius, int row, int column);

// Gravity stone movements
void gravityRight(int map[SIZE][SIZE]);
void gravityPushRight(int map[SIZE][SIZE], int row, int column);
void gravityLeft(int map[SIZE][SIZE]);
void gravityPushLeft(int map[SIZE][SIZE], int row, int column);
void gravityTopRight(int map[SIZE][SIZE]); 
void gravityPushTopRight(int map[SIZE][SIZE], int row, int column);
void gravityTopLeft(int map[SIZE][SIZE]);
void gravityPushTopLeftt(int map[SIZE][SIZE], int row, int column);

// Player Movement
int changePlayerPos(int playerX, int movement);

// End Game conditions
int checkNoStone(int map[SIZE][SIZE]);
int checkStoneFinalRow(int map[SIZE][SIZE]);

// Above are all the definitions and functions required for this program to run

int main (void) {
    // This line creates our 2D array called "map" and sets all
    // of the blocks in the map to EMPTY.
    int map[SIZE][SIZE] = {EMPTY};
    int mapAbove[ABOVESIZE][SIZE] = {EMPTY};

    // This line creates out playerX variable. The player starts in the
    // middle of the map, at position 7.
    int playerX = SIZE / 2;
    
    int linesOfStone;
    printf("How many lines of stone? ");
    scanf("%d", &linesOfStone);
    // This scans in the number of lines of block 

    int row, column, length, value;
    printf("Enter lines of stone:\n");
    
    int lineCounter = 0;
    while (lineCounter < linesOfStone) {
        scanf("%d %d %d %d", &row, &column, &length, &value);
        addStones(map, mapAbove, row, column, length, value);
        // This Scans in the line of blocks and add them to either
        // the normal map or the hidden map above the normal map
        lineCounter++;
    }


    printMap(map, playerX);
    
    int flipCounter = 0;
    int gravityFlipCounter = 0;
    // These counters ensure that a specific flip can only be used once
    
    int direction = 0;
    // This determines what direction a marching stone goes
    
    int command;
    // There are five different commands and end of file to check through
    
    while (!(scanf("%d", &command) == EOF)) {
        if (command == MOVEPLAYER) {
            int movement;
            scanf("%d", &movement);
            playerX = changePlayerPos(playerX, movement);
            // Moves the player right or left by one space if valid
        }
        else if (command == FIRELASER) {
            shootStones(map, playerX);
            // Shoot three stones in front of the player
            if (!checkNoStone(map)) {
                printMap(map, playerX);               
                printf("Game won!\n");
                return 0;
            }
            // Ends the program if the player has shot all the stones on the map
        }
        else if (command == SHIFTDOWN) {
            if (checkStoneFinalRow(map)) {
                printMap(map, playerX);               
                printf("Game lost!\n");
                return 0;
            }
            // Checks whether the final row has been shifted down with a stone
            // in it. This is a Game lost! condition
            
            shiftStones(map, mapAbove);
            // Moves all the stones in the normal map forward one step 
            // and gets a new first row from the map above the normal map
            marchStones(map, direction);
            // The marching stones are moved to their proper positions
            // relative to the marching stone direction state
            shiftStonesAbove(mapAbove);
            // The stones in the map above the normal map are all moved forward
            // one step
            
          
            if (direction < MAXDIRECTIONS) {
                direction++;
            }
            else {
                direction = 0;
            }
            // Ensures that the marching stones can only be moving down, right
            // or left, with down repeating twice. The directions loop around
        }
        else if (command == VERTICALFLIP) {
            if (!flipCounter) {
                flipCounter++;
                verticalFlipStones(map);
                // Flips the stones in the normal map vertically across 
                // and prevents future flips
            }
        }
        else if (command == GRAVITYSTORM) {
            int gravityDirection;
            scanf("%d", &gravityDirection);
            if (!gravityFlipCounter) {
                gravityFlipCounter++;
                changeMapGravity(map, gravityDirection);
            }
            // Moves the stones either right, left, top right, or top left
            // and prevents future flips
        }
        printMap(map, playerX);
        // The map is displayed after each command
    }
    return 0;
    // Ends the program once EOF has been reached
}

int changePlayerPos(int playerX, int movement) {
    int new_position = playerX + movement;
    if (movement == RIGHT || movement == LEFT) {
        if (new_position >= MINBOUND && new_position <= MAXBOUND) {
            return new_position;
        }
    }
    return playerX;
}
/* This checks that the player's position will remain within the bounds of
   the map and that the movement is a valid movement, either one space to the 
   left or one space to the right. It will return the new position if so, or it  
   will just return the player's position without change
*/

int checkNoStone(int map[SIZE][SIZE]) {
    int row = 0;
    int column = 0;
    int stoneCounter = 0;
    
    while (row < SIZE) {
        while (column < SIZE) {
            if (map[row][column]) {
                stoneCounter++;
            }
            column++;
        }
        column = 0;
        row++;
    }
    return stoneCounter;
}
/* This goes through the entire normal map and checks whether there is even
   a single stone in its entirety. If there is none, then the game is won in
   the main program
*/

int checkStoneFinalRow(int map[SIZE][SIZE]) {
    int column = 0;
    int stoneCounter = 0;
    
    while (column < SIZE) {
        if (map[MAXBOUND][column]) {
            stoneCounter++;
        }
        column++;
    }
    return stoneCounter;
}
/* This goes through the final row of the normal map and checks whether
   there is even a single stone there. If there is one and the map shifts
   down, then the game is lost in the main program
*/

void addStones(int map[SIZE][SIZE], int mapAbove[ABOVESIZE][SIZE], int row, 
                                    int column, int length, int value) {
    int stonesPlaced = 0;
    // The conditions check that all the stones that the input desires to be
    // placed are within bounds and if not, it does nothing
    if (row >= MINBOUND && column >= MINBOUND && (column + length) <= SIZE) {
        while (stonesPlaced < length) {
            map[row][column + stonesPlaced] = value;
            stonesPlaced++;
        }
    }
    // Determines whether the inputted stones are to be placed on the normal map
    // and then places them down 
    
    else if (row >= -ABOVESIZE && column >= 0 && (column + length) <= SIZE) {
        while (stonesPlaced < length) {
            mapAbove[ABOVESIZE + row][column + stonesPlaced] = value;
            stonesPlaced++;
        }
    }
    // Determines whether the inputted stones are to be placed on the map above
    // and then places them down
}

void shootStones(int map[SIZE][SIZE], int playerX) {
    int rowCounter = 0;
    int stoneShotCounter = 0;
    
    while (rowCounter < SIZE && stoneShotCounter < MAXSTONESSHOT) {
        if (map[MAXBOUND - rowCounter][playerX] == STONE || 
            map[MAXBOUND - rowCounter][playerX] == MARCHINGSTONE) {
            
            map[MAXBOUND - rowCounter][playerX] = EMPTY;
            // Kills the stone when it is hit by the bullet
            stoneShotCounter++;
        }
        else if (map[MAXBOUND - rowCounter][playerX] >= MINTNT && 
                 map[MAXBOUND - rowCounter][playerX] <= MAXTNT) {
                 
            explodeStones(map, playerX, MAXBOUND - rowCounter, 
                          map[MAXBOUND - rowCounter][playerX] * 1.0);
            // This destroys the all the stones around the TNT block hit
            stoneShotCounter = MAXSTONESSHOT;
            // Ends the shots if the player's shot hits a TNT block
        }
        rowCounter++;    
    }
}
/* This iterates through all the rows in front of the player and kills them
   if they are in the path of the bullet and the bullet has not reached its
   kill limit yet. It can kill up to three normal or marching stones, and 
   a maximum of one tnt stone
*/

void explodeStones(int map[SIZE][SIZE], int centreX, int centreY, double radius) {
    int row = 0;
    int column = 0;
    
    while (row < SIZE) {
        while (column < SIZE) {
            checkWithinRadius(map, centreX, centreY, radius, row, column);
            column++;
        }
        column = 0;
        row++;    
    }
}
// Kills all stones within a certain radius of the TNT block

void checkWithinRadius(int map[SIZE][SIZE], int centreX, int centreY, 
                       double radius, int row, int column) {
    double distance;

    if (map[row][column]) {
        distance = sqrt(pow(column - centreX, 2) + pow(row - centreY, 2)); 
        // Uses pythagoras to find the distance between two points on a map
        if (distance < radius) {        
            map[row][column] = EMPTY;
        }
        // If the stone is within the blast radius, the stone is killed 
    }    
}
// Checks whether a point lies within the TNT's blast radius and kills it

void shiftStones(int map[SIZE][SIZE], int mapAbove[ABOVESIZE][SIZE]) {
    int rowCounter = 0;
    int columnCounter = 0;
    
    while (rowCounter < SIZE) {
        while (columnCounter < SIZE) {
            int row = MAXBOUND - rowCounter; 
            int rowAbove = row - 1;
            
            if (row > MINBOUND) {
                map[row][columnCounter] = map[rowAbove][columnCounter];
                /* Starting from the bottom up, this takes a row and a column 
                   and replaces the value within that position with the values
                   in the row above
                */
            }
            else {
                map[row][columnCounter] = mapAbove[MAXBOUNDABOVE][columnCounter];
            }
            // If the first row needs more values, it takes it from the map
            // above the normal map
            columnCounter++;
        }
        columnCounter = 0;
        rowCounter++;
    }
}
// Shifts all stones in the normal map one row down

void shiftStonesAbove(int mapAbove[ABOVESIZE][SIZE]) {
    int rowCounter = 0;
    int columnCounter = 0;
    
    while (rowCounter < MAXBOUNDABOVE) {
        while (columnCounter < SIZE) {
            int row = MAXBOUNDABOVE - rowCounter; 
            int rowAbove = row - 1;
            
            if (row > MINBOUND) {
                mapAbove[row][columnCounter] = mapAbove[rowAbove][columnCounter];
            }
            else {
                mapAbove[row][columnCounter] = EMPTY;
            }
            // The topmost row of the map above the normal map has no new stones
            // feeding into it
            columnCounter++;
        }
        columnCounter = 0;
        rowCounter++;
    }
}
// Shifts all stones in the map above the normal map one row down, potentially
// out of the map

void marchRight(int map[SIZE][SIZE], int row, int column) {
    int rowAbove = row - 1;
    // Since we have moved all the stones foward already, we need to now
    // move the marching stones back and consume whatever is in their path
    int columnRight = column + 1;
    
    if (column < MAXBOUND) {
        map[rowAbove][columnRight] = map[row][column];
    }
    // Moves the marching stone to the right
    else {
        map[rowAbove][column] = map[row][column];     
    }
    // If the column is at the edge of the map, it can't move right any further
    // and so it just follows the edge
    
    map[row][column] = EMPTY;
    // Because the marching stone is moving right, the position in front of
    // where it was initially before the stones stepping forward 
    // must be empty because the stone there would have moved forwards
}

void marchLeft(int map[SIZE][SIZE], int row, int column) {
    int rowAbove = row - 1;
    // Since we have moved all the stones foward already, we need to now
    // move the marching stones back and consume whatever is in their path
    int columnLeft = column - 1;
    
    if (column > MINBOUND) {
        map[rowAbove][columnLeft] = map[row][column];
    }
    // Moves the marching stone to the left
    else {
        map[rowAbove][column] = map[row][column];     
    }
    // If the column is at the edge of the map, it can't move left any further
    // and so it just follows the edge
    
    map[row][column] = EMPTY;
    // Because the marching stone is moving left, the position in front of
    // where it was initially before the stones stepping forward 
    // must be empty because the stone there would have moved forwards
}

void marchStones(int map[SIZE][SIZE], int direction) {
    int row = 1;
    // We know the minimum row must be 1 because we have already moved all 
    // stones on the normal map forward by one beforehand and we do not
    // want marching stones that have just entered from the map above
    // to retreat back
    int column = 0;
    
    while (row < SIZE) {
        while (column < SIZE) {
            if (direction == MARCHDOWNONE || direction == MARCHDOWNTWO) {
                row = SIZE;
                column = SIZE;
            }
            // When direction of marching stone is down, all stones have 
            // been pushed down already so nothing is necessary except
            // to leave the loop
            else if (direction == MARCHRIGHT && map[row][column] == MARCHINGSTONE) {
                marchRight(map, row, column);   
            }
            // The stone jumps to the top right to make up for having moved
            // a step forward already
            else if (direction == MARCHLEFT && map[row][column] == MARCHINGSTONE) {
                marchLeft(map, row, column);
            }
            // The stone jumps to the top left to make up for having moved
            // a step forward already
            column++;
        }
        column = 0;
        row++;
    }
}
// Moved the marching stones to their rightful positions after
// the function moving all stones a step forward has been called

void verticalFlipStones(int map[SIZE][SIZE]) {
    int row = 0;
    int column = 0;
    
    while (row < SIZE/2 + 1) {
        // Flips the stones through the middle rows, so going through 
        //all the rows would simply leave us with the original state
        while (column < SIZE) {
            int originalValue = map[MAXBOUND - row][column];

            map[MAXBOUND - row][column] = map[row][column];
            map[row][column] = originalValue;
            // Switches the values of two opposing rows with the same column
            // around
            column++;
        }
        column = 0;
        row++;
    }
}
// Flips the stones vertically

void gravityPushLeft(int map[SIZE][SIZE], int row, int column) {
    int columnRight = column + 1;
    
    if (map[row][column] == EMPTY) {
        map[row][column] = map[row][columnRight];
        map[row][columnRight] = EMPTY;
    }
}
// Takes the value within the column to the right and transfers it to 
// one space to the left, leaving an empty space behind

void gravityLeft(int map[SIZE][SIZE]) {
    int row = 0;
    int column = 0;
    int repetitions = 0;
    // Since it pushes one step at a time, this is how we ensure that
    // the end result has all the stones together with no gaps in between
    
    while (repetitions < SIZE) {
        while (row < SIZE) {
            while (column < SIZE - 1) {
                gravityPushLeft(map, row, column);
                column++;
            }
            // The column is limited just below the size so that the edgemost
            // stones aren't taking values from outside the map array
            column = 0;
            row++;
        }     
        repetitions++;
        row = 0;
        column = 0;
    }
}
// This is a crude algorithm that pushes all stones to the left one step at
// a time until the maximum possible amount of movements to the left is
// checked for

void gravityPushRight(int map[SIZE][SIZE], int row, int column) {
    int columnRight = column + 1;
    
    if (map[row][MAXBOUND - column] == EMPTY) {
        map[row][MAXBOUND - column] = map[row][MAXBOUND - columnRight];
        map[row][MAXBOUND - columnRight] = EMPTY;
    }
}
// Takes the value within the column to the left and transfers it to 
// one space to the right, leaving an empty space behind


void gravityRight(int map[SIZE][SIZE]) {
    int row = 0;
    int column = 0;
    int repetitions = 0;
    // Since it pushes one step at a time, this is how we ensure that
    // the end result has all the stones together with no gaps in between
    
    while (repetitions < SIZE) {
        while (row < SIZE) {
            while (column < SIZE - 1) {
                gravityPushRight(map, row, column);
                column++;
            }
            // The column is limited just below the size so that the leftmost
            // stones aren't taking values from outside the map array to the 
            // left (which is why we use MAXBOUND - column in 
            // gravityPushRight)
            column = 0;
            row++;
        }     
        repetitions++;
        row = 0;
        column = 0;
    }
}
// This is a crude algorithm that pushes all stones to the right one step at
// a time until the maximum possible amount of movements to the right is
// checked for

void gravityPushTopLeft(int map[SIZE][SIZE], int row, int column) {
    int rowBelow = row + 1;
    int columnRight = column + 1;
    
    if (map[row][column] == EMPTY) {
        map[row][column] = map[rowBelow][columnRight];
        map[rowBelow][columnRight] = EMPTY;
    }
}
// Takes the value within the space to the bottom right and takes it 
// one space to the top left, leaving an empty space behind

void gravityTopLeft(int map[SIZE][SIZE]) {
    int row = 0;
    int column = 0;
    int repetitions = 0;
    // Since it pushes one step at a time, this is how we ensure that
    // the end result has all the stones together with no gaps in between
    
    while (repetitions < SIZE) {
        while (row < SIZE - 1) { 
            while (column < SIZE - 1) {
                gravityPushTopLeft(map, row, column);
                column++;
            }
            column = 0;
            row++;
        } 
        // These ranges are constructed so that if there is a movable stone
        // to the bottom right of any of the spaces on the map, it will move
        // towards the top left
        // The only stones that cannot be reached to the bottom right are at
        // The top right most corner and the bottom left most corner
        
        repetitions++;
        row = 0;
        column = 0;
    }
}
// This is a crude algorithm that pushes all stones to the top left one step at
// a time until the maximum possible amount of movements to the top left is
// checked for

void gravityPushTopRight(int map[SIZE][SIZE], int row, int column) {
    int rowBelow = row + 1;
    int columnLeft = column - 1;
    
    if (map[row][column] == EMPTY) {
        map[row][column] = map[rowBelow][columnLeft];
        map[rowBelow][columnLeft] = EMPTY;
    }
}
// Takes the value within the space to the bottom left and takes it 
// one space to the top right, leaving an empty space behind

void gravityTopRight(int map[SIZE][SIZE]) {
    int row = 0;
    int column = 1;
    // The column starts at one so that there will always be something to the
    // left for the stone to move towards the top right for
    
    int repetitions = 0;
    // Since it pushes one step at a time, this is how we ensure that
    // the end result has all the stones together with no gaps in between
    
    while (repetitions < SIZE) {
        while (row < SIZE - 1) { 
            while (column < SIZE) {
                gravityPushTopRight(map, row, column);
                column++;
            }
            column = 1;
            row++;
        } 
        // These ranges are constructed so that if there is a movable stone
        // to the bottom left of any of the spaces on the map, it will move
        // towards the top right
        // The only stones that cannot be reached to the bottom left are at
        // The top left most corner and the bottom right most corner
        repetitions++;
        row = 0;
        column = 1;
    }
    
}
// This is a crude algorithm that pushes all stones to the top right one step at
// a time until the maximum possible amount of movements to the top right is
// checked for

void changeMapGravity(int map[SIZE][SIZE], int gravityDirection) {
    if (gravityDirection == GRAVITYLEFT) {
        gravityLeft(map);
    }
    else if (gravityDirection == GRAVITYTOPLEFT) {
        gravityTopLeft(map);
    }
    else if (gravityDirection == GRAVITYTOPRIGHT) {
        gravityTopRight(map);
    }
    else if (gravityDirection == GRAVITYRIGHT) {
        gravityRight(map);
    }
    
}
// Checks which direction the gravity forces the stones towards
// and pushes them all that way until they are all packed together
// with no spaces in between them in the direction of gravity


// Print out the contents of the map array. Then print out the player line
// which will depends on the playerX variable.
void printMap(int map[SIZE][SIZE], int playerX) {
    
    // Print values from the map array.
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            printf("%d ", map[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }    
    
    // Print the player line.
    i = 0;
    while (i < playerX) {
        printf("  ");
        i++;
    }
    printf("P\n");
}

