// =========================================================
/// \file   devboard.h 
///
/// \brief  Simple Application Programming Interface for the
///         Arduino-based Development Board
///
/// Author: Dr Mike Oliver, 
/// Version: 1.0, DMU, 17 October 2018.
// =========================================================

#ifndef _DEVBOARD_H_
#define _DEVBOARD_H_


/// 7-Segment defines for individual segments
#define SEG_A         B00000001
#define SEG_B         B00000010
#define SEG_C         B00000100
#define SEG_D         B00001000
#define SEG_E         B00010000
#define SEG_F         B00100000
#define SEG_G         B01000000
#define SEG_DP        B10000000

// Character set definitions for individual characters
#define CHAR_0        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define CHAR_1        (SEG_B | SEG_C)
#define CHAR_2        (SEG_A | SEG_B | SEG_D | SEG_E | SEG_G)
#define CHAR_3        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_G)
#define CHAR_4        (SEG_B | SEG_C | SEG_F | SEG_G)
#define CHAR_5        (SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define CHAR_6        (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define CHAR_7        (SEG_A | SEG_B | SEG_C)
#define CHAR_8        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define CHAR_9        (SEG_A | SEG_B | SEG_C | SEG_F | SEG_G)
#define CHAR_A        (SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define CHAR_B        (SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define CHAR_C        (SEG_A | SEG_D | SEG_E | SEG_F)
#define CHAR_D        (SEG_B | SEG_C | SEG_D | SEG_E | SEG_G)
#define CHAR_E        (SEG_A | SEG_D | SEG_E | SEG_F | SEG_G)
#define CHAR_F        (SEG_A | SEG_E | SEG_F | SEG_G)

// Character set array
const byte sCharSet[] = {CHAR_0, CHAR_1, CHAR_2, CHAR_3, CHAR_4, CHAR_5, CHAR_6, CHAR_7, 
                         CHAR_8, CHAR_9, CHAR_A, CHAR_B, CHAR_C, CHAR_D, CHAR_E, CHAR_F};

// Button status
typedef enum {
    BUTTON_RELEASED, 
    BUTTON_PRESSED
} buttonStatus_t;



// ===================================================================
//                     OUTPUT FUNCTIONALITY
// ===================================================================

// =========================================================
/// \fn     void DEVBOARD_ledRedOn() 
///
/// \brief  Turns the Red LED On
// =========================================================

void DEVBOARD_ledRedOn();


// =========================================================
/// \fn     void DEVBOARD_ledRedOff() 
///
/// \brief  Turns the Red LED Off
// =========================================================

void DEVBOARD_ledRedOff();


// =========================================================
/// \fn     void DEVBOARD_ledYellowOn() 
///
/// \brief  Turns the Yellow LED On
// =========================================================

void DEVBOARD_ledYellowOn();


// =========================================================
/// \fn     void DEVBOARD_ledYellowOff() 
///
/// \brief  Turns the Yellow LED Off
// =========================================================

void DEVBOARD_ledYellowOff();


// =========================================================
/// \fn     void DEVBOARD_ledGreenOn() 
///
/// \brief  Turns the Green LED On
// =========================================================

void DEVBOARD_ledGreenOn();


// =========================================================
/// \fn     void DEVBOARD_ledGreenOff() 
///
/// \brief  Turns the Green LED Off
// =========================================================

void DEVBOARD_ledGreenOff();


// =========================================================
/// \fn     void DEVBOARD_setLedRed(int state) 
///
/// \brief  Sets the state of the RED led
///
/// \param  state: 0 = off, non-zero = on  
// =========================================================

void DEVBOARD_setLedRed(int state);


// =========================================================
/// \fn     void DEVBOARD_setLedYellow(int state) 
///
/// \brief  Sets the state of the YELLOW led
///
/// \param  state: 0 = off, non-zero = on  
// =========================================================

void DEVBOARD_setLedYellow(int state);


// =========================================================
/// \fn     void DEVBOARD_setLedGreen(int state) 
///
/// \brief  Sets the state of the GREEN led
///
/// \param  state: 0 = off, non-zero = on  
// =========================================================

void DEVBOARD_setLedGreen(int state);


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

void DEVBOARD_writeLedBank(byte nValue);


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

void DEVBOARD_sevenSegment(byte nData);


// =========================================================
/// \fn     void DEVBOARD_soundBuzzer()
///
/// \brief  Produces a short (approx 250ms) beep
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_soundBuzzer();


// =========================================================
/// \fn     void DEVBOARD_soundTick()
///
/// \brief  Produces a short tick sound
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_soundTick();



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

void DEVBOARD_lcdClear();


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

void DEVBOARD_lcdSetCursor(int x, int y);


// =========================================================
/// \fn     void DEVBOARD_lcdPrintString(char *str)
///
/// \brief  Writes the string str to the LCD display
///
/// \param  str Pointer to the string to be written
// =========================================================

void DEVBOARD_lcdPrintString(char *str);


// =========================================================
/// \fn     void DEVBOARD_lcdPrintInt(int n)
///
/// \brief  Writes the integer n to the LCD display
///
/// \param  n Value to be written
// =========================================================

void DEVBOARD_lcdPrintInt(int n);


// =========================================================
/// \fn     void DEVBOARD_lcdPrintLong(long n)
///
/// \brief  Writes the long-integer n to the LCD display
///
/// \param  n Value to be written
// =========================================================

void DEVBOARD_lcdPrintLong(long n);


// =========================================================
/// \fn     void DEVBOARD_lcdPrintFloat(float f)
///
/// \brief  Writes the floating-point value f to the LCD display
///
/// \param  f Value to be written
// =========================================================

void DEVBOARD_lcdPrintFloat(float f);


// =========================================================
/// \fn     void DEVBOARD_lcdPrintHex(long n)
///
/// \brief  Writes the long-integer n to the LCD display
///
/// \param  n Value to be written
///
/// The value will be displayed in hexadecimal
// =========================================================

void DEVBOARD_lcdPrintHex(long n);


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

buttonStatus_t DEVBOARD_buttonS1Status();


// =========================================================
/// \fn     buttonStatus_t DEVBOARD_buttonS2Status()
///
/// \brief  Obtains the current switch (button) status of S2.
///
/// \return BUTTON_RELEASED or BUTTON_PRESSED
///
/// This currently examines the state of the switch. 
// =========================================================

buttonStatus_t DEVBOARD_buttonS2Status();



// =========================================================
/// \fn     void waitForS1Press()
///
/// \brief  Waits for switch S1 to be pushed and released.
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_waitForS1Press();


// =========================================================
/// \fn     void waitForS2Press()
///
/// \brief  Waits for switch S2 to be pushed and released.
///
/// Note that this is a blocking function.
// =========================================================

void DEVBOARD_waitForS2Press();


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

byte DEVBOARD_keypadScan();


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
/// a valid keypress and release is detected. 
// =========================================================

byte DEVBOARD_keypadRead();


// =========================================================
/// \fn     int DEVBOARD_keypadReadInt(int len);
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

int DEVBOARD_keypadReadInt(int len);


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

void DEVBOARD_driveMotor(byte drive);


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

int DEVBOARD_readPotentiometer();


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

void DEVBOARD_readAccelerometer(int *xAccel, int *yAccel, int *zAccel);


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

void DEVBOARD_readTemperature(int *temp_out);


//========================================================================
//                       INITIALIZATION
//========================================================================

// =========================================================
/// \fn     void DEVBOARD_init()
///
/// \brief  Performs the Development Board initialization.
// =========================================================

void DEVBOARD_init(); 

#endif
