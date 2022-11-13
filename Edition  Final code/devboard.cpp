// =========================================================
/// \file   devboard.cpp 
///
/// \brief  Simple Application Programming Interface for the
///         Arduino-based Development Board
///
/// Author: Dr Mike Oliver, 
/// Version: 1.0, DMU, 17 October 2018.
// =========================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "devboard.h"


void dmu_init();
static long dmu_ticks;
static long dmu_seconds;
static boolean dmu_done;


LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Accelerometer defines
#define MPU_addr    0x68  // I2C address of the MPU-6050

// Shift-register defines
#define DATA_PIN      4
#define CLK_PIN       8
#define LATCH_PIN     7

// LED Defines
#define LED_RED       13
#define LED_YELLOW    12
#define LED_GREEN     11

// Buzzer Pin Defines
#define BUZZER_PIN    3

// Switch Pin Defines
#define SW1_PIN       10
#define SW2_PIN       9
#define DEBOUNCE_TIME  150

// Analogue Pot Defintions
#define ANALOGUE_POT  0

// Expansion Port Defintions
#define EXP_PORT_RESET  5

// Motor Pin Definitions
#define MOTOR_PIN     6

// IR Counter Pin Definitions
#define IR_COUNTER_PIN  A3

// Encoder Pin Definitions
#define ENCA_PIN      A1
#define ENCB_PIN      A2
#define ENC_BUTTON    2

// definition for the type of the button state variables
enum button_t { notPRESSED, partialPRESS, normalPRESS, heldPRESS};
button_t B2_state; // button SW2 state variable

// MCP I2C Port Extender
#define MCP_ADDRESS   0x20
#define MCP_DDRA      0x00
#define MCP_DDRB      0x01
#define MCP_GPIOA     0x12
#define MCP_GPIOB     0x13
#define KEYPAD_DEBOUNCE_CYCLES  30

// Traffic Lights Tables
#define MAX_EXP_LEDS    8
#define TL_MAX_STATES   3

// ===================================================================
//                     OUTPUT FUNCTIONALITY
// ===================================================================

// =========================================================
/// \fn     void DEVBOARD_ledRedOn() 
///
/// \brief  Turns the Red LED On
// =========================================================

void DEVBOARD_ledRedOn()
{
    digitalWrite(LED_RED, HIGH);
}


// =========================================================
/// \fn     void DEVBOARD_ledRedOff() 
///
/// \brief  Turns the Red LED Off
// =========================================================

void DEVBOARD_ledRedOff()
{
    digitalWrite(LED_RED, LOW);
}


// =========================================================
/// \fn     void DEVBOARD_ledYellowOn() 
///
/// \brief  Turns the Yellow LED On
// =========================================================

void DEVBOARD_ledYellowOn()
{
    digitalWrite(LED_YELLOW, HIGH);
}


// =========================================================
/// \fn     void DEVBOARD_ledYellowOff() 
///
/// \brief  Turns the Yellow LED Off
// =========================================================

void DEVBOARD_ledYellowOff()
{
    digitalWrite(LED_YELLOW, LOW);
}


// =========================================================
/// \fn     void DEVBOARD_ledGreenOn() 
///
/// \brief  Turns the Green LED On
// =========================================================

void DEVBOARD_ledGreenOn()
{
    digitalWrite(LED_GREEN, HIGH);
}


// =========================================================
/// \fn     void DEVBOARD_ledGreenOff() 
///
/// \brief  Turns the Green LED Off
// =========================================================

void DEVBOARD_ledGreenOff()
{
    digitalWrite(LED_GREEN, LOW);
}


// =========================================================
/// \fn     void DEVBOARD_setLedRed(int state) 
///
/// \brief  Sets the state of the RED led
///
/// \param  state: 0 = off, non-zero = on  
// =========================================================

void DEVBOARD_setLedRed(int state)
{
    if (state)
    {
        DEVBOARD_ledRedOn();
    }
    else
    {
        DEVBOARD_ledRedOff();
    }
}


// =========================================================
/// \fn     void DEVBOARD_setLedYellow(int state) 
///
/// \brief  Sets the state of the YELLOW led
///
/// \param  state: 0 = off, non-zero = on  
// =========================================================

void DEVBOARD_setLedYellow(int state)
{
    if (state)
    {
        DEVBOARD_ledYellowOn();
    }
    else
    {
        DEVBOARD_ledYellowOff();
    }
}


// =========================================================
/// \fn     void DEVBOARD_setLedGreen(int state) 
///
/// \brief  Sets the state of the GREEN led
///
/// \param  state: 0 = off, non-zero = on  
// =========================================================

void DEVBOARD_setLedGreen(int state)
{
    if (state)
    {
        DEVBOARD_ledGreenOn();
    }
    else
    {
        DEVBOARD_ledGreenOff();
    }
}


// =========================================================
/// \fn     void DEVBOARD_writeLedBank(byte value) 
///
/// \brief  Writes the parameter 'value' to MCP Bank B (LEDs)
///
/// \param  value Byte to write
///
/// From a technical perspective, the bits are inverted as the 
/// anode is sourced from Vcc. From a programming perspective,
/// this should be of no concern.
// =========================================================

void DEVBOARD_writeLedBank(byte nValue)
{
    nValue = ~nValue;     // Invert the data
    Wire.beginTransmission(MCP_ADDRESS);
    Wire.write(MCP_GPIOB);
    Wire.write(nValue);
    Wire.endTransmission();
}


// =========================================================
/// \fn     void DEVBOARD_sevenSegment(byte segments) 
///
/// \brief  Writes a 1-byte bit pattern to the shift reg / 7 seg 
///
/// \param  segments Data pattern to write
///               LSB: Bit 0 controls Segment A
///                    Bit 1 controls Segment B
///                    Bit 2 controls Segment C
///                    Bit 3 controls Segment D
///                    Bit 4 controls Segment E
///                    Bit 5 controls Segment F
///                    Bit 6 controls Segment G
///               MSB: Bit 7 controls the decimal point
///
/// (From a hardware perspective, this function drives a common 
/// anode 7-segment display. For common cathode, swap the two 
/// writes to DATA_OUT.)
// =========================================================

void DEVBOARD_sevenSegment(byte nData)
{
    byte nMask;

    digitalWrite(LATCH_PIN, LOW);
    for (nMask = B10000000; nMask; nMask >>= 1)
    {
        digitalWrite(CLK_PIN, LOW);
        if (nMask & nData)
        {
            digitalWrite(DATA_PIN, LOW);  // Invert: common Anode
        }
        else
        {
            digitalWrite(DATA_PIN, HIGH); // Invert: common Anode
        }
        digitalWrite(CLK_PIN, HIGH);
    }
    digitalWrite(LATCH_PIN, HIGH);
}


// =========================================================
/// \fn     void DEVBOARD_soundBuzzer()
///
/// \brief  Produces a short (approx 250ms) beep
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_soundBuzzer()
{
    unsigned long nTimeOut;
    int   i;

    // Produce 1000 cycles of a square wave. 
    // Approximately 125us high and approximately 125us low.
    for (i = 0; i < 1000; i++)
    {
        nTimeOut = micros() + 125;
        digitalWrite(BUZZER_PIN, HIGH);
        while (nTimeOut > micros());
    
        nTimeOut = micros() + 125;
        digitalWrite(BUZZER_PIN, LOW);
        while (nTimeOut > micros());
    }
}


// =========================================================
/// \fn     void DEVBOARD_soundTick()
///
/// \brief  Produces a short tick sound
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_soundTick()
{
    unsigned long nTimeOut;
    int   i;

    // Produce 1 cycles of a square wave. 
    // Approximately 125us high and approximately 125us low.
    for (i = 0; i < 1; i++)
    {
        nTimeOut = micros() + 125;
        digitalWrite(BUZZER_PIN, HIGH);
        while (nTimeOut > micros());
    
        nTimeOut = micros() + 125;
        digitalWrite(BUZZER_PIN, LOW);
        while (nTimeOut > micros());
    }

}


// ===================================================================
//                   LCD DISPLAY HELPER FUNCTIONS
// ===================================================================

// =========================================================
/// \fn     void DEVBOARD_lcdClear()
///
/// \brief  Clears the LCD screen
///
/// As well as clearing the LCD, the cursor is reset to 
/// the home position (0, 0)
// =========================================================

void DEVBOARD_lcdClear()
{
    lcd.clear();
}


// =========================================================
/// \fn     void DEVBOARD_lcdSetCursor(int x, int y)
///
/// \brief  Moves the cursor to the specified position
///
/// \param  x x-position
/// \param  y y-position
/// 
/// Note the screen is zero-indexed. (0,0) is the origin.
// =========================================================

void DEVBOARD_lcdSetCursor(int x, int y)
{
    lcd.setCursor(x, y);
}


// =========================================================
/// \fn     void DEVBOARD_lcdPrintString(char *str)
///
/// \brief  Writes the string str to the LCD display
///
/// \param  str Pointer to the string to be written
// =========================================================

void DEVBOARD_lcdPrintString(char *str)
{
    lcd.print(str);
}


// =========================================================
/// \fn     void DEVBOARD_lcdPrintInt(int n)
///
/// \brief  Writes the integer n to the LCD display
///
/// \param  n Value to be written
// =========================================================

void DEVBOARD_lcdPrintInt(int n)
{
    lcd.print(n);
}


// =========================================================
/// \fn     void DEVBOARD_lcdPrintLong(long n)
///
/// \brief  Writes the long-integer n to the LCD display
///
/// \param  n Value to be written
// =========================================================

void DEVBOARD_lcdPrintLong(long n)
{
    lcd.print(n);
}


// =========================================================
/// \fn     void DEVBOARD_lcdPrintFloat(float f)
///
/// \brief  Writes the floating-point value f to the LCD display
///
/// \param  f Value to be written
// =========================================================

void DEVBOARD_lcdPrintFloat(float f)
{
    lcd.print(f);
}


// =========================================================
/// \fn     void DEVBOARD_lcdPrintHex(long n)
///
/// \brief  Writes the long-integer n to the LCD display
///
/// \param  n Value to be written
///
/// The value will be displayed in hexadecimal
// =========================================================

void DEVBOARD_lcdPrintHex(long n)
{
    lcd.print(n, HEX);
}


// ===================================================================
//                     USER INPUT FUNCTIONALITY
// ===================================================================

// =========================================================
/// \fn     buttonStatus_t DEVBOARD_buttonS1Status()
///
/// \brief  Obtains the current switch (button) status of S1.
///
/// \return BUTTON_RELEASED or BUTTON_PRESSED
///
/// This currently examines the state of the switch. 
// =========================================================

buttonStatus_t DEVBOARD_buttonS1Status()
{
    if (digitalRead(SW1_PIN))
    {
        return BUTTON_RELEASED;
    }
    return BUTTON_PRESSED;
}


// =========================================================
/// \fn     buttonStatus_t DEVBOARD_buttonS2Status()
///
/// \brief  Obtains the current switch (button) status of S2.
///
/// \return BUTTON_RELEASED or BUTTON_PRESSED
///
/// This currently examines the state of the switch. 
// =========================================================

buttonStatus_t DEVBOARD_buttonS2Status()
{
    if (digitalRead(SW2_PIN))
    {
        return BUTTON_RELEASED;
    }
    return BUTTON_PRESSED;
}

// =========================================================
/// \fn     void DEVBOARD_waitForS1Press()
///
/// \brief  Waits for switch S1 to be pushed and released.
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_waitForS1Press()
{
    int nCount;

    // Wait for SW1 to be held down for a number of consecutive 1ms
    // 'ticks' defined by DEBOUNCE_TIME.
    nCount = 0;
    while (nCount < DEBOUNCE_TIME)
    {
        delay(1);
        if (digitalRead(SW1_PIN) == LOW)
        {
            nCount++;
        }
        else
        {
            nCount = 0;
        }
    }

    // Wait for SW1 to be released for a number of consecutive 1ms
    // 'ticks' defined by DEBOUNCE_TIME.
    nCount = 0;
    while (nCount < DEBOUNCE_TIME)
    {
        delay(1);
        if (digitalRead(SW1_PIN) == HIGH)
        {
            nCount++;
        }
        else
        {
            nCount = 0;
        }
    }
}


// =========================================================
/// \fn     void DEVBOARD_waitForS2Press()
///
/// \brief  Waits for switch S2 to be pushed and released.
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_waitForS2Press()
{
    int nCount;

    // Wait for SW2 to be held down for a number of consecutive 1ms
    // 'ticks' defined by DEBOUNCE_TIME.
    nCount = 0;
    while (nCount < DEBOUNCE_TIME)
    {
        delay(1);
        if (digitalRead(SW2_PIN) == LOW)
        {
            nCount++;
        }
        else
        {
            nCount = 0;
        }
    }

    // Wait for SW2 to be released for a number of consecutive 1ms
    // 'ticks' defined by DEBOUNCE_TIME.
    nCount = 0;
    while (nCount < DEBOUNCE_TIME)
    {
        delay(1);
        if (digitalRead(SW2_PIN) == HIGH)
        {
            nCount++;
        }
        else
        {
            nCount = 0;
        }
    }
}


// =========================================================
/// \fn     byte DEVBOARD_keypadScan() 
///
/// \brief  Performs a scan of the 4x3 keypad 
///
/// \return 0-9 for '0'-'9'
///         10  for '*'
///         11  for '#'
///         254 for error (simulataneous key presses)
///         255 for error (no press)
/// 
/// This is useful for non-blocking applications 
// =========================================================

byte DEVBOARD_keypadScan()
{
    static byte sRowPattern[4] = {B10111111, B11011111, B11101111, B11110111};
    byte nValue = 0;
    byte nGotPress = 0;
    byte nNewPress;

    byte nRow, nColumn;

    // Work through the keypad on a row-by-row basis
    for (nRow = 0; nRow < 4; nRow++)
    {
        // Pull the indexed row Low
        Wire.beginTransmission(MCP_ADDRESS);
        Wire.write(MCP_GPIOA);    
        Wire.write(sRowPattern[nRow]);
        Wire.endTransmission();

        // Read the column value
        Wire.beginTransmission(MCP_ADDRESS);
        Wire.write(MCP_GPIOA); 
        Wire.endTransmission();
        Wire.requestFrom(MCP_ADDRESS, 1); 
        nColumn = Wire.read() & B00000111;

        // Try to parse the column value
        nNewPress = 0;
        switch(nColumn)
        {
            case B00000111:   // nothing pressed
                break;
            case B00000011:
                nValue = 1;
                nNewPress = 1;
                nGotPress++;
                break;
            case B00000101:
                nValue = 2;
                nNewPress = 1;
                nGotPress++;
                break;
            case B00000110:
                nValue = 3;
                nNewPress = 1;
                nGotPress++;
                break;
            default:
                return 0xFF;    // Error: no keypress
        }
        if (nGotPress > 1)
        {
            return 0xFE;        // Error: Simultaneous key presses
        }

        // Work out which key was pressed based on the row number and which
        // key in that row was pressed,
        if (nNewPress)
        {
            nValue = nValue + (nRow * 3);
        }
    }

    // Finished performing the scan. Return if no action.
    if (!nGotPress)
    {
        return 0xFF;
    }

    // Finalizing the output
    switch (nValue)
    {
    case 11:
        nValue = 0;     // Zero is on position 11 :)
        break;
    case 12:
        nValue = 11;    // '#' is on position 12     
    }
    return nValue;
}


// =========================================================
/// \fn     byte DEVBOARD_keypadRead() 
///
/// \brief  Waits for a valid keypress of the 4x3 keypad 
///
/// \return 0-9 for '0'-'9'
///         10  for '*'
///         11  for '#'
/// 
/// This function is a blocking function. It will wait until
/// aa valid keypress and release is detected. 
// =========================================================

byte DEVBOARD_keypadRead()
{
    int nCount;
    byte nState, nResult;
    int nPrevState = 0xFF;     // No Press

    // Wait for a single key to be held down for a number of consecutive 1ms
    // 'ticks' defined by KEYPAD_DEBOUNCE_CYCLES.
    nCount = 0;
    while (nCount < KEYPAD_DEBOUNCE_CYCLES)
    {
        delay(1);
        nState = DEVBOARD_keypadScan();
        if ((nState >= 0) && (nState <= 11))
        {
            if (nState == nPrevState)
            {
                nCount++;
            }
            else
            {
                nCount = 0;
            }
        }
        else
        {
            nCount = 0;
        }
        nPrevState = nState;
    }
    nResult = nState;
    if (nResult < 10)
    {
        DEVBOARD_sevenSegment(sCharSet[nResult]);
    }

    // Wait for key to be released for a number of consecutive 1ms
    // 'ticks' defined by KEYPAD_DEBOUNCE_CYCLES.
    nCount = 0;
    while (nCount < KEYPAD_DEBOUNCE_CYCLES)
    {
        delay(1);
        nState= DEVBOARD_keypadScan();
        if (nState == 0xFF)
        {
            nCount++;
        }
        else
        {
            nCount = 0;
        }
    }
    DEVBOARD_sevenSegment(B10000000);
    DEVBOARD_soundTick();

    return nResult;
}


// =========================================================
/// \fn     int DEVBOARD_keypadReadInt(int len) 
///
/// \brief  Reads an integer value from the keypad 
///
/// \param  len Required number of key presses (0 for unknown)
///
/// \return The integer that has been read.
/// 
/// This function is a blocking function. It will wait until
/// the integer has been read. This will terminate if the #
/// key is pressed or the required number of keypresses. 
// =========================================================

int DEVBOARD_keypadReadInt(int len)
{
    long nValue = 0;
    byte keypress;
    int  numChars = 0;

    for(;;)
    {
        keypress = DEVBOARD_keypadRead();

        // Deal with the # key
        if (keypress == 11)
        {
            break;
        }
        
        if (keypress < 10)
        {
            nValue *= 10;
            nValue += (long)keypress;
            numChars++;
        }

        if (len && (numChars >= len))
        {
            break;
        }
    }

    return (int)nValue;
}



// ===================================================================
//                    OTHER OUTPUT FUNCTIONALITY
// ===================================================================

// =========================================================
/// \fn     void DEVBOARD_driveMotor(byte drive) 
///
/// \brief  Drives the motor 
///
/// \param  drive Motor drive in the range 0 - 255.
// =========================================================

void DEVBOARD_driveMotor(byte drive)
{
    if (!dmu_done)
    {
        analogWrite(MOTOR_PIN, drive);
    }
}


// ===================================================================
//                    SENSOR INPUT FUNCTIONALITY
// ===================================================================

// =========================================================
/// \fn     int DEVBOARD_readPotentiometer() 
///
/// \brief  Reads the ADC associated with the potentiometer 
///
/// \return ADC value in the range 0 to 1023.
// =========================================================

int DEVBOARD_readPotentiometer()
{
    return analogRead(ANALOGUE_POT);
}


// =========================================================
/// \fn     void DEVBOARD_readAccelerometer(int *xAccel, int *yAccel, int *zAccel) 
///
/// \brief  Reads the x, y & z components of acceleration 
///
/// \param  *xAccel Pointer for the x-component
/// \param  *yAccel Pointer for the y-component
/// \param  *zAccel Pointer for the z-component
///
/// Note that these component values are raw unscaled values
// =========================================================

void DEVBOARD_readAccelerometer(int *xAccel, int *yAccel, int *zAccel)
{
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,6,true);  // request a total of 6 registers
    *xAccel = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
    *yAccel = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    *zAccel = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)    
}


// =========================================================
/// \fn     void DEVBOARD_readTemperature(int *temperature) 
///
/// \brief  Reads the temperature value 
///
/// \param  *temperature Pointer for the temperature value
///
/// Note that this values is the raw unscaled values and will 
/// need to be scaled using the formula
///      T = (TEMP_OUT / 340.00) + 36.53 
// =========================================================

void DEVBOARD_readTemperature(int *temp_out)
{
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x41);  // starting with register 0x3B (TEMP_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,2,true);  // request a total of 6 registers
    *temp_out = Wire.read() << 8 | Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)     
}


//========================================================================
//                       INITIALIZATION
//========================================================================

void DEVBOARD_init() 
{
    // Set-up the LCD module
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);

    // Set-up Traffic Lights LEDs
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    // Set-up Shift Register LEDs
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);

    // Set-up Buzzer Pin
    pinMode(BUZZER_PIN, OUTPUT);

    // Set-up the SW1 and SW2 as inputs
    pinMode(SW1_PIN, INPUT);
    pinMode(SW2_PIN, INPUT);

    // FWIW: Set up the incremental encoder pins as inputs
    pinMode(ENCA_PIN, INPUT);
    pinMode(ENCB_PIN, INPUT);
    pinMode(ENC_BUTTON, INPUT);

    // Initialize the LEDs
    digitalWrite(LED_RED, LOW);       // RED off
    digitalWrite(LED_YELLOW, LOW);    // YELLOW off
    digitalWrite(LED_GREEN, LOW);     // GREEN off
    DEVBOARD_writeLedBank(B00000000);          // All off
    DEVBOARD_sevenSegment(B00000000);   

    // Make the expansion port reset rock-solid
    pinMode(EXP_PORT_RESET, INPUT);
    digitalWrite(EXP_PORT_RESET, HIGH);

    // Set-up the motor pin
    pinMode(MOTOR_PIN, OUTPUT);

    // FWIW: Set-up the IR counter pin
    pinMode(IR_COUNTER_PIN, INPUT);

    // Set-up I2C MCP; Port A for keypad switches
    // Rows R1 to R4 (inputs) on IO15-IO12 respectively
    // Cols C1 to C3 (outputs) on IO11-IO9 respectively
    // Note the IO pins are not zero-indexed.
    Wire.beginTransmission(MCP_ADDRESS);
    Wire.write(MCP_DDRA);     // Set Bank B to
    Wire.write(B00000111);    // all outputs
    Wire.endTransmission(); 

    // Set-up I2C MCP; Port B for LED outputs
    Wire.beginTransmission(MCP_ADDRESS);
    Wire.write(MCP_DDRB);     // Set Bank B to
    Wire.write(B00000000);    // all outputs
    Wire.endTransmission(); 

    // initialise the I2C bus. wake up the MPU-6050
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);      

    lcd.print("Good Morning Universe");
    //lcd.setCursor(0, 1);
    //lcd.print("Push SW1 to cont");

    // Call dmu_init() for motor-driven projects.
    dmu_init();
}




//========================================================================
//                                MAGIC 
//========================================================================

void dmu_init()
{
    dmu_ticks = 0;
    dmu_seconds = 0;
    dmu_done = false;

    cli();
    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    TCCR1C = 0x00;
    OCR1A = 250;
    TIFR1 = (1 << OCF1A) | (1 << OCF1B) | (1 << TOV1);
    TIMSK1 = (1 << OCIE1A);    
    sei();
}

ISR(TIMER1_COMPA_vect)
{  
    dmu_ticks++;
    if (dmu_ticks >= 1000)
    {
        if (dmu_done == true)
        {
            OCR0A = 0x00;
            return;
        }
        dmu_ticks = 0;
        dmu_seconds++;
        if (OCR0A == 0)
        {
            dmu_seconds = 0;
        }
        if (dmu_seconds == 30)
        {
            dmu_done = true;
        }
    }
}


