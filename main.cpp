#include "raylib.h" //call raylib libary and include 

#if defined(PLATFORM_WEB) //if defined the platform is the web
    #include <emscripten/emscripten.h> //inclube emscripten as it is open source compiler tool
#endif //end of if statement


// Some Defines

#define MAX_TUBES 100  // defining the size of the tubes
#define FLOPPY_RADIUS 16 // the raduis of the circle (player)
#define TUBES_WIDTH 80 // the width of the tubes
#define MOVING_TUBES_HEIGHT 100 // the moving tubes in the game height
#define MOVING_TUBES_SPACING 100 // the moving tubes space also set to integer 100
#define VARIATION_AMOUNT 10 // defining the variation in the amount to be set to integer 10
#define VARIATION_SPEED 0.3f // speed of variation set to float 0.3

#define COLLISION_AUDIO_FILE_PATH "resources/collision.wav" //including the file pathway to the audio cannot be changed as declared at beginning of code
#define BACKGROUD_MUSIC_FILE_PATH "resources/background.wav" //defining background noise of collision for this to be included


// Types and Structures Definition grouping several related variables into one place. The structure contains many different variable types an int, float, bool etc

typedef struct Floppy { //define the variable floppy, defining the structure of the vector 2 position x & y, the radius and the colour
    Vector2 position; //vector 2 position of floppy
    int radius; // the raduis of the floppy the circle
    Color color; //floppy will have a colour and this continous structure is being defined within floppy typedef struct
} Floppy;

typedef struct Tubes { //redefine the name of the already exsisiting variable tubes
    Vector2 size; //vector 2 is the sizing of the tubes as a defined size throughout the game
    float topYPosition; //structure of vector top of floating top y position 
    float bottomYPosition; //structure of vector bottom of floating y position
    Color color; //there will be a colour for the tubes and that is being defined at the begining of the code before intialising the window
    bool active; //we are declaring that the booleen is going to be active in moving tubes structure. Boolean variable can hold true or false values.
    float yVariation; //the floating point is storing the floating point numbers in variables. It is storing the variation of tubes in the game
    bool variatingUpwards; // the booleen can either be true or false for the tube to be variating upwards
} Tubes;

typedef struct MovingTubes { //define the variable moving tubes
    Vector2 size; //defining the vector size for the structure of the moving tubes in the game
    float yPosition; //also structuring the float position of the moving tubes to set position within the game
    Color color; //there will be a colour for the tubes and that is being defined at the begining of the code before intialising the window
    bool active; //we are declaring that the booleen is going to be active in moving tubes structure. Boolean variable can hold true or false values.
} MovingTubes;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800; //these are constant variables that cannot be changed throughout the game the screeen width is 800
static const int screenHeight = 450; //constant height of window that appears for player, the const will allow for variable values will be unchangeable and read only

static bool gameOver = false; //static variable with a booleen game over intialised by false
static bool pause = false; //booleen for game to be paused equal to false
static int score = 0; //static int is a variable storing integer value of the score which is declared static, by declaring the variable as static it is there till the end of the code once initialised
static int hiScore = 0; //static int is a variable storing integer value of the hiscore which is declared static, by declaring the variable as static it is there till the end of the code once initialised

static Floppy floppy = { 0 }; //declaring floppy within the game, floppy is initialised to the value of 0 
static Tubes tubes[MAX_TUBES] = { 0 }; //static for max amount of tubes
static MovingTubes movingTubes[MAX_TUBES] = { 0 }; //declaring max tube and global variable of moving tubes, initialised to the value of 0 
static float tubesXPos[MAX_TUBES] = { 0 }; //static float of the tubes for position x will remain in the float position given to position x 
static bool isMovingTube[MAX_TUBES] = { 0 }; //moving tubes is a bool within max tubes and is set to value of 0
static int tubesSpeedX = 0; //static integer for the speed of tubes along the x axis
static float movingTubesSpeedY = 0; //the moving tube speed Y is a static float that will remain the speed given throughout the game
static bool superfx = false; //adding variable bool this can have a true or false value in this case it is equal to false

static Sound collisionSound; //adding static to store collision sound as stores the variable of the collision sound
static Music backgroundMusic; //adding the variable to remain throughout the game, by doing this the static will store the variable for the duration and cannot be changed
static bool audioLoaded = false; //boolean for the audio to be loaded set to false

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) //the function takes no argument by including void, if void isnt included the function can take a number of arguments
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "floppy game 2.0 version"); //initialise screen width and height and title of window

    InitGame(); //initalising game 

#if defined(PLATFORM_WEB) //if defined the platform is the web
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60); //setting the target frame rate to be 60 so objects move witht the speed of the frame and they objects done fly across the screen
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    //  // while the window shouldnt close if player press button to close or select the Escape key
    {
        // Update and Draw Frame
        UpdateDrawFrame();
     
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0; //means that game will execute successfully 
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void) //initalising game by using void it specifies that the function doesnt return a value as void function takes no parameters
{
    floppy.radius = FLOPPY_RADIUS; //initialising floppy raduis
    floppy.position = (Vector2){80, screenHeight/2.0f - floppy.radius}; //screen height divided by the screen height, places floppy in the middle of the screen
    tubesSpeedX = 2; //how fast the tubes move across the screen, this is set to two
    movingTubesSpeedY = 0.75f; //intialising the speed of the moving tubes down, given speed of 0.75f

    for (int i = 0; i < MAX_TUBES; i++) //while the tubes are less than 0, increment 1 up to 100
    {
        tubesXPos[i] = 500 + 280*i; //position of the tubes, say for example this was 800 + 280*1 the cicrle and the tube would be far apart at the beginning when the player starts the game
    }

    for (int i = 0; i < MAX_TUBES; i++) //for loop the tubes are less than 0, increment 1 up to 100. This is a for loop with if and else statement
    {
        if (i % 8 < 4) //the sequence of the tubes changes every 4 times out of 8, so for the first 4 tubes stay the same once player avoids collision with the first 4 tubes it will change to the tubes moving 
        //down the screen and repeat this sequence again (this time by using an if statement)
        {
            tubes[i].topYPosition = -GetRandomValue(0, 120); //indexing and setting a value to i for the top y position of the tube,
            tubes[i].bottomYPosition = 600 + tubes[i].topYPosition - 255; //setting the y position of the botttom and top of the tubes (600 bottom position of tube)(-255 top position of tube)
            tubes[i].size.x = TUBES_WIDTH; //setting the size x tubes to be equal to tubes width
            tubes[i].size.y = 255; //setting the size of the tubes y to be equal to 255

            tubes[i].yVariation = GetRandomValue(-VARIATION_AMOUNT/2, VARIATION_AMOUNT/2); //y variation amount is equal to getting a random value and the variation amount is divided by 2
            tubes[i].variatingUpwards = false; //using a boolean and index the tubes variating upwards to be false

            tubes[i].active = true; //tubes is indexed to an integer [i] which is active. This is a boolean for the tubes set to equal to true
            movingTubes[i].active = false; // moving tubes are indexed and equal to false
            isMovingTube[i] = false; //is tube moving again indexed to a set integer that is a boolean equal to false
        }
        else //this is a for loop that if section above else instruct the code to do the following with variable tubes in game
        {
            movingTubes[i].yPosition = GetRandomValue(0, TUBES_WIDTH + 100); //else moving tubes indexed to integer, the y position of the moving tubes equal to getting a random number(0 with the tubes width plus 100 for the position)
            movingTubes[i].size.x = TUBES_WIDTH; //else statement the moving tubes size x is equal to the tubes width
            movingTubes[i].size.y = MOVING_TUBES_HEIGHT; //the moving tube size y is equal to the tubes height

            movingTubes[i].active = true; //moving tubes is indexed to an integer [i] which is active. This is a boolean for the moving tubes set to equal to true
            tubes[i].active = false; //tubes are indexed and equal to false
            isMovingTube[i] = true; //is tube moving again indexed to a set integer that is a boolean equal to true
        }
    }

    score = 0; //score is equal to zero

    gameOver = false; //game over is a boolean equal to false, the game is not over
    superfx = false; // superfx is equal to false in a boolean, boolean are true or false statements
    pause = false; //pause is equal to a boolean that is false, the game has not been paused

    if (!audioLoaded) { //if audio does not load
        InitAudioDevice(); //intialise audio device

        collisionSound = LoadSound(COLLISION_AUDIO_FILE_PATH); //loading collision sound from the file pathway

        backgroundMusic = LoadMusicStream(BACKGROUD_MUSIC_FILE_PATH); //loading background music, calling file pathway to main cpp file
    backgroundMusic.looping = true; //background music to loop is true play background music
        SetMusicVolume(backgroundMusic, 0.5f); //setting music volume to play at 0.5f and specify the string i.e background music
    PlayMusicStream(backgroundMusic); //extra step to play background music and specify the string

        audioLoaded = true; //if statement using a booleen, for the audio to be loaded is equal to true
    }
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver) //if statement for game not being over
    {
        if (IsKeyPressed('P')) pause = !pause; // if the key p is pressed the game is paused

        if (!pause) //if the game is not paused
        {
            for (int i = 0; i < MAX_TUBES; i++) { //for loop initialised tubes 
                tubesXPos[i] -= tubesSpeedX; //tubes x position at that index is changed to tubes speed x

                tubes[i].yVariation += (tubes[i].variatingUpwards)? -VARIATION_SPEED : VARIATION_SPEED; // Moving the tubes up and down tubes 

                if (tubes[i].yVariation > VARIATION_AMOUNT/2.0f) { // Tube i at y axis is greater than the var amount divide by floating point 2.0
                    tubes[i].variatingUpwards = true; // Then set the value tubes variating upwards to true
                    //movingTubes[i].yPosition -= MOVING_TUBES_HEIGHT + MOVING_TUBES_SPACING;
                } else if (tubes[i].yVariation < -VARIATION_AMOUNT/2.0f) { // if not grater than the amount, divid the variation amount by 2.0 floating point
                    tubes[i].variatingUpwards = false; // then set index value to false
                } 

                movingTubes[i].yPosition += movingTubesSpeedY;// Move descending tubes downwards

                if (movingTubes[i].yPosition > MOVING_TUBES_SPACING) { // If the moving tubes index at y position is greather than the tubes spacing
                    movingTubes[i].yPosition -= MOVING_TUBES_HEIGHT + MOVING_TUBES_SPACING; // then adjust the height and spacing
                }
            }

            if (IsKeyDown(KEY_SPACE) && !gameOver) //if key space is down the game is not over
                floppy.position.y -= 3; //if key is down floppy moves up in position y by 3 while still moving forward in the game
            else
                floppy.position.y += 1; //else if the key is not pressed down floppy moves down in position y by 1
            
            bool collision = false; //creating a boolean to check to see if there is collision and setting the value to false

            
            for (int i = 0; i < MAX_TUBES; i++) // Check Collisions
            {
                if (!isMovingTube[i]) {
                    Rectangle rec; // setting rec to be in a certain point
                    rec.x = tubesXPos[i]; //tubes x position is indexed to a integer  
                    rec.y = tubes[i].topYPosition + tubes[i].yVariation; //rec y is equal to top y position of tubes + the tubes y variation to be added
                    rec.width = tubes[i].size.x; //width of tubes is indexed to a integer being the size of the width of the tubes
                    rec.height = tubes[i].size.y; //height of tubes is indexed to a integer being the size of the height of the tubes

                    collision = collision || CheckCollisionCircleRec(floppy.position, floppy.radius, rec); //checking collision of the circle, radius and position if it collides with rectangle(tubes)

                    rec.y = tubes[i].bottomYPosition + tubes[i].yVariation; //rec y is equal to bottom y position of tubes + the tubes y variation to be added
                    collision = collision || CheckCollisionCircleRec(floppy.position, floppy.radius, rec); //check if the circle has collided with the rec. Specifying the position and radius of floppy

                    if ((tubesXPos[i] < floppy.position.x) && tubes[i].active && !gameOver) //if the tubes position is less than the floppy position x and the parameter of the tubes is active the game is not over

                    {
                        score += 50; //value of the score to be 50, value is going up as 50,100,150 etc
                        tubes[i].active = false; //tubes active false 

                        superfx = true;

                        if (score > hiScore) hiScore = score; //if the score is greater than the high score than the score will become the new high score for the player
                    }
                }

                if (isMovingTube[i]) { //if statement for moving tubes
                    for (int t = 0; t < 3; t++) { //for loop for the integer to be less than 3
                        Rectangle rec;
                        rec.x = tubesXPos[i]; //tubes x position is indexed to a integer  
                        rec.y = movingTubes[i].yPosition + t * (MOVING_TUBES_HEIGHT + MOVING_TUBES_SPACING); //moving tubes y position  plus the integer t multiplied by the moving tube height plus spacing
                        rec.width = movingTubes[i].size.x; //width of moving tubes is indexed to a integer being the size of the width of the moving tubes
                        rec.height = movingTubes[i].size.y; //height of  moving tubes is indexed to a integer being the size of the height of the moving tubes

                        collision = collision || CheckCollisionCircleRec(floppy.position, floppy.radius, rec); //checking the collision of the circle, if floppy position and radius touch the rectangle(tubes)
                    }

                    if ((tubesXPos[i] < floppy.position.x) && movingTubes[i].active && !gameOver) //if the tubes x position if less than floppy position and the moving tubes the game is not over
                    {
                        score += 10; //score for the tubes moving down the screen is set to ten each time player does not collide with moving tube gets 10 points added to score
                        movingTubes[i].active = false; //booleen moving tubes equal to false

                        superfx = true;

                        if (score > hiScore) hiScore = score; //if the score is greater than the high score than the score will become the new high score for the player
                    }
                }
            }

            if (collision) { //setting an if statement for colllision 
                gameOver = true; //if there is a collision then the game is over and collsion is true
                pause = false; // if came is paused the collision is false 

                PlaySound(collisionSound); //collision sound plays if player hits object in game
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER)) //if statement for if the key is pressed the key being enter the game will place
        {
            InitGame(); //initalising game
            gameOver = false; //game is not over if player pressed enter, this is a booleen set to be false 
        }
    }

    if (audioLoaded) { //loading audio again when player wants to start game again
    UpdateMusicStream(backgroundMusic); //updatoing background music and specify the string
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing(); //setup canvas to start drawing

        ClearBackground(SKYBLUE); // Background of window white

        if (!gameOver) //if the game is not over 
        {
            DrawCircle(floppy.position.x, floppy.position.y, floppy.radius, BLUE); //draws a circle, it takes 4 inputs - position y, position x, the radius and the colour

            // Draw tubes
            for (int i = 0; i < MAX_TUBES; i++)
            {
                if (!isMovingTube[i]) { //if tube is moving
                    Rectangle rec;
                    rec.x = tubesXPos[i]; //x position of tube
                    rec.y = tubes[i].topYPosition + tubes[i].yVariation; //y position of tubes & y position of tubes variation
                    rec.width = tubes[i].size.x; //height of tubes is indexed to a integer being the x size of the height of the tubes
                    rec.height = tubes[i].size.y; //height of tubes is indexed to a integer being the y size of the height of the tubes

                    DrawRectangleRec(rec, PINK); //draw rectangle, and colour of rec being pink
                    
                    rec.y = tubes[i].bottomYPosition + tubes[i].yVariation; //parameters of the rec position is y, this is equal to the tube. i is indexed as a number for the bottom y position of
                    // the tubes & the tube y variation is indexed using i
                    DrawRectangleRec(rec, ORANGE); //draw rectangle, and colour of rec being orange
                }

                if (isMovingTube[i]) { //if tube is moving is indexed to a integer
                    for (int t = 0; t < 7; t++) { //setting integer of the tubes moving to be less than 7,for example it was set to 1 as the integer be only 1 tube moving down the window
                        Rectangle rec;
                        rec.x = tubesXPos[i]; // rec x position is equal to tubes x position indexed to a integer
                        rec.y = movingTubes[i].yPosition + t * (MOVING_TUBES_HEIGHT + MOVING_TUBES_SPACING); //moving tubes indexed to integer on y position + t multiplied by the moving tubes height plus moving tubes space
                        rec.width = movingTubes[i].size.x; // rec wdith is equal to moving tubes (indexed using integer i the size of x)
                        rec.height = movingTubes[i].size.y; // rec height is equal to moving tubes (indexed using integer i the size of y)

                        DrawRectangleRec(rec, VIOLET); //draw rectangle, and colour of rec being violet
                    }
                }
            }

        
            if (superfx) // Draw flashing fx (one frame only)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, SKYBLUE); //draw rectangle,that is white and that is the same sixe as the width and height of the screen
                superfx = false; //superfx is equal to false by using a boolean
            }

            DrawText(TextFormat("%04i", score), 20, 20, 40, GREEN); // drawing word score and the score throughout the game, setting colour as gray and the size of box
            DrawText(TextFormat("HI-SCORE: %04i", hiScore), 20, 70, 20, VIOLET); //draw hightest score 

            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, ORANGE); //if the game is paued draw text that says game is paused
        }
        else DrawText("DONT GIVE UP! PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText(" DONT GIVE UP! PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, PURPLE); 
        //if the player loses press enter to play the game again, text appears that says press enter to play again. The positioning of text is divided by the screen height and width. This is a if else statement.
        //draw text & measure text set the size of text to 20

    EndDrawing(); // end the drawing and swap buffers
}


void UnloadGame(void) //executes the variable of the game to be unloaded, the void function is used as it executes the function but does not return the value
{
    
    UnloadSound(collisionSound); //unload collison sound
    UnloadMusicStream(backgroundMusic); //unload background sound
    CloseAudioDevice(); //close the audio device when game is stopped
    audioLoaded = false; //audio not loaded, booleen set to be false
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}



