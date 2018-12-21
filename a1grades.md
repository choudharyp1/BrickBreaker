# CS349 A1
Student: p2choudh
Marker: Sung-Shine Lee


Total: 88 / 100 (88.00%)

Code: 
(CO: won't compile, CR: crashes, FR: UI freezes/unresponsive, NS: not submitted)


Notes:   

## GAME REQUIREMENTS

1. [5/5] There are at least 5 rows of coloured blocks at the top of the screen.


2. [8/8] There is a paddle at the bottom of the screen that can be moved left or right with the keyboard.


3. [5/5] When the game starts, the ball starts to move across the screen.


4. [8/8] The ball bounces off the side and top walls and the paddle, roughly conforming to the reflection law.


5. [2/6] If the ball hits a block, the block disappears and the ball bounces off.

-2 Sometimes, the ball does not bounce after hitting a block
-2 Sometimes a collision is not detected

6. [6/6] The game ends when all blocks are cleared or the ball touches the bottom of the screen. The user should be prompted to play again or quit the game. A new set of blocks (5 new rows) is loaded if the user chooses to continue.


7. [5/5] There is a scoring system that rewards bounces of the ball or hits of the block. The score is displayed and updated in real-time.


8. [2/2] There is a user interface control (e.g. a button) or a keyboard command to quit the game.


## TECHNICAL REQUIREMENTS

1. [5/5] There is a makefile. Issuing "make" builds the program. [Optionally: "make run" builds and executes].


2. [2/2] There is a readme file describing the program and any additional feature.


3. [5/5] The program opens with a splash screen, which displays the student's name and ID and instructions for playing (e.g. key usage).


4. [5/5] The programs takes in a parameter for adjusting frames per second (FPS). The program was tested with 10, 30, 45, and 60 FPS.


5. [5/5] The programs takes in a parameter for adjusting the ball speed. The program was tested with the values 1, 3, 7, and 10.


6. [4/4] The frame rate and the speed of the ball are changed independently.


7. [0/2] When the frame rate is 30 FPS, there is no lag when playing the game.

-2 Game is laggy at 30 FPS.

8. [2/2] The window is 1280x800 pixels and does not need to be resized.


9. [5/5] Double-buffering is used to eliminate "stuttering" in gameplay.

10. [5/5] The program does not consume too much CPU time (< 50% of CPU time).


## DESIGN AND AESTHETICS

1. [3/5] The interface design is aesthetic.

2. [1/5] The gaming experience is smooth and enjoyable.

3. [5/5] Intuitive mouse and/or keyboard controls are used as game input.
