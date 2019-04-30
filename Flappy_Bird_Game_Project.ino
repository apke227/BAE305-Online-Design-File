
// includes refrences to libraries supporting the LCD(UTFT) screen as well as touch screen capabilities (UTR) and storing values for score (EEPROM)
#include <UTFT.h> 
#include <URTouch.h>
#include <EEPROM.h>

//Create Objects for the LCD screen as well as the touch screen
UTFT    myGLCD(ILI9341_16,38,39,40,41); 
URTouch  myTouch( 6, 5, 4, 3, 2);

extern unsigned int bird01[0x41A]; // uploaded bitmap of the bird
int x, y; // makes variables of interest x and y, as referenced later on

int xP = 212;// initializes x coordinate of pillars
int yP = 74; // initializes y coordinate of pillars
int yB = 25; // initializes y coordinate of the bird
int movingRate = 5; // sets initial velocity of the bird to 5
int fallRateInt = 0; // sets initial falling rate to be an unchanging 0 (bird does not move until screen is pressed)
float fallRate = 0; // sets the intermittant fall rate to be a floating value
int score = 0; // sets initial score to 0
int lastSpeedUpScore = 0; // sets the last score value
int highestScore;
boolean screenPressed = false;
boolean gameStarted = false;

void setup() {
  // Initiate display, set up the TFT touch screen
  myGLCD.InitLCD(); // initiates LCD capabilities of the screen
  myGLCD.clrScr(); // clears the LCD screen
  myTouch.InitTouch(); // initiates touch scfeen
  myTouch.setPrecision(PREC_MEDIUM); // sets precision of the touch feature of the touch screen to medium
  
  highestScore = EEPROM.read(0); // Read the highest score stored in the EEPROM Library, initially to be 0
  
  initiateGame(); // initiate game custom function
}

void loop() {
    xP=xP-movingRate; // xP - x coordinate of the pilars; range: 319 - (-51)   
    drawPilars(xP, yP); // Draws the pillars 
    
    // yB - y coordinate of the bird which depends on value of the fallingRate variable
    yB+=fallRateInt; 
    fallRate=fallRate+0.4; // Each inetration the fall rate increases by an addition of 0.4 so that we can the effect of acceleration/ gravity
    fallRateInt= int(fallRate);
    
    // Checks for collision between the pillars and the bird
    if(yB>=180 || yB<=0){ // top and bottom
      gameOver();
    }
    if((xP<=74) && (xP>=5) && (yB<=yP-2)){ // bird hits the upper pillar
      gameOver();
    }
    if((xP<=74) && (xP>=5) && (yB>=yP+60)){ // bird hits the power pillar lower pillar
      gameOver();
    }
    
    // Draws the bird, custom function to read in the .csv file to a bitmap
    drawBird(yB);

    // After the pillar has passed through the screen, redraws the pillars for the next screen
    if (xP<=-51){
      xP=212; // Resets xP to 319
      yP = rand() % 100+20; // Random number for the pillars height
      score++; // Increase score by one for each pillar successfully completed
    }
    // Control the bird
    if (myTouch.dataAvailable()&& !screenPressed) {
       fallRate=-6; // Setting the fallRate as a negative number allows the bird to experience the effect of "jumping" while the screen is pressed. (i.e the bird falls in the opposite 
                    // direction as that defined as positive, at the specified rate
       screenPressed = true;
    }
    // Can not hold the screen, it needs to be pressed (touched and released)
    else if ( !myTouch.dataAvailable() && screenPressed){
      screenPressed = false;
    }
    
    // Completing 5 successful pillars increases the moving rate of the pillars
    if ((score - lastSpeedUpScore) == 5) {
      lastSpeedUpScore = score;
      movingRate++; 
    }
}
// initiateGame - Custom Function. sets the colors of the background and pillars based on the RGB color codes
void initiateGame() {
  myGLCD.clrScr();
  // Blue background
  myGLCD.setColor(114, 198, 206); // sets the colors
  myGLCD.fillRect(0,0,319,239); // sets the outlines of the rectangles
  // Ground
  myGLCD.setColor(221,216,148); // sets grass color
  myGLCD.fillRect(0, 215, 319, 239); // fills grass color
  myGLCD.setColor(47,175,68); // sets dirt color
  myGLCD.fillRect(0, 205, 319, 214); // fills dirt color
  // Text
  myGLCD.setColor(0, 0, 0); // sets font to white
  myGLCD.setBackColor(221, 216, 148);
  myGLCD.setFont(BigFont); // sets font size for score
  myGLCD.print("Score:",5,220); // displays the score at these coordinates on the screen
  myGLCD.setFont(SmallFont); // sets font size for highest score
  myGLCD.setColor(0, 0, 0); // sets font color to white
  myGLCD.setBackColor(114, 198, 206); 
  myGLCD.print("Highest Score: ",5,5);// displays the highest score at these x and y positions
  myGLCD.printNumI(highestScore, 120, 6); // prints the value of the highest score
  myGLCD.print(">RESET<",255,5); // prints the option of a reset button 
  myGLCD.drawLine(0,23,319,23);
  myGLCD.print("TAP TO START",CENTER,100); // displays this in the center of the screen
  
  drawBird(yB); // Draws the bird, custom function
  
  // Wait until the screen is tapped to initialize
  while (!gameStarted) {
    if (myTouch.dataAvailable()) {
    myTouch.read();
    x=myTouch.getX();// gets x coordinate
    y=myTouch.getY();  // gets y coordinate      
    // Resets higest score when highest score button is pressed
    if ((x>=250) && (x<=212 &&(y>=0) && (y<=25)) {
    highestScore = 0; 
    myGLCD.setColor(114,198,206);
    myGLCD.fillRect(120, 0, 150, 22);
    myGLCD.setColor(0, 0, 0);
    myGLCD.printNumI(highestScore, 120, 5);
    } 
    if ((x>=0) && (x<=319) &&(y>=30) && (y<=239)) {
    gameStarted = true; // if the screen is pressed and the game is initialized
    myGLCD.setColor(114, 198, 206);
    myGLCD.fillRect(0, 0, 319, 32);
    }   
  }
  }
  // Clears the "TAP TO START" text before the game starts
  myGLCD.setColor(114, 198, 206);
  myGLCD.fillRect(85, 100, 235, 116);
  
}
// drawPlillars - Custom function
void drawPilars(int x, int y) {
    if (x>=270){
      myGLCD.setColor(0, 200, 20); // sets color to green
      myGLCD.fillRect(318, 0, x, y-1); // draws dinensions in the x direction to the left of the rectangle centerline
      myGLCD.setColor(0, 0, 0); // sets color to white
      myGLCD.drawRect(319, 0, x-1, y); // draws rectangle dimensions to the right of the centerline

      myGLCD.setColor(0, 200, 20); // sets color to green
      myGLCD.fillRect(318, y+81, x, 203); // draws dimensions of rectangle in the y direction
      myGLCD.setColor(0, 0, 0); // sets color to white
      myGLCD.drawRect(319, y+80, x-1, 204); // draws rectangle to the left of the centerline
    }
    else if( x<=268) {
      // Draws blue rectangle to the right of the pillars
      myGLCD.setColor(114, 198, 206);
      myGLCD.fillRect(x+51, 0, x+60, y);
      // Draws the pillar
      myGLCD.setColor(0, 200, 20); // sets color of pillar
      myGLCD.fillRect(x+49, 1, x+1, y-1); // fills the color of the pillar
      // Draws the black frame of the pillar
      myGLCD.setColor(0, 0, 0); // sets color to white
      myGLCD.drawRect(x+50, 0, x, y);
      // Draws the blue rectangle left of the pillar
      myGLCD.setColor(114, 198, 206);
      myGLCD.fillRect(x-1, 0, x-3, y);

      // The bottom pillar. Repeat a similar process to the top portion
      myGLCD.setColor(114, 198, 206);
      myGLCD.fillRect(x+51, y+80, x+60, 204);
      myGLCD.setColor(0, 200, 20);
      myGLCD.fillRect(x+49, y+81, x+1, 203);
      myGLCD.setColor(0, 0, 0);
      myGLCD.drawRect(x+50, y+80, x, 204);
      myGLCD.setColor(114, 198, 206);
      myGLCD.fillRect(x-1, y+80, x-3, 204);
  }
  // Draws the score
  myGLCD.setColor(0, 0, 0); // displays the text in white
  myGLCD.setBackColor(221, 216, 148); // sets background color
  myGLCD.setFont(BigFont); // sets font size
  myGLCD.printNumI(score, 100, 220); // prints the score value
}

//drawBird() - custom function
void drawBird(int y) {
  // Draws the bird - converts the .csv to a bitmap
  myGLCD.drawBitmap (50, y, 35, 30, bird01); // draws bitmap  
  // Draws blue rectangles above and below the bird.as it moves to clear its previus state
  myGLCD.setColor(114, 198, 206);
  myGLCD.fillRoundRect(50,y,85,y-6);
  myGLCD.fillRoundRect(50,y+30,85,y+36);
}
//gameOver() - Custom Function
void gameOver() {
  delay(3000); // 3 second delay between the end of the game and when the reset screen shows up
  // Clears the screen and prints the text
  myGLCD.clrScr(); // clears the screen
  myGLCD.setColor(255, 255, 255); // sets the font color to black
  myGLCD.setBackColor(0, 0, 0); // sets background color to white
  myGLCD.setFont(BigFont); // sets font size
  myGLCD.print("GAME OVER", CENTER, 40); // displays game over in the center of the screen at this y position
  myGLCD.print("Score:", 100, 80); // displays the word scpre at this position on the screen
  myGLCD.printNumI(score,200, 80); // displays the value for score stored  in the EEPROM library
  myGLCD.print("Restarting...", CENTER, 120); // displays the wors restarting in the center of the screen at this y position
  myGLCD.setFont(SevenSegNumFont);// sets font style type
  myGLCD.printNumI(2,CENTER, 150); // starts a countdown. displays the number 2 in the center at this y position on the screen
  delay(1000); // waits 1 second
  myGLCD.printNumI(1,CENTER, 150); // clears the number and replaces it with the number 1 in the same position on the screen
  delay(1000); // delays 1 second
  
  // Writes the highest score in the EEPROM
  if (score > highestScore) {
    highestScore = score; // takes the value of score and keeps it as the value to store
    EEPROM.write(0,highestScore); // stores the value
  }
  // Resets the variables to start position values
  xP=212;
  yB=74;
  fallRate=0;
  score = 0;
  lastSpeedUpScore = 0;
  movingRate = 5;  
  gameStarted = false;
  // Restart game, initiates the initiate game function again
  initiateGame();
}
