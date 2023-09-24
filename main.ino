#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, A4,A5);
 
byte gameBoard[16][80] = {};
int ballUpdateTime = 100;
byte ballXDir = 0;
byte ballYDir = -1;
byte ballX = 7;
byte ballY = 35;
byte ballCharArray[8] = {};
byte playerScore = 0;
byte aiScore = 0;
byte aiPaddlePos = 6;
byte myPaddlePos = 6;


byte aiPaddleArray[2][8] = {0,0,0,0,0,1,1,1};
byte myPaddleArray[2][8] = {0,0,0,0,0,16,16,16};
byte aiPaddleColArray[16] = {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0};
byte myPaddleColArray[16] = {0,0,0,0,0,16,16,16,0,0,0,0,0,0,0,0};
const int moveRightButton = 8;
const int moveLeftButton = 7;
int rightButtState = 0;
int leftButtState = 0;
int rightLastButtState = 0;
int leftLastButtState = 0;

void ClearPaddles(){
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 8; j++){
      aiPaddleArray[i][j] = 0;
      myPaddleArray[i][j] = 0;
    }
  }
}

void PrintStuff(){
  lcd.clear();

  byte* mPTemp1 = myPaddleArray[0];
  byte* mPTemp2 = myPaddleArray[1];
  byte* aiPTemp1 = aiPaddleArray[0];
  byte* aiPTemp2 = aiPaddleArray[1];

  lcd.createChar(0, mPTemp1);
  lcd.createChar(1, mPTemp2);
  lcd.createChar(14, aiPTemp1);
  lcd.createChar(15, aiPTemp2);

  lcd.setCursor(14, 0);
  lcd.write(byte(0));

  lcd.setCursor(14, 1);
  lcd.write(byte(1));

  lcd.setCursor(1, 0);
  lcd.write(byte(14));

  lcd.setCursor(1, 1);
  lcd.write(byte(15));

  byte charNum = 0;

  if((ballY > 9) && (ballY < 70)){
    byte LCDCol = ballY / 5;
    byte LCDRow = (ballX <= 7) ? 0 : 1;
    charNum = ballY / 5;
    lcd.createChar(charNum, ballCharArray);
    lcd.setCursor(LCDCol,LCDRow);
    lcd.write(byte(charNum));
  }
}

void PrintPaddles(){
  byte* mPTemp1 = myPaddleArray[0];
  byte* mPTemp2 = myPaddleArray[1];
  byte* aiPTemp1 = aiPaddleArray[0];
  byte* aiPTemp2 = aiPaddleArray[1];

  lcd.createChar(0, mPTemp1);
  lcd.createChar(1, mPTemp2);
  lcd.createChar(14, aiPTemp1);
  lcd.createChar(15, aiPTemp2);

  lcd.setCursor(14, 0);
  lcd.write(byte(0));

  lcd.setCursor(14, 1);
  lcd.write(byte(1));

  lcd.setCursor(1, 0);
  lcd.write(byte(14));

  lcd.setCursor(1, 1);
  lcd.write(byte(15));
}

void SetupPaddles(){
  ClearPaddles();
  myPaddleArray[0][5] = 16;
  myPaddleArray[0][6] = 16;
  myPaddleArray[0][7] = 16;
  aiPaddleArray[0][5] = 1;
  aiPaddleArray[0][6] = 1;
  aiPaddleArray[0][7] = 1;
  PrintPaddles();
}

void MovePaddleUp(){
  if(myPaddlePos != 1){
    myPaddlePos--;
    aiPaddlePos--;
    UpdatePaddlesAfterMove();
  }  
}

void MovePaddleDown(){
  if(myPaddlePos != 14){
    myPaddlePos++;
    aiPaddlePos++;
    UpdatePaddlesAfterMove();
  }  
}

void UpdatePaddlesAfterMove(){
  for(int i = 0; i < 16; i++){
      if((i == (myPaddlePos-1)) || (i == myPaddlePos) || (i == (myPaddlePos+1))){
        myPaddleColArray[i] = 16;
        aiPaddleColArray[i] = 1;
      } else {
        myPaddleColArray[i] = 0;
        aiPaddleColArray[i] = 0;
      }
    }
    for(int j = 0; j < 8; j++){
      myPaddleArray[0][j] = myPaddleColArray[j];
      aiPaddleArray[0][j] = aiPaddleColArray[j];
    }
    for(int k = 8; k < 16; k++){
      myPaddleArray[1][k-8] = myPaddleColArray[k];
      aiPaddleArray[1][k-8] = aiPaddleColArray[k];
    }
}

int GetLEDRowValue(byte ledRow, byte maxColumn){ 
  int minColumn = maxColumn - 4;
  int ledValue = 0; 

  int multiplier = 1;
  for(int i = maxColumn; i >= minColumn; i--){
    ledValue += (gameBoard[ledRow][i] * multiplier);
    multiplier *= 2; 
  }
  return ledValue;
}

void GenerateBallArray(){
  byte maxCol = ((ballY / 5) * 5) + 4;
  byte minCol = maxCol - 4;
  byte startRow = (ballX <= 7) ? 0 : 8;
  byte endRow = startRow + 8;
  if(startRow == 0){
    for(int i = startRow; i < endRow; i++){
      ballCharArray[i] = GetLEDRowValue(i, maxCol);
    }
  } else {
    for(int i = startRow; i < endRow; i++){
      ballCharArray[i-8] = GetLEDRowValue(i, maxCol);
    }
  }
}

void SetupBall(){
  ballYDir *= -1;
  gameBoard[7][35] = true;
 
}

void AwardAPoint(){
  if(ballY <= 8){
    playerScore++;
  } else {
    aiScore++;
  }
  delay(100);
  ballYDir *= -1;
}

void UpdateBall(){
  delay(ballUpdateTime);
  if((ballY <= 8) || (ballY >= 71)){
    AwardAPoint();
  } else if((ballX == 0) || (ballX == 15)){
    ballXDir *= -1;
  } else if((ballY == 69) && (ballX == myPaddlePos)){
    Serial.println("MIDDLE\n");
    ballYDir *= -1;
  } else if((ballY == 69) && (ballX == (myPaddlePos + 1))){
    Serial.println("BOTTOM\n");
    ballYDir *= -1;
    ballXDir = 1;
  } else if((ballY == 69) && (ballX == (myPaddlePos - 1))){
    Serial.println("TOP\n");
    ballYDir *= -1;
    ballXDir = -1;
  }
  gameBoard[ballX][ballY] = false;
  ballX += ballXDir;
  ballY += ballYDir;
  gameBoard[ballX][ballY] = true;
  GenerateBallArray();
  
  PrintStuff();
}


void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.backlight();
  lcd.init();

  pinMode(moveRightButton, INPUT_PULLUP);
  pinMode(moveLeftButton, INPUT_PULLUP);

  SetupBall();
  GenerateBallArray();
  SetupPaddles();
  PrintStuff();
}

void loop() {
  UpdateBall();
  rightButtState = digitalRead(moveRightButton);
  leftButtState = digitalRead(moveLeftButton);

  if(rightButtState != rightLastButtState){
    if(rightButtState == HIGH){

      MovePaddleUp();
      PrintStuff();
    }
    delay(50);
    rightLastButtState = rightButtState;
  }

  if(leftButtState != leftLastButtState){
    if(leftButtState == HIGH){

      MovePaddleDown();
      PrintStuff();
    }
    delay(50);
    leftLastButtState = leftButtState;
  }

}
