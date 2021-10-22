// Reaction Time Device Source Code
// ENEL200
// Nicholas Stephens & Ben Rainey & Elliot Baguley

// include libraries
#include <math.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Assign pins for LEDs and Buttons
const int LED_PIN1 = 8;
const int LED_PIN2 = 7;
const int LED_PIN3 = 6;

const int BUTTON_PIN1 = 5;
const int BUTTON_PIN2 = 2;
const int BUTTON_PIN3 = 3;

// Create button arrays
// Expand these arrays and increase BUTTON_NUMBER if more buttons/LEDs are to be added
const int BUTTON_NUMBER = 3;
const int LED_ARRAY [BUTTON_NUMBER] = {LED_PIN1, LED_PIN2, LED_PIN3};
const int BUTTON_ARRAY [BUTTON_NUMBER] = {BUTTON_PIN1, BUTTON_PIN2, BUTTON_PIN3};



// bool variables
bool isStart = false; // bool for whether the reaction time test is started or not
bool isAction = false; //if ongoing action true otherwise false

// int variables for choosing count
int chosenTryCount = 0;
bool isChoosingTry = true;

// int variables for current actions
int currentActionNumber; // detones what action is currently in effect (i.e. what button the person needs to press currently)
int buttonReading; // stores the HIGH/LOW reading of the button
int reactionTimeArray [50] = {}; // array for all of the current reaction times to be stored
int currentReactionTime; // int for storing current reaction time
int actionTotal = 0; // tally of number of reactions done

// variables for storing time in general and start time for an action
unsigned long myTime;
unsigned long startTime;



// Setup function (runs when device is turned on)
void setup() {

  // setup LED and Button pins
  // **** could make this a for loop for code to be nicer/easily modifiable for more buttons/LEDs
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
 
  pinMode(BUTTON_PIN1, INPUT);
  pinMode(BUTTON_PIN2, INPUT);
  pinMode(BUTTON_PIN3, INPUT);

  // set isStart to false 
  isStart = false;
  // set isChoosingTry to false and chosenTryCount to zero
  isChoosingTry = true;
  chosenTryCount = 0;
  
  // setup LCD to turn it on 
  setupLcd();

  // set a random seed for pseudo number generation
  randomSeed(analogRead(A0));
  
  // display "Hold for number of tries" on LCD
  printLCD("Hold for number","of tries");
  
  // Flash LEDs three times as feedback that device has been turned on
  startUpFlash();
  
  // begin Serial and print "DEVICE STARTED" in Serial
  Serial.begin(9600); 
  Serial.println("DEVICE STARTED");
  
}

// function for seting up LCD
void setupLcd() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();                 // turn on lcd backlight 
}

//Takes two strings and prints each one to the upper and lower row on LCD
void printLCD(String row1,String row2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(row1);
  lcd.setCursor(0,1);
  lcd.print(row2);
  

// start up flash function
void startUpFlash() {
  // blink LEDs on and off 3 times when device is turned on
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < BUTTON_NUMBER; j++) {
      digitalWrite(LED_ARRAY[j], HIGH);
    }
    delay(100);
    for(int k = 0; k < BUTTON_NUMBER; k++) {
      digitalWrite(LED_ARRAY[k], LOW);
    }
    delay(100);
  }
}

// function for generating a randomised number between 0 and the number of actions
int randomGenerateAction() {
  // generate and return a number between 0 and the number of actions
  int randomNumber = random(0, BUTTON_NUMBER);
  return (randomNumber);
  
}

// generate a random delay between actions
int randomGenerateDelay() {
  // generate random number between 1000 and 5000 (1-5 seconds)
  int randomNumber = random(1000, 5000);
  return(randomNumber);
}

// function for reading button input
bool readButton() {
  // return true if the current action button has been pressed, false if not
  buttonReading = digitalRead(BUTTON_ARRAY[currentActionNumber]);
  if(buttonReading == HIGH) {
    return(true);
    
  } else {
    return(false);
  }
}

// function for turning on an LED
void turnOnLed(int ledNumber) {
  digitalWrite(ledNumber, HIGH);
}

// function for turning off an LED
void turnOffLed(int ledNumber) {
  digitalWrite(ledNumber, LOW);
}

// function for chosing try count when the device is started up
void chooseTries() {
  
  // check if any button has high reading and set bool isHighButtonReading to true if there is high reading
  bool isHighButtonReading = false;
  for (int i = 0; i < BUTTON_NUMBER; i++) {
    if (digitalRead(BUTTON_ARRAY[i]) == HIGH) {
      isHighButtonReading = true;
    }
  }
  
  //  Add to the try count while button is being held
  if (isHighButtonReading) {
    // set chosen try count to 3 if pressed for first time and display on lcd
    if (chosenTryCount == 0) {
      chosenTryCount = 3;
      printLCD("Try Count : " + String(chosenTryCount),NULL);
      
      delay(1000);
    // increment try count by one (cycle back to 3 if it goes over 30) and display on lcd
    } else {
      chosenTryCount += 1;
      if (chosenTryCount > 30) {
        chosenTryCount = 3;
      }
      printLCD("Try Count : " + String(chosenTryCount),NULL);
      delay(1000);
    }
    
  // when button has stopped being pressed, and there is a chosen try count, set the isChoosingTry to false,
  // display number of tries chosen on lcd and after delay and startup flash, display 'press to start'
  } else if (chosenTryCount != 0) {
    isChoosingTry = false;
    
    printLCD("You chose " + String(chosenTryCount), "tries");
    delay(5000);
    //do startup flashes
    startUpFlash();
    // display "Press to start" on LCD
    printLCD("Press to start",NULL);
    
    
  }
}



// main looping function
void loop() {
  // put your main code here, to run repeatedly:
  
  // 5 ms delay
  // delay(5);

  // set the myTime variable to current arduino clock time
  myTime = micros()/1000;


  if (isChoosingTry == true) {
    chooseTries();
  
  // do this part if the reaction time reading has been started
  } else if (isStart == true) {
    // do this part if a valid action is being expected
    if (isAction == true) {
      
      // do this part if a valid button press is read
      if(readButton() == true) {
        
        // add to action total
        actionTotal += 1;
       
        // set currentReactionTime to the current Arduino time minus Arduino time when action was started
        currentReactionTime = myTime - startTime;

        // add the current reaction time to the reaction time array
        reactionTimeArray[actionTotal-1] = currentReactionTime;
        
        // print current reaction time to serial
        Serial.println("#" + String(actionTotal) + ": Button pressed in " + String(currentReactionTime) + " milliseconds");

        // set isAction to false for a new reaction to start 
        isAction = false;
        // turn off the LED that was on
        turnOffLed(LED_ARRAY[currentActionNumber]);
  
        // run this code to calculate and display average reaction time if the action total has reached chosenTryCount 
        if (actionTotal >= chosenTryCount) {
          
          // add up reaction times and divide by try count for average reaction time
          unsigned long totalReactionTimes = 0;
          for(int i = 0; i < actionTotal; i++) {
            totalReactionTimes += reactionTimeArray[i];
          }
          float averageReactionTime = float(totalReactionTimes) / chosenTryCount;

          // print the average reaction time to the LCD screen
          printLCD("Average time:  ",String(averageReactionTime) +"ms");
          
          // reset the actions to 0, clear the reaction time array, display "RESET" in the serial
          actionTotal = 0;
          memset(reactionTimeArray, 0, sizeof(reactionTimeArray));
          Serial.println("RESET");
          
          
          
          // delay by a second and do start up flash again to signify a reset
          delay(10000);
          startUpFlash();
          
          // display "Hold for number of tries" on LCD
          
          printLCD("Hold for number","of tries");
          isChoosingTry = true;
          chosenTryCount = 0;
          isStart = false;
          
          
        }
  
        // generate a random delay for next action
        delay(randomGenerateDelay());
      }
    } else {
      // this code runs if an action has just finished, i.e. starting of next action
      // set start time to current clock time
      startTime = myTime;
      
      // generate a random action
      currentActionNumber = randomGenerateAction();
     
      // turn on LED associated with that action
      turnOnLed(LED_ARRAY[currentActionNumber]);
      
      // set isAction to true
      isAction = true;
    }
  
  } else {
    // this code runs if the device has just been started up, count received, and starting button has not been pressed

    // check if any button has high reading and set bool isHighButtonReading to true if there is high reading
    bool isHighButtonReading = false;
    for (int i = 0; i < BUTTON_NUMBER; i++) {
      if (digitalRead(BUTTON_ARRAY[i]) == HIGH) {
        isHighButtonReading = true;
      }
    }
    
    
    // if any button is high start the reaction time detections
    if (isHighButtonReading) {

      // set isStart to true, clear LCD and print "Go"
      isStart = true;
      
      printLCD("Go",NULL);
      // generate first random delay
      delay(randomGenerateDelay());
    }
    
  }
  // delay by 5ms
  // delay(5);
}
