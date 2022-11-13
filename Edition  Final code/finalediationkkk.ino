/*

  Name : Mahmoud Elmokadem

  ID\ 2701566

*/
//--> include Libraries
#include "devboard.h"
//-->define switches buttons
#define SW1_PIN       10
#define SW2_PIN       9
//--> define string messages
String error_massage1[5] = {"E1:Pump", "E2:Pipe ", "E3:isolation ", "E4:contamination", "E5:Unknown "};
String error_massage2[5] = {"malfunction", "burst detected ", "Value closed", "detected", "error"};
//====================================
int pump_speed ;
int ledredlow = 1;
int ledyellowhigh = 2;
//--> Defining Inteurption const
const byte ledPin = 12;
const byte interruptPin = 2;
volatile byte state = HIGH ;
//-->function Prototypes======
void cutdown() ;
void interption();
void Mpumpspeed();
void mahmoud_reset();
int initialinput;
//--> TANK PART SIMU ***************
//simulation time============================================
const int simulationTime = 2000;
//water demand pattern in l/s
float demand[24] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24}; //dummy water demand pattern
//-->define tank data structure
typedef struct {
  int diameter = 8; //[m]
  int height = 5; //[m]
  float area = 3.14159 * pow(diameter / 2, 2); //cross-sectional area, pi*r^2 [m^2]
  int initialLvl; //[%]
  float currentLvl; //[m]
  float lastLvl; //[m]
  float minLevel = 0.1 * height; //min level set to 10%
  float maxLevel = 0.85 * height; //max level set to 85%
} tank;

tank myTank;      //declare variable of tank type#
const int dt = 3600; //real-world time interval
bool flag = false;
//--> starting the setup code......
void setup() {
  DEVBOARD_ledYellowOff();
  DEVBOARD_ledRedOff();
  DEVBOARD_ledGreenOff();
  pinMode(12, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interption, CHANGE);
  //  pinMode(ledyellowhigh, OUTPUT);
  //-->Setup the switch button As INPUT
  pinMode(SW1_PIN, INPUT);
  pinMode(SW2_PIN, INPUT);
  // system leds states ON or OFF
  Serial.begin(115200); // initialize the serial communication:
  DEVBOARD_init();
  DEVBOARD_lcdClear();
  DEVBOARD_lcdSetCursor(0, 0);
  DEVBOARD_lcdPrintString("Pump Control System");
  delay(1000);
  // initilasation
  DEVBOARD_lcdClear();
  DEVBOARD_lcdSetCursor(0, 0);
  DEVBOARD_lcdPrintString("Initialisation");
  delay(1000);
  DEVBOARD_lcdClear();
  DEVBOARD_lcdSetCursor(0, 0);
  DEVBOARD_lcdPrintString("System Ready");
  if (flag)interption_happen();
}

void loop() {
  digitalWrite(ledPin, state);
  if (!flag) {
    if (flag)interption_happen();
    DEVBOARD_lcdClear();
    DEVBOARD_lcdSetCursor(0, 0);
    DEVBOARD_lcdPrintString("Manual Press 1 ");
    DEVBOARD_lcdSetCursor(0, 1);
    DEVBOARD_lcdPrintString("Automatic Press2 ");
    //read the input data from the user
    byte user_input ;
    user_input = DEVBOARD_keypadRead();

    //=========================================
    //=====Manual Mode========================
    //=========================================
    if (user_input == 1) {

      while (user_input == 1) {
        DEVBOARD_writeLedBank(B10000000);
        if (flag)interption_happen();
        myTank.initialLvl = 75 ; //set initial tank level to 50%
        DEVBOARD_lcdClear();
        DEVBOARD_lcdSetCursor(0, 0);
        DEVBOARD_lcdPrintString("Manual Mode");
        DEVBOARD_lcdSetCursor(2, 1);
        DEVBOARD_lcdPrintString("System Ready");
        DEVBOARD_lcdClear();
        DEVBOARD_lcdSetCursor(5, 0);
        DEVBOARD_lcdPrintString("S1/START");
        DEVBOARD_lcdSetCursor(5, 1);
        DEVBOARD_lcdPrintString("S2/STOP");
        if (flag)interption_happen();

        if (digitalRead(SW1_PIN) == LOW) {
          cutdown();
          for (int i = 0; i < 24; i++) //simulate system over 24 hours period
          {
            if (flag)interption_happen(); // checking  the Inturreption

            if (digitalRead(SW2_PIN) == LOW) // Using S2 to Break the loop with S2
            {
              //the turn the D5 red LED ON ---
              DEVBOARD_writeLedBank(B10000000);
              // ----
              DEVBOARD_lcdClear();
              DEVBOARD_lcdSetCursor(3, 0);
              DEVBOARD_lcdPrintString("System STOP");
              DEVBOARD_lcdSetCursor(3, 1);
              DEVBOARD_lcdPrintString("System Restart");
              mahmoud_reset();
              cutdown();
              return setup();
            }
            // The manual Mode
            int  analogValue = DEVBOARD_readPotentiometer();
            // obtain reading from 0-1023 to 0-240 RPM constrians by Using Map function
            byte    analogValue1 = map(analogValue, 1, 1020, 10, 240);
            DEVBOARD_driveMotor(analogValue1);
            //function to show the PUMP speed
            Mpumpspeed(analogValue1);

            Serial.print("Time:"); Serial.print(i); Serial.print("h"); Serial.print(", ");
            //AT MANUAL MODE
            //set a dummy flow  FUNCTION -----------------------
            float q = 0.082 * (analogValue1) + 5.6678;

            Serial.print("Pump flow: "); Serial.print(q, 4); Serial.print(" l/s");
            Serial.print(", ");
            /* END PUMP CONTROL */

            // TANK DYNAMICS SIMULATION
            if (i == 0) {                                                                                                         //account for intial condition
              myTank.currentLvl = float(myTank.initialLvl) / 100 * myTank.height + 1 / myTank.area * ((q - demand[i]) / 1000) * dt ; //tank level in [m], 1000 is to convert l/s to m^3/s
            }
            else
            {
              myTank.currentLvl = myTank.lastLvl + 1 / myTank.area * ((q - demand[i]) / 1000) * dt; //tank level in [m]
            }

            myTank.lastLvl = myTank.currentLvl; //store current level


            Serial.print("Tank level: "); Serial.println(myTank.currentLvl, 4);
            /* END TANK DYNAMICS SIMULATION*/
            int mahmoud = ((myTank.currentLvl * 5));
            // to monitior the tank capicity level on RED LED and Yello LED
            if (myTank.currentLvl < myTank.minLevel)
            {
              DEVBOARD_ledRedOn();
              DEVBOARD_ledYellowOff();
            }
            else if (myTank.currentLvl > myTank.maxLevel ) {
              DEVBOARD_ledRedOff();
              DEVBOARD_ledYellowOn();
            }

            int tankprecent = ((myTank.currentLvl * 5));
            //DISPALY VALUES ON LCD  *
            DEVBOARD_lcdClear();
            DEVBOARD_lcdSetCursor(0, 0);
            DEVBOARD_lcdPrintString("M");
            DEVBOARD_lcdSetCursor(1, 0);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(2, 0);
            DEVBOARD_lcdPrintString("M");
            DEVBOARD_lcdSetCursor(3, 0);
            DEVBOARD_lcdPrintString("");
            DEVBOARD_lcdSetCursor(4, 0);
            DEVBOARD_lcdPrintString("T");
            DEVBOARD_lcdSetCursor(5, 0);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(6, 0);
            DEVBOARD_lcdPrintInt(i);
            DEVBOARD_lcdSetCursor(8, 0);
            DEVBOARD_lcdPrintString("h");
            DEVBOARD_lcdSetCursor(9, 0);
            DEVBOARD_lcdPrintString("");
            DEVBOARD_lcdSetCursor(11, 0);
            DEVBOARD_lcdPrintString("C");
            DEVBOARD_lcdSetCursor(12, 0);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(13, 0);
            DEVBOARD_lcdPrintInt(tankprecent);
            DEVBOARD_lcdSetCursor(15, 0);
            DEVBOARD_lcdPrintString("%");
            DEVBOARD_lcdSetCursor(0, 1);
            DEVBOARD_lcdPrintString("P");
            DEVBOARD_lcdSetCursor(1, 1);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(2, 1);
            DEVBOARD_lcdPrintInt(analogValue1);
            DEVBOARD_lcdSetCursor(5, 1);
            DEVBOARD_lcdPrintString("RPM,");
            DEVBOARD_lcdSetCursor(9, 1);
            DEVBOARD_lcdPrintString("D:");
            DEVBOARD_lcdSetCursor(11, 1);
            DEVBOARD_lcdPrintInt(int(demand[i]));
            DEVBOARD_lcdSetCursor(13, 11);
            DEVBOARD_lcdPrintString("L/S");
            //   END DISPALY VALUES ON LCD
            delay(simulationTime);
          }
          // system cutdown and Restart
          cutdown();
          return setup();
        }
        else if (digitalRead(SW2_PIN) == LOW)
        {

          // to turn the D5 red led to show the user the system is OFF
          DEVBOARD_writeLedBank(B10000000);
          //clear the screen
          DEVBOARD_lcdClear();
          DEVBOARD_lcdSetCursor(3, 0);
          DEVBOARD_lcdPrintString("System STOP");
          cutdown();
          return setup();
        }
        delay(500) ;
      }
      delay(500) ;
    }

    //=====================================================================
    //Automatic MODE ======================================================
    //=====================================================================
    else if (user_input == 2) {
      DEVBOARD_writeLedBank(B10000000);
      DEVBOARD_lcdClear();
      DEVBOARD_lcdSetCursor(0, 0);
      DEVBOARD_lcdPrintString("Automatic Mode ");
      DEVBOARD_lcdSetCursor(1, 0);
      delay(500);
      DEVBOARD_lcdClear();
      DEVBOARD_lcdSetCursor(0, 0);
      DEVBOARD_lcdPrintString("Enter intial level");
      DEVBOARD_lcdSetCursor(0, 1);
      DEVBOARD_lcdPrintString("10% to 85%");
      initialinput = DEVBOARD_keypadReadInt(0);
      if (10 < initialinput , initialinput < 85) {
        myTank.initialLvl = initialinput; //user input the inaial level of the tank
      }
      else
      {
        DEVBOARD_lcdClear();
        DEVBOARD_lcdSetCursor(0, 0);
        DEVBOARD_lcdPrintString("Enter value");
        DEVBOARD_lcdSetCursor(0, 1);
        DEVBOARD_lcdPrintString("10% to 85%");
        delay(500);
        return setup();
      }
      DEVBOARD_lcdClear();
      DEVBOARD_lcdPrintString("System Ready ");
      delay(500);
      while (user_input == 2) {
        DEVBOARD_lcdClear();
        DEVBOARD_lcdSetCursor(5, 0);
        DEVBOARD_lcdPrintString("S1/START");
        DEVBOARD_lcdSetCursor(5, 1);
        DEVBOARD_lcdPrintString("S2/STOP ");

        if (digitalRead(SW1_PIN) == LOW) {
           cutdown();
          for (int i = 0; i < 24; i++) //simulate system over 24 hours period
          {
            DEVBOARD_writeLedBank(B00100000);
            // checking  the Inturreption
            if (flag)interption_happen();
            // Using S2 to Break the loop with S2
            if (digitalRead(SW2_PIN) == LOW)
            {
              DEVBOARD_lcdClear();
              DEVBOARD_lcdSetCursor(0, 0);
              DEVBOARD_lcdPrintString("System STOP");
              mahmoud_reset();
              cutdown();
              return setup();
            }

            int  tankcontrolprecent = (myTank.currentLvl * 5);
            Serial.print(tankcontrolprecent);
            float q ;
            if (tankcontrolprecent < 10)
            {
              pump_speed = 240;
              DEVBOARD_driveMotor(pump_speed);

            }
            else if (tankcontrolprecent > 85)
            {
              pump_speed = 10;
              DEVBOARD_driveMotor(pump_speed);

            }
            // END of Bang-bang controller=====================================================
            Mpumpspeed(pump_speed);
            pump_speed ;
            Serial.print("Time:"); Serial.print(i); Serial.print("h"); Serial.print(", ");

            //set a dummy flow  FUNCTION -----------------------
            q = 0.082 * (pump_speed) + 5.6678;

            Serial.print("Pump flow: "); Serial.print(q, 4); Serial.print(" l/s");
            Serial.print(", ");
            /* END PUMP CONTROL */

            /* TANK DYNAMICS SIMULATION*/
            if (i == 0) { //account for intial condition
              myTank.currentLvl = float(myTank.initialLvl) / 100 * myTank.height + 1 / myTank.area * ((q - demand[i]) / 1000) * dt; //tank level in [m], 1000 is to convert l/s to m^3/s
            }
            else {
              myTank.currentLvl = myTank.lastLvl + 1 / myTank.area * ((q - demand[i]) / 1000) * dt; //tank level in [m]
            }

            myTank.lastLvl = myTank.currentLvl; //store current level

            if (myTank.currentLvl < myTank.minLevel)
            {
              DEVBOARD_ledRedOn();
              DEVBOARD_ledYellowOff();
            }
            else if (myTank.currentLvl > myTank.maxLevel) {
              DEVBOARD_ledRedOff();
              DEVBOARD_ledYellowOn();
            }
            Serial.print("Tank level: "); Serial.println(myTank.currentLvl, 4);
            int  tankprecent = (myTank.currentLvl * 5);
            /* END TANK DYNAMICS SIMULATION*/

            /* DISPALY VALUES ON LCD  */
            //This example displays demand value on the LCD
            DEVBOARD_lcdClear();
            DEVBOARD_lcdSetCursor(0, 0);
            DEVBOARD_lcdPrintString("A");
            DEVBOARD_lcdSetCursor(1, 0);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(2, 0);
            DEVBOARD_lcdPrintString("M");
            DEVBOARD_lcdSetCursor(3, 0);
            DEVBOARD_lcdPrintString("");
            DEVBOARD_lcdSetCursor(4, 0);
            DEVBOARD_lcdPrintString("T");
            DEVBOARD_lcdSetCursor(5, 0);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(6, 0);
            DEVBOARD_lcdPrintInt(i);
            DEVBOARD_lcdSetCursor(8, 0);
            DEVBOARD_lcdPrintString("h");
            DEVBOARD_lcdSetCursor(9, 0);
            DEVBOARD_lcdPrintString("");
            DEVBOARD_lcdSetCursor(11, 0);
            DEVBOARD_lcdPrintString("C");
            DEVBOARD_lcdSetCursor(12, 0);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(13, 0);
            DEVBOARD_lcdPrintInt(tankprecent);
            DEVBOARD_lcdSetCursor(15, 0);
            DEVBOARD_lcdPrintString("%");
            DEVBOARD_lcdSetCursor(0, 1);
            DEVBOARD_lcdPrintString("P");
            DEVBOARD_lcdSetCursor(1, 1);
            DEVBOARD_lcdPrintString(":");
            DEVBOARD_lcdSetCursor(2, 1);
            DEVBOARD_lcdPrintInt(pump_speed);
            DEVBOARD_lcdSetCursor(5, 1);
            DEVBOARD_lcdPrintString("RPM,");
            DEVBOARD_lcdSetCursor(9, 1);
            DEVBOARD_lcdPrintString("D:");
            DEVBOARD_lcdSetCursor(11, 1);
            DEVBOARD_lcdPrintInt(int(demand[i]));
            DEVBOARD_lcdSetCursor(13, 11);
            DEVBOARD_lcdPrintString("L/S");
            /* END DISPALY VALUES ON LCD  */

            delay(simulationTime);
          }
          cutdown();
          return setup();
        }

        else if (digitalRead(SW2_PIN) == LOW) {

          // to turn the D5 red led to show the user the system is OFF
          DEVBOARD_writeLedBank(B10000000);
          //clear the screen
          DEVBOARD_lcdClear();
          DEVBOARD_lcdSetCursor(3, 0);
          DEVBOARD_lcdPrintString("System STOP");
          cutdown();
          return setup();
        }
        delay(500);
      }
      delay(500);
    }
    exit(0) ;
  }
  delay(1000);
}
//My Functions
void interption() {

  flag = true ;

}
//=========inturreption Function=================
void interption_happen()
{
  //turn off all LEDs
  DEVBOARD_ledYellowOff();
  DEVBOARD_ledRedOff();
  DEVBOARD_ledGreenOff();

  DEVBOARD_driveMotor(0);
  DEVBOARD_lcdClear();
  // Displaying E in 7 segment
  byte characterhint = (SEG_A | SEG_D | SEG_E | SEG_F | SEG_G);
  DEVBOARD_sevenSegment(characterhint);
  /// display rendom number from 0 to 5 on LCD Screen --------------------------------------------
  //first messg part 1
  
  int random_int = random(1,5);
  String msg = error_massage1[random_int];
  DEVBOARD_lcdSetCursor(0, 0);
  DEVBOARD_lcdPrintString(&msg[0]);
  //second messg part 2
  String msg2 = error_massage2[random_int];
  DEVBOARD_lcdSetCursor(0, 1);
  DEVBOARD_lcdPrintString(&msg2[0]);
  // showing the error messages on serial port
  Serial.print(msg);
  Serial.print(msg2);
  // to make the LED Blink
  for (int h = 0; h < 100 ;  h++) {
    DEVBOARD_writeLedBank(B01000000);
    delay (50);
    DEVBOARD_writeLedBank(B00000000);
    delay(50) ;
    DEVBOARD_soundBuzzer();
    delay(50);
  }
  exit(0);
}

void mahmoud_reset() {
  DEVBOARD_driveMotor(0);
}
void cutdown()
{
  byte characterh_9 = (SEG_A | SEG_D | SEG_C | SEG_F | SEG_G | SEG_B);
  DEVBOARD_sevenSegment(characterh_9);
  delay(1000);
  byte characterh_8 = (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G);
  DEVBOARD_sevenSegment(characterh_8);
  delay(1000);
  byte characterh_7 = (SEG_A | SEG_B | SEG_C);
  DEVBOARD_sevenSegment(characterh_7);
  delay(1000);
  byte characterh_6 = (SEG_F | SEG_G | SEG_E | SEG_D | SEG_C);
  DEVBOARD_sevenSegment(characterh_6);
  delay(1000);
  byte characterh_5 = (SEG_A | SEG_F | SEG_G | SEG_C | SEG_D);
  DEVBOARD_sevenSegment(characterh_5);
  delay(1000);
  byte characterh_4 = (SEG_F | SEG_G | SEG_B | SEG_C);
  DEVBOARD_sevenSegment(characterh_4);
  delay(1000);
  byte characterh_3 = (SEG_A | SEG_B | SEG_G | SEG_C | SEG_D);
  DEVBOARD_sevenSegment(characterh_3);
  delay(1000);
  byte characterh_2 = (SEG_A | SEG_B | SEG_G | SEG_E | SEG_D);
  DEVBOARD_sevenSegment(characterh_2);
  delay(1000);
  byte characterh_1 = (SEG_F | SEG_E);
  DEVBOARD_sevenSegment(characterh_1);
  delay(1000); 
   byte characterh_0 =(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F );
  DEVBOARD_sevenSegment(characterh_0);
  delay(1000); 
}
//Function to display the pump speed
void Mpumpspeed(int pspeed) {

  //TO TUREN THE SYS ON LED GREEN

  if (pspeed <= 10)
  {
    DEVBOARD_ledGreenOff();

    DEVBOARD_writeLedBank(B00100001);

  } else if (10 < pspeed, pspeed < 50)
  {
    DEVBOARD_ledGreenOn();

    DEVBOARD_writeLedBank(B00100011);

  }
  else if (50 < pspeed, pspeed < 120) {
    DEVBOARD_ledGreenOn();
    DEVBOARD_writeLedBank(B00100111);

  }
  else if (120 < pspeed, pspeed < 161)
  {
    DEVBOARD_ledGreenOn();
    DEVBOARD_writeLedBank(B00101111);

  } else if (161 < pspeed, pspeed < 245)
  {
    DEVBOARD_ledGreenOn();
    DEVBOARD_writeLedBank(B00111111);

  }
}
