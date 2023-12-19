# Bomberman
This is the Introduction to Robotics matrix project, taken in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest. It involves making a game on a 8x8 matrix of LEDs and an LCD screen using a joystick.
I chose Bomberman because i think it is a game that suits the tasks perfectly. Before making it i played online to understand the game better and to make the functionalities as close as the original.

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
