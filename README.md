# Snake-Eternal
Multiplayer Snake game for the LaunchPad Tiva C EK-TM4C123GXL microcontroller with an Adafruit 320x240 pixel ST7789 display. This project was created for the Mech 405 class as Washington State University in 2020.  

![MainImage](/images/MainImage.png)

# The Game
This is a 1-4 player version of the game snake. Player 1 may select the number of players and the game board size using their joystick and button0 on the microcontroller.  
For a single player game, the goal of the game is to eat yellow food pellets and avoid the walls and yourself. If the player reaches the size of the game board, they win the game.  
For a multiplayer game, the goal of the game is the same, however with the added obstacle of other players. The last player remaining on the board, wins.

# Putting It Together
### Bill of Materials
* 1 - Tiva C EK-TM4C123GXl (https://www.digikey.com/reference-designs/en/open-source-mcu-platforms/2554?utm_adgroup=xGeneral&utm_source=google&utm_medium=cpc&utm_campaign=Dynamic%20Search_EN_Product&utm_term=&utm_content=xGeneral&gclid=Cj0KCQjwmcWDBhCOARIsALgJ2QeFPy7R3bFkXMDxtO4tFqAgARe4f10my_QNgzrPlGeBeSdb-ExWlZ8aAnEvEALw_wcB)
* 4 - 2-axis Analog Thumb Joysticks (https://www.adafruit.com/product/512?gclid=Cj0KCQjwmcWDBhCOARIsALgJ2QdTtyHgDw4a-RWpFLWy5ZlMLKZVwrNl4ITmCVAIiMSWxRMarkZ_VQIaAqYtEALw_wcB)
* 1 - 320x240 ST7789 Display (https://www.adafruit.com/product/4311)

Other microcontrollers, joysticks, and SPI screens can be substituted, but compatiblity is no guaranteed. 

### Wiring

![Wire Diagram](/images/WireDiagram.jpg)  
(Wire Diagram, including 4 joysticks.)  
Using the parts suggested in "Bill of materials", the wiring is as follows:
#### TFT Display
The TFT Display is being used over SPI.  
HostPin (Alt Name) -> ScreenPin  
11 (PA_2) -> SCK  
8  (PA_5) -> MOSI  
12 (PA_3) -> CS  
24 (PD_1) -> RST  
25 (PD_2) -> DC  
23 (PD_0) -> BL  

#### Joysticks
The each joystick has two input pins, one for the X direction and one for th Y direction.
There are 4 joysticks total.   

{HostPinX, HostPinY} -> Joystick #  
{29 (PE_3), 28 (PE_2)} -> Joystick 1  
{27 (PE_1), 26 (PD_3)} -> Joystick 2  
{2  (PB_5), 5  (PE_4)} -> Joystick 3  
{6  (PE_5), 7  (PB_4)} -> Joystick 4  

### Programming
#### IDE
The Tiva C microcontroller can be programmed using Energia, a free Integrated Developement Environment for the board.  
  
Energia can be downloaded here: https://energia.nu/download/  
Once Energia is installed, go to:  
Tools -> Board: -> Boards Manager  
and install Energia Tiva boards from the list.
  
Once installed, go to:
Tools -> Board:  
and select " LaunchPad (Tiva C) w/ tm4c123 (80MHz)"  

This project was later programmed in Visual Studio, however it is not required for this project.
#### Libraries
The required libraries are available in the [libraries/](libraries) directory.  
Place all files in [libraries/](libraries) into \Documents\Energia\libraries on your local computer, or wherever your Energia library folder is.  

The AdaFruit_GFX_Library and the Adafruit_ST7735_and_ST7789_Library were modified to work with the Tiva C, and the spiE-TIVAC-SPI library is no longer available from its original location. (Credit to AdaFruit for the AdaFruit_GFX_Library and AdaFruit_GFX_Library AdaFruit_GFX_Library libraries; and to https://github.com/Paul47 for spiE-TIVAC-SPI)

#### Installing the Program
To install the program, download [source/SnakeGame_Beta_0.07_4-16-2020.ino](source/SnakeGame_Beta_0.07_4-16-2020.ino) and open it with Energia. Connect the Tiva C via USB to your computer. Once connected, go to:   
tools -> Port  
and select the port your microcontroller is plugged into.  
Then click "upload".  
Some troubleshooting may be required.  
  
The source code from Visual Studio can be found in source. [source/](source) 

# License
Copyright 2020 Zachariah Weber, Alixander Richards-Thompson, Clayton Walhstrom

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
