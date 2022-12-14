Gem Drop X README
-----------------
Gem Drop X
version 0.9

by Bill Kendrick
(c) New Breed Software
bill@newbreedsoftware.com
http://www.newbreedsoftware.com/gemdropx/

August 1997 - May 18, 2002


WHAT IS GEM DROP X?
-------------------
  COMPARED TO OTHER GAMES
  -----------------------
    "Gem Drop X" is an interesting one-player puzzle game using the
    Simple DirectMedia Layer (SDL) libraries.

    It is a direct port of "Gem Drop," an Atari 8-bit game written in Action!
    (a very fast C- and Pascal-like compiled language for the Atari).

    It was originally ported to X11, using SDL for sound and music.
    Eventually, the Xlib graphics calls were removed and replaced with
    SDL calls.

    The concept of the game "Gem Drop" is based on an arcade game for the
    NeoGeo system called "Magical Drop III" by SNK.

    If you're familiar with games like Jewels, Klax, Bust-A-Move or Tetris,
    this game is similar to them all.  I consider it closest to Klax.
    Some people have compared it to "Tetris meets Space Invaders."

  HOW IT WORKS
  ------------
    The game is played with YOU at the bottom of the screen.  At the
    top is a random assortment of colored shapes ("gems").  As time
    goes on, more gems appear at the very top of the screen, pushing the
    rest downwards.  The game is over when the gems reach the bottom.

    You'll know the gems are about to come down because a flashing light
    appears at the bottom of the screen.  (Note that before the gems
    start moving in a level, you get a little breathing room.  For example,
    if it typically takes 10 seconds before gems move in the level you're
    on, it'll take 20 seconds for them to START moving, then they'll drop
    every 10 seconds from then on.)

    It's your job to keep the screen from filling up.  You do this by
    "grabbing" gems from the top of the screen, carrying them around,
    if need be, and "throwing" them back up.  You automatically grab as
    many of the same gem as you can that are above you.  You can carry
    as many of the same gem as you want.  You know how many you're carrying
    by how many you see next to your character at the bottom of the screen.
    If you see one, you're carrying one.  If you see two, you're carrying
    two.  If you see three, you're carrying three OR MORE.  (It doesn't
    matter how many, since the moment you throw, you'll make a match,
    explained below.)

    If, when you throw some gems back up, you create a "match" of 3 or more
    gems in a vertical column, they disappear (with a cool little explosion).
    At this point, if there are any "matches" of the same gem to the left or
    right of this column, they disappear too!  And so on.  This is great for
    cool chain-reactions! You also get more points for the more gems that
    disappear in a match.  (Matching four gives as many points as matching
    three twice, for example.)

  LEVEL ADVANCEMENT
  -----------------
    A "match" (no matter how few (minimum of 3, of course), or how many
    gems disappear) is also known as a "line."  You beat a level of the game
    (there are 20 in all) by getting enough lines.  Each level requires
    you to get more lines than the last.

    Each level also starts out with more gems at the top of the screen,
    which means you must worry about dying more immediately!

    Each level is also FASTER!

  SPECIAL OBJECTS
  ---------------
    But, there's hope!  Three special objects help you along the way.
    These special objects can't be grabbed and thrown.  They are activated
    by including them as part of a match of three or more.  (If you're
    carrying two gems, you can throw them on a special object to activate
    it, for example.)

    The objects are:

    * BOMB - These destroy the four gems adjacent to it.  (Above,
             below, to the right, and to the left.)

    * CLOCK - This temporarily stops more gems from appearing on the
              screen.  You'll love this one in the higher levels!
              A clock icon appears at the bottom of the screen while
              the clock is activated.

    * WILDCARD - (A questionmark.)  This matches with all adjacent gems,
                 whether or not they match the line you just created.  This
                 sometimes makes for AWESOME chain reactions!

  LEVEL 15 AND BEYOND
  -------------------
    There are 20 levels in Gem Drop X.  The first 15 contain four gem
    shapes.  The last five contain eight shapes (which makes it
    extremely difficult to get matches quickly).  Level 15 starts a
    little slower, and with gems a little higher than level 14, to
    give you a little break.  But don't get used to it!

    After you beat level 20, you've won!  I bet you can't do it, though!


PLAYING GEM DROP X
------------------
  PROGRAM OPTIONS
  ---------------
    First off, you can get a quick help page by invoking the program with
    the "--help" switch:

        ./gemdropx --help

    You can also find out what version you have with "--version":

        ./gemdropx --version

    Finally, if you want to remind yourself what command line arguments
    "Gem Drop X" accepts, use "--usage":

        ./gemdropx --usage

    If sound is enabled and you wish to disable music, use "--nomusic":

        ./gemdropx --nomusic


  STARTING A GAME
  ---------------
    Start a game of Gem Drop X up by typing the following at a shell
    prompt:

        ./gemdropx

    The Gem Drop X window should appear.  It should be about 625 pixels
    high, so it should fit on any good display's monitor.  On 800x600
    screens, you'll still be able to see everything important, if you
    position the window carefully.

  THE TITLE SCREEN
  ----------------
    In the title screen, you can press [SPACE] or [RETURN] to begin the
    game at the selected level (displayed at the bottom),

    ...or you can press [L] to change the level (between 1 and 10 or
    the highest level you've beaten, whichever is higher).

    ...or you can press [Q] or [ESCAPE] to quit Gem Drop X.

  THE GAME
  --------
    KEYBOARD
    --------
      While in the game, you can use the arrow keys in the following way:

        LEFT / RIGHT  -  Move the man left and right.  (He will wrap around
           K / L         at the edges of the screen.)

        DOWN / UP     -  Grab gems and throw gems.
           Z / A

        RETURN        -  Cause a row of gems to come down immediately.

    MOUSE
    -----
      You can also use the mouse:

        Clicking Left Or Right Of The Man      -  Moves the man left or right.
        Left-Clicking Anywhere Above The Man   -  Grabs gems.
        Right-Clicking Anywhere Above The Man  -  Throws gems.

    JOYSTICK
    --------
      LEFT / RIGHT    - Move the man left and right.
       FIRE #1        - Throw
       FIRE #2        - Grab

    GAME CONTROLS
    -------------
      * You can abort the game by pressing [Q] or [ESCAPE].
        (It's like "game over")

      * You can pause the game by pressing [P] or [SPACE].
        Press a key to continue.

  VISUAL CUES
  -----------
    Some things to pay attention to while playing the game:

      * How many lines you need.  How many you have made already.
      * The flashing light at the bottom of the screen - it means
        more gems are about to come down at you!
      * The look on the man's face - happy means you're doing alright;
        sad/frightened means you're getting close to losing!


COMMENTS OR QUESTIONS?
----------------------
  CONTACTING US
  -------------
  If you have any suggestions, ideas, comments, praise,
  or donations for Gem Drop X, please send them by!

      New Breed Software
      c/o Bill Kendrick
      675 Alvarado Ave #27
      Davis, CA 95616-0620 USA

      e-mail: bill@newbreedsoftware.com
      web:    http://www.newbreedsoftware.com/gemdropx/

  LINUX GAME TOME!
  ----------------
    If you like Gem Drop X, please take a moment to review Gem Drop X at
    the Linux Game Tome:

      http://www.happypenguin.org/show?gem+drop+x

Thank you!
