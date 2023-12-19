# Bomberman
This is the Introduction to Robotics matrix project, taken in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest. It involves making a game on a 8x8 matrix of LEDs and an LCD screen using a joystick.
I chose Bomberman because i think it is a game that suits the tasks perfectly. Before making it i played online to understand the game better and to make the functionalities as close as the original.

### Youtube link presenting the homework: <a href= "https://youtu.be/mV54f1vsJz8?si=SKThWqmz0ILGHrO_"> Click here </a>

### Components Used:

* 1x 8x8 LED Matrix
* 1x Arduino Uno
* 1x Half breadboard
* 2x Mini breadboard
* 1x Joystick
* 1x Button
* 1x Led
* 1x Buzzer
* 1x MAX7219
* 1x LCD display
* resistors and capacitors as needed
* many wires

### Technical Task:
Menu Task: Create a menu for your game, emphasis on ‘the game. You should scroll on the LCD with the joystick. The menu should include the following functionality:
1. Intro Message - When powering up a game, a greeting message
should be shown for a few moments.
2. Should contain roughly the following categories:
<ul>
<li> Start game, starts the initial level of your game
<li> Highscore:
  <ul>
<li> Initially, we have 0.
<li> Update it when the game is done. Highest possible score
should be achieved by starting at a higher level.
<li> Save the top 3+ values in EEPROM with name and score.
    </ul>
<li> Settings:
  <ul>
<li> Enter name. The name should be shown in highscore. Maybe
somewhere else, as well? You decide.
<li> LCD brightness control (mandatory, must change LED wire
that’s directly connected to 5v). Save it to eeprom.
<li> Matrix brightness control (see function setIntesnity from the
ledControl library). Make sure to display something on the
matrix when selecting it. Save it to eeprom.
<li> Sounds on or off. Save it to eeprom.
<li> Extra stuff can include items specific to the game mechanics,
or other settings such as chosen theme song etc. Again, save
it to eeprom. You can even split the settings in 2: game
settings and system settings.
    </ul>
<li> About: should include details about the creator(s) of the game.
At least game name, author and github link or user (use scrolling
text?)
<li> How to play: short and informative description
</ul>
3. While playing the game: display all relevant info
  <ul>
<li> Lives
<li> Level
<li> Score
<li> Time?
<li> Player name?
<li> etc
  </ul>
4. Upon game ending:
<li> Screen 1: a message such as ”Congratulations on reaching level/score
X”. ”You did better than y people.” etc. Switches to screen 2
upon interaction (button press) or after a few moments.
<li> Screen 2: display relevant game info: score, time, lives left etc.
Must inform player if he/she beat the highscore. This
menu should only be closed by the player, pressing a button.

### Description of the game:
There is a player that should go from the top left corner to the bottom right corner, then a new map is generated. To achieve that the player should destroy the walls by putting bombs next to them. The bomb destroys one wall from each side (up, down, left, right). When a bomb is planted the player has 3 seconds to move from next to it or the game restarts and a new map is generated. There is a led that warns the player that he is next to the bomb. To win the game the player should complete the map 2 times. It has 3 lives in the beggining which are shown on the LCD display and every time the player plants a bomb and doesn't move dies, loses a life, a new map is generated and the game goes from the beggining (even though the player completed the map one time it doesn't count anymore).

### How to play
<ul>
  <li> Move player with joystick
  <li> Plant bombs by pressing the joystick
  <li> The bomb destroys one wall each direction (up, down, right, left)
  <li> The bomb kills the player if the led is on
  <li> There are 3 lives
  <li> To win or to level up go to top left corner 
  <li> To exit the game press the button
</ul>

### Photos of functionalities:
<ul>
<li> Intro message
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/e4f2d8a6-e400-40de-bb4e-531b8283ecc0" width="49%" height="500">
</p>

<li> Game
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/a9fb5483-6229-4296-ad74-83ef83023ccd" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/99b10065-f6fc-4a37-9f4d-de98e0a34303" width="49%" height="500">
</p>
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/64481d1a-c25b-4371-a67f-671e7bfb2762" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/1d347a59-275b-485b-9df7-d81f897225aa" width="49%" height="500">
</p>

<li> Highscores
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/bd04b22b-6a5b-46aa-a57f-0f5cbb17f229" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/b239dd7b-1d43-469c-bb6a-379aa8a15b7b" width="49%" height="500">
</p>

<li> How to play
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/7faa6a72-c146-4ef8-b9e1-ff771c2aa17f" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/d1a58ae9-d782-49a9-b799-ae0a11d278f1" width="49%" height="500">
</p>

<li> About
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/454edd92-bcc8-4304-8072-3aeb543cdade" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/32bfde9d-9853-4841-86d0-68c7b72078ba" width="49%" height="500">
</p>

<li> Choose name
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/0ef34aa8-d6ab-41e5-9402-49b69d0c98d7" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/e821cb5d-b056-45de-9b71-d3916da6e06a" width="49%" height="500">
</p>

<li> Brightness LCD
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/0929cb87-be64-42cd-90cb-9066c4c901da" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/12a26c45-49f9-4571-a8e1-022689335f4b" width="49%" height="500">
</p>

<li> Brightness Matrix
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/149494d3-9dd6-4e15-b575-8202ae82cb3a" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/2d949d0b-c182-4a56-8d89-b566b88dfc17" width="49%" height="500">
</p>

<li> Sound ON/OFF
<p float = "left">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/23cb0602-941d-4ef3-a4ed-577ad0878a16" width="49%" height="500">
<image src = "https://github.com/corinagherasim/Bomberman/assets/94368761/e37be261-615d-4b9a-85e7-29869525d246" width="49%" height="500">
</p>

<li> Reset highscores
<p float = "left">
<image src = "" width="49%" height="500">
<image src = "" width="49%" height="500">
</p>



