# TANK_LEVEL_CONTROL_PROGRAM_BY_Arduino_C
## Abstract   

* This paper is a documentary on a software program made by Arduino C programming language for the water distribution system, it contains a brief introduction that introduces the system and user specification that discuss user requirements, and algorithms illustrated in flowcharts and description of them, and description of major important parts in the code and how it implemented, and at the final, it discussed the test plan and debugging and some problems and how it sorted out.    
## Introduction

![image](https://user-images.githubusercontent.com/40359381/201549522-cf7798a3-1957-493c-9bd6-56a4a10d22dd.png)
 
   Figure 1[ A typical water distribution system] Source  [3] 
                    
A water distribution system consists of the water treatment plant, pump, tank, and residential area as shown in figure 1 the water from the source is pumped into a tank located at a certain elevation to provide a sufficient water pressure all the day to a residential area, this paper aims to build an algorithm that controls the pump in two modes the first mode is a manual mode which will use a potentiometer to control the pump speed and also build constrain in the system to ensure the water level not exceed 85% of the tank the second mode is automatic which will use a Bang-bang (on-off) controller that will control the level of water inside the tank and pump speed automatically, that means when the tank is empty the level sensor will indicate the low level and the controller will start the pump automatically until the tank reach 85% and stop the pump to make sure the residential area have sufficient water during all the day. The goal is to fulfill water demand at any time of day while keeping the water level in the tank within operating constraints (between the minimum of 10% and maximum levels of 85%).

The code will be written by Arduino C programming language and simulated on ATMega328p Microcontroller development board by DMU shown in figure2, this hardware kit and Arduino IDE have all ability to achieve our aims and to simulate the code on it, for example, the DC motor will be the pump of the system and the screen will be the user interface and keypad will be input for the user also it has some outputs such as LEDs which some of them will show the pump speed and others will show the level of the tank and potentiometer will control the pump in manual mode also seven-segment will show the cutdown after user select the mode before start. 
![image](https://user-images.githubusercontent.com/40359381/201549580-07ea9402-4b1b-46d0-9e90-406f165952d3.png)

   Figure 2[ DMU-designed ATMega328P Microcontroller Development Board] source [1] 

## Specification Requirements

A water distribution system contains a lot of subsystems working together to obtain the main goal of the system and offer a good experience to the user, the main goal of the software is to manage the working process of this system by offering two different operation modes to the user and LCD screen to show the user water demanded and tank level every hour, as well as many LEDs some of them will show the pump speed and others, show the tank level and system operation. On the other hand, the system also has an interruption button that will help users to interrupt the working process at any time. The requirements of this system are distributed in the subsystems inside the main system the most important subsystems are shown in figure 3

![image](https://user-images.githubusercontent.com/40359381/201549659-4b21b356-b6d5-44a2-929f-2e358d00a046.png)
    Figure 3[ Schematic of Input/output devices in our system]
* The Pump: The pump plays an important role in the system in which it supplies the tank with water, So the user will be able to control the pump in different modes the first one is Manual mode: in this mode, the user will input the speed of the pump by using an analog potentiometer manual. the second mode is Automatic mode: in this mode, the user will input the initial value for the water level through the keypad, and the constrain will build to accept the initial value from the user between 10% to 85% of the tank capacity if the user enters the value in this range the program will start without any problem if not the program will show the message on LCD tell the user it is wrong input and will ask him to input a value from 10% to 85%. In the automatic mode, the controller called bang-bang controller will control the speed of the pump depending on the water level inside the tank and it will adjust the pump speed over the 24 hours horizon. Also, the two buttons will be available to the user to start and stop the operation modes.  As well as the constraints will be built to ensure the maximum speed of the pump does not exceed the permissible speed and yet the water level in the tank is between 10% to 85% of the tank’s capacity and preventing it from emptying or overflowing. 
* 	The Tank: the tank should contain sensors that measure the level of water for 24 hours and send their data as output to the controller, but in our case, the code will be written to simulate the water demand during 24 hours of the day and the controller will interact with it as like a real tank, Also, there are two LEDs (yellow and red)  as shown in figure 3 will indicate the water level in the tank if the water level less than 10% the red LED will be turned on, and when the water reaches 85% the yellow LED turned on. 
* The LCD screen: the LCD screen will interface with the user and the user will be able to see data related to the system on it such as pump speed, the current timer in hours, represent the mode M/A, tank current volume by % and current damped in l/s. 
* The Keypad: the user will use it to select the working mode of the system by pressing 1 for Manual mode or pressing 2 for Automatic mode. 
* The potentiometer: the user will use it to control the pump speed during manual mode running. 
* The S1 and S2 buttons: the user will use them to start and stop the process. 
* Interruption Button: it is for interrupting the system and when the user press on it, the system will stop automatically, and the E character will be displayed on Seven segments with an error message will be chosen randomly displayed on the serial monitor and LCD.  
*	The seven segments: it will display a cutdown from 9 to 1 after the user press on start or stop button after that the system will start to work or will stop. 
*	LED 7 AND LED 5: when the system is operating the green LED 7 will be ON to indicate the system is running, and when the system is of the red LED will be ON. 	
*	LED 8 TO LED 12: the LED bar will show the pump speed to the user when the pump is running, the LED 8 represents the maximum speed, and LED 12 represents the minimum speed. 
*
The following table shows all data need to know about the user requirements specification: 
*
<img width="449" alt="image" src="https://user-images.githubusercontent.com/40359381/201549803-874ef72f-e4ae-40db-a565-f132e3960bc8.png">

## The Algorithms 

 By breaking the problem into smaller parts, we can notice that there are main goals for our program as shown in figure4 the first one is the drive the system automatically to adjust the water level in the tank by the bang-bang controller to ensure the tank has a sufficient water level for 24 hours, the second task is to give another option to the user to control the system manually by using a potentiometer to control the pump speed. The third and the most important part is an interruption button that plays an emergency button that gives the user ability to stop the system at any time by pressing only one button. Every task in those tasks will have an individual algorithm for it and at the final, all three algorithms will combine into one algorithm. 
 ![image](https://user-images.githubusercontent.com/40359381/201549852-96e53bd7-a523-4b92-889b-fa9dbdf2c057.png)

Figure 4[ an Overview of main tasks on the water distribution system]

To make the work easier and decrease the code lines, some functions will be designed that are common in the three tasks such as cutdown action before starting the operation and visualizing the pump speed in LEDs, and getting the user input from the keypad. All those repeated processes will be assigned as functions that will do the same thing in different tasks that thing will save a lot of time and make the code readable. 

The design plane will be to code every algorithm individually and combine the three algorithms into one algorithm the next pages will discusthe s algorithms strategy and work plan. 

## Manual Mode Algorithm The First Algorithm 

![image](https://user-images.githubusercontent.com/40359381/201549937-015bd44f-5b3b-46b6-ba3f-b3d87aee1b74.png) 

Figure 5 [ Flow Chart of Manual mode Algorithm] 


* •	At the first a welcome message called “Water distribution system” will appear on the LCD for 10 seconds after that another message will appear: asks the user to select an operation mode manual or automatic.

* •	The user will use a Keypad to enter the selection 1 for manual and 2 for automatic.

* •	When the user presses a 1 on Keypad the manual program will start and the tank will initialize with a 70% water level after that the message will appear to the user to tell him “System Ready” and the program will ask the user to press S1 Button to start the system or to press S2 button to stop the system if the user press on S2 Button a cutdown appear on seven segments from 9 to 0 after that a message will appear “system stop” on LCD and the program will return to Start, on the other hand, if the user press on S1 button a system will start to work after a cutdown appears on seven segments from 9 to 0. 

* •	In the Manual mode the user will control the pump by using a potentiometer, the pump working range is from 10 to 240 Rpm and the potentiometer working range is 0 to 1024. So, the function called map ()  [1]will be used to re-maps the range of the potentiometer to the working range of the pump from 10 to 240. Also, a constrain will be built to make sure the pump does not exceed 240 rpm as maximum speed and 10 rpm as a minimum speed.  

* •	The LEDs bank from 8 to 12 will visualize the pump speed which the LED 8 mean max speed and LED 12 min speed, as well as a green, LED  3 will be on if the pump running higher or equal to 6 rpm. Also, LED 1 and LED 2 will indicate the water level in the tank if the water level is 10% the red LED 1 will be on and if the water level is 85% the yellow LED 2 will be on. Also, the LEDs 7 and 5 will indicate the system states if the system is running the LED 7 will be ON and if the system is off the LED 5 will be off. The following table shows the induction of LEDs in the system: 

<img width="401" alt="image" src="https://user-images.githubusercontent.com/40359381/201550020-fd5adbbf-d795-4889-b827-ed75079ffef5.png">

* •	A function y = 0.082x+5.6678 called Flow function is shown in the flow chart will simulate the system amount of water where Y is the water level and X is the pump Rpm. 

* •	the data belonging to the system will be displayed on LCD and Serial monitors such as model selection, timer, current volume concerning capacity %, Pump speed in Rpm and current demanded in L\s the following table shows the format of LCD:

<img width="277" alt="image" src="https://user-images.githubusercontent.com/40359381/201550041-ae1b085d-1d68-4fbd-bce3-11f0c00aa775.png">

## Automatic Mode Algorithm  The Second Algorithm  

![image](https://user-images.githubusercontent.com/40359381/201550074-94f71f8a-d0ed-4f51-834b-75f07fac1ecf.png)

* •	After the user press on 2 the pop-up message will ask the user to enter the initial value of water level between 10% to 85% to initialize the system.

* •	After that a message will ask the user to press on the S1 switch to start the system or press on S2 to shut down the program.

* •	If the user press on S1 “start button” the cutdown will show in seven-segment with a delay of 10 seconds and the program will start to work if the user press on S2 button the cutdown will show in seven-segment with a delay of 10 seconds and will be shut down. 

* •	The PUMP controller: the pump speed will be controlled by the Bang-bang controller a simple closed-loop controller shown in Figures 7 and 8. When the water level is low the controller will turn the pump on and when the water level is high the controller will turn the pump off, it will work according to constrain that will make the controller control the pump in the working range from 10% and 85% to ensure the water not exceed the maximum level in the tank. The if statement will be used to code this controller. 

<img width="410" alt="image" src="https://user-images.githubusercontent.com/40359381/201550118-d6005b6d-2ee7-4eb0-b844-acf1ecdda257.png">

* •	Like manual mode the LEDs in the system will do the same function in automatic mode. 

* •	A function y = 0.082x+5.6678 called Flow function is shown in the flow chart will simulate the system amount of water where Y is the water level and X is the pump Rpm. 

* All information about the system when it is running will display on LCD and serial monitor like Manual mode

<img width="274" alt="image" src="https://user-images.githubusercontent.com/40359381/201550157-f3d2a688-e397-47b3-b7ea-3683f32c4ca0.png">

## The Third Algorithm for “Interruption Button”:  

the main function of this algorithm is to stop the system at any time with one click from the user, when the user press the interruption button the program will stop and the red  LED 6 will blink as a warning and E character will be displayed on Seven-segment also an error message selected randomly will be displayed on LCD and buzzer will turn on. The following table shows the error code and message:

![image](https://user-images.githubusercontent.com/40359381/201550195-4ee96520-75dd-4b52-a4e8-11b4b63eaa91.png)

<img width="220" alt="image" src="https://user-images.githubusercontent.com/40359381/201550215-d49989f1-3fbe-46a2-8f32-11102ff98f65.png">


## In the final, all three algorithms will be combined into one Algorithm as shown in figure10.   

![image](https://user-images.githubusercontent.com/40359381/201550230-5494f2e6-55d9-4c7a-ba39-53eea6b73504.png)

## Code Implementation and Description 

As discussed in user specification requirements every device in the DEVBOARD has a function to the program such as 7seg, LED bank, switches, LCD etc. In this section the code of every device in DEVBOARD will be explained and the way of implementing the code as well. 

> 1-	 7-segment: 

* In the 7- segment the main function of it is to display a cutdown of 10 seconds to the user in many parts of the program such as when the user presses the SW2 stop button or when the user presses SW1 to start the program etc. so to decrease the lines of code, the function has been defined at the end of the code (outside the main loop function), and when we call the Function in the main program the cutdown start the function structure illustrated in figure 11, the function prototype is defined at the first of the code ( void cutdown() ; ) and when we need to call the function inside the program (main loop) we just calla by a single line ( cutdown (); )  the cutdown will start.

![image](https://user-images.githubusercontent.com/40359381/201550317-762278a7-412e-4423-b013-a584069937e9.png)

 > 2-	 LED bank and Green LED: 
 
 * 	the function of the LED bar is to display the pump speed in Manual mode and Auto mode and the function of the green LED is to display the state of the pump if the pump working it will be on if not it will be off. So, to decrease the lines of code because the part is repeated on Manual and auto modes, the Function built to do this job every time the program run, and the pump works the pump speed will be passed to this function and it will display the pump speed and pump state the function is shown in figure 12, the function prototype is defined at the first of program( void Mpumpspeed();)  and we can call it by (  Mpumpspeed( integer value that needs to display on the LED bar  ); ). 
 
 * 	The structure of the function builds on if statement condition, when the pump speed value is passed to the function inside the main loop this value will enter the function if this value is between 10 and 50 the green led will be on, and to display this value on the led bar the ready-made function is used called DEVBOARD_WriteLedBank this function takes a binary number and displayed on the bar as shown in figure 12 it has 8 LEDs when the value equal one that means led on and when is zero that means led off.   

![image](https://user-images.githubusercontent.com/40359381/201550372-d60fcb8a-a89d-4852-a2fe-6292f8a6c8c5.png)

> 3 -	 Interruption Button: 

* One of the very important buttons in the program, it works as emergency stop button it should stop the program at any time when the program running, to implement the interruption button at the First step: the bool variable called flag is defined at the first of the program (bool flag = false;) this flag will work as indicator, the Second step: is define a two function at the end of the program the first one to check the state of the interruption and the second function that run when the interruption happen, the E character will appear on 7-seg ,motor off and an random error message will appear on LCD to show the user what the problem also the yellow led will blink and buzzer, the two function are shown in figure 13, to display the error message on LCD at the first of program string variables defined as array has 5 message and to make the error message fit in LCD we divided messages on two arrays, by using random function to select the message randomly from 0 to 5 form the error messages arrays and display them by using a built in function called DEVBOARD_lcdPrintString(& msg[0] ); this character & to make the function take the message from the address of the array.


![image](https://user-images.githubusercontent.com/40359381/201550403-2c283de6-49f5-453a-9469-e1c39a36e7c2.png)

The third step:  is to set up the interruption pins, and LED and define the attchinterrpution function as shown in the following figure.

![image](https://user-images.githubusercontent.com/40359381/201550418-8300f40a-82e8-4d4e-96fe-07193d661492.png)

The final step: To detect the interruption of the program, the function prototype (interruption_happen () ;) called by if the condition on multiply places in the code if (flag)interption_happen();  if the flag is true that means the user interrupts the program also we added it on while loop so when the program running the interruption state will be checked every iteration of the loop, and if the user interrupts the program the interpption_happen function will be run. 


> 4. The Pump:


> The Pump in Manual Mode: 

* 	In the manual mode, the pump is controlled a by a potentiometer, and the potentiometerangeng is 1 to 1024 so we convert the working rang to the pump working range 10 to 240 by using a function called map  [2] this function covert the working range of pot to the working range of the pump as shown in figure 14 after that the mapped value goes to the function that makes the pump work and the value goes to the function Mpumpspeed to display the pump speed on LED bank. 

![image](https://user-images.githubusercontent.com/40359381/201550453-2ddc7efb-181b-48ae-baa3-f6d39322da46.png)

Figure 14[ implementing the code of pump and potentiometer in manual mode]

> The pump in Auto Mode: 

* 	In the Automatic mode, the pump should work automatically by using a simple controller called Bang-bang control discussed before in the automatic algorithm. Before implementing the Bang-bang controller I multiplied the tank current level value by the height of the tank 5 to get the percentage of tank level after that I used this value in the if statement to make the Bang-bang controller, as shown in figure 15, the current level of the tank is less than 10% the pump will work in full speed 240rpm and if the tank level higher than 85% the pump will stop. Also, the pump speed is displayed in the LED bank by using the function Mpumpspeed.   

![image](https://user-images.githubusercontent.com/40359381/201550510-ca85d848-08a6-4039-bdbb-d8a97932e5e5.png)

Figure 15[ implementing the automatic controller for the pump in auto mode]


> 5 -	SW1 and SW2 (Start and Stop buttons):  

* To implement the code to make the user choose to press SW one to start the program or SW two to stop the program we need something to check the state of buttons every iteration in the code  and this thing is the while loop statement as shown in figure 16 inside the while loop there is if statement to check if the button one is pressed or button pressed at the first there is a block of code ask the user to press the SW1 to start or SW2 to stop, and if the user press on button one the program will go in start block code until it finishes it and if the user press on button two the program will stop as shown in figure 16. 

![image](https://user-images.githubusercontent.com/40359381/201550613-5333e4e2-2860-41a2-8b6e-da73417ef409.png)

Figure 16[ the schematic illustrates briefly how the buttons are implemented]

At the first of the code, we defined the button pins as shown in figure 17, the figure 18 explains the implementation of Buttons. And defining the input in void setup function by digitalRead(SW1_PIN, INPUT) and digitalRead(SW2_PIN, INPUT). 

![image](https://user-images.githubusercontent.com/40359381/201550631-2b947d05-dce0-4b13-b371-9502fe04e499.png)

Figure 17[ defining the buttons pins at the first of the code]


<img width="383" alt="image" src="https://user-images.githubusercontent.com/40359381/201550650-2bff2267-14d0-4f64-a777-9f921fd74a4f.png">

The same strategy is implemented in automatic mode. 

> 6-	LCD and Keypad: 

* The LCD works as a user interface for the user which means it displays all data and monitors the system states, etc I used the ready-made function from the DEVBOARD header to display any string to the user for example intonation messages that are displayed at the start of the program for the user as shown in figure 19.  

![image](https://user-images.githubusercontent.com/40359381/201550720-3f51a313-e52b-4e81-b2a0-ae5074c0c64e.png)

Figure 19[ initialization messages]

And I used it to ask the user to enter 1 or 2 for mode selection from the Keypad and I used a ready-made function called DEVBOARD_keypadRead () to store the user input value as shown in figure 20. 

![image](https://user-images.githubusercontent.com/40359381/201550742-3e04b775-913a-485d-8756-c45fc350fa42.png)

Figure 20[ displaying the mode selection for the user]

And to display the mode information such as pump speed in Rpm, time, demanded water, and tank capacity, the (DEVBOARD_lcdSetCursor(x,y)) function is used to set the position for every String and by using DEVBOARD_LcdPrintString(   )) function to display the information on the LCD, the code illustrated on figure 21. 

![image](https://user-images.githubusercontent.com/40359381/201550759-83185d46-4e69-4e58-9ca6-f4a4dd4f1a6d.png)

Figure 21[ the code to display the information during the mode is running]

The result of the code is shown in figure 22.

![image](https://user-images.githubusercontent.com/40359381/201550780-d5d6b188-2863-46b3-95de-9e000a67ae1e.png)

Figure 22[the LCD output during the program running]


> 7-	The Tank level LEDs indicator:

* he yellow LED D2 indicates that the water level in the tank is higher than 85% and the red LED indicates that the water level in the tank is less than 10%, implementing the code of those LEDs the if statement was used in auto and manual mode with the same strategy as shown in figure 23. 

![image](https://user-images.githubusercontent.com/40359381/201550824-b68016b8-4310-45e9-a288-10269558bc6f.png)

Figure 23[ the code that implemented to show the tank level state to the user]

> 8-	LED 5 system ON and LED 7 system Off: 

The LEDs 5 and 7 are related to the LED Bank so to make them work, the function called DEVBOARD_writeLedBank(B10000000); to turn the red LED 5 on when the system stop and DEVBOARD_writeLedBank(B00100000); when the system is run to turn the LED 7 green ON. 

![image](https://user-images.githubusercontent.com/40359381/201550850-030e6c64-82d7-435c-b4db-a933753a2726.png)

> 9-	The initial level of the Tank: 

* In the Manual Mode the initial level is set by default at 75%, on the other hand, in Automatic mode the program asks the user to enter the value between 10% to 85% as the initial level of the tank, to implement the code of this task, the if the statement was used to make a constrain to make sure that the input from user incorrect range 10% to 85% and if the user enters other number  doesn’t in this range the program will show a message that asks the user to enter the correct value between 10% and 85% as shown in figure 25. 

![image](https://user-images.githubusercontent.com/40359381/201550875-fdbea9d6-7f6a-4db3-a030-1e12fff07dd2.png)

Figure 25[ the code of auto mode to ask the user to enter initial tank level]

At the End of this section, I want to rise the point that the code has comments that describe all things on it and all full code is in the Appendix section. 

## Testing the Program and Debugging 

The program went through several testing stages to make sure everything in the program works as it should, on this section the testing Plan of the project and debugging will be demonstrated. 

In the first stage as shown in the following table, the program passes some tests such as displaying messages on LCD, model selection, bang-bang control, and cutdown function. But it fails on other tests, which will be discussed in debugging section.

<img width="429" alt="image" src="https://user-images.githubusercontent.com/40359381/201550953-61d241c5-b305-4b03-9308-f20114b707a6.png">

> 1-	The interruption problem and debug: 

* 	During the first test stage, the interruption button doesn’t respond and interrupted the program, the problem was in the interruption structure I used a strategy that if the flag is not false the program will run, else the program will interrupt, this strategy can work in small or clear form loops programs but it didn’t work in my code because of the code has many and many loops, for example, the for loop that demonstrate the simulation 24 hours once the program goes there it will not detect any interruption because it inside the loop.

* 	So the solution was to make a function called interruption happen this function has the structure of else that mentioned before and we will call this function with a condition like that[if (flag)interption_happen();] what does it mean if the (flag == true) the program go inside this function and we added it inside the loops so when the loop is repeated it will check is there any interruption if yes the program will go inside this function if no the program will continue normally.       
> 2-	The manual mode pump is controlled by Pot problem and debug: 

* 	The problem was when the program enters manual mode, the potentiometer doesn’t change the value of the pump speed, so the potentiometer doesn’t control the pump speed. 

* 	The problem was the code of pot and pump was outside the for loop so when the program run it take the fist value of the pot and enter for loop. 

* 	The solution is to move the code block that make pot control the pump inside the for loop, when the for loop run the pot value will be updated every iteration.  

> 3-	Display the pump speed on LED bank: 

* The problem was the pump speed doesn’t appear on led bank that’s because of I used a different way two display build the function that take the value of pump speed between 10 to 240. 

* The solution was to use a specific ready-made function called DEVBOARD_writeLedBank(Binary number); this function is provided in header file of DEVBOARD and the Led bank is connected by it so there are eight LEDs on the led bank every one of them represented by binary number when you want to turn it ON make the value of it by one and if you want to turn it off make the value of zero and so on. 

> 4-	The Start and stop buttons:   

* The problem was the start and stop buttons didn’t work any more and if they work the start button is work and the stop button doesn’t work, that because of at the first I used a ready made functions called  DEVBOARD_waitForS1Press() and DEVBOARD_waitForS2Press()  and those functions are blocking function that means when I call the first function at the beginning of the code the program will go there and will not do anything until this function is done.

*	So to avoid this problem I made a while loop for each mode the structure is very simple while the user input one that’s mean the program goes to manual mode and this loop checked the states of the buttons every iteration if the button one state low that mean button pressed go there (start the program) else if the button two state low pressed go there (stop the program) and inside this loop I but the interruption happen function with condition as discussed before to check the interruption flag every iteration. 

> 5-	The LED 5 and LED 7 to show the system state on or off:  

* 	The problem was the LEDs 5 and 7 didn’t work that because the way of the implementing them was wrong, those LEDs are belonging to LED bank and I discussed before the LED bank has a function called DEVBOARD_writeLedBank(Binary number); this function is to turn on and off the led.

* 	So, when we need to turn the LED 5 on, we write DEVBOARD_writeLedBank(B10000000); and when we need to turn it off and turn the LED7 on we write DEVBOARD_writeLedBank(B00100000); and etc. 

* The final test stage: After changing all incorrect things that discussed below, we are testing the system again and we found the program working as it should and the interruption is working at any time in the program, and the program was tested as shown in following table.  

<img width="399" alt="image" src="https://user-images.githubusercontent.com/40359381/201551141-345490ea-4fb9-4a1b-a3d8-6d13e901b741.png">

## Conclusion 

In The paper, the program written by Arduino C programming language for water distribution system is addressed, at the beginning of the paper a brief introduction talk about the system and how it works and the target hardware (DEVBOARD) that the program will be implemented on it and description of component of this hardware, the second part is a specification requirements this part discuss the user requirements according to the hardware and working of the program and how it should behave and summarise the requirements in a table that describe every part function, the third part is talking about the Algorithms that used to build the program and described them in flowcharts. The fourth part is major code description and how they implemented, the fifth part include the test plan and the problems that faced us during the test plan and debugging them. Also, the full code is attached in Appendix part.  


## References
[1] Anonymous "Map() - arduino reference", [Online] Available from: https://www.arduino.cc/reference/en/language/functions/math/map/ .

[2] arduino, "Map function ", [Online] Available from: https://www.arduino.cc/reference/en/language/functions/math/map/ .

[3] Laboratory 4-6 -assignment instructors: Daniel paluszczyszyn (paluszcol@dmu.ac.uk), dimitry dmitriy dubovitskiy (dmitriy.dubovitskiy@dmu.ac.uk) ENGT5203 microprocessors applications version:1.1 de montfort university, 2022.


