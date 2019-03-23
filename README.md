# battleship
Socket based battleship

How To Launch:
===============================================================================
  Makefile Commands:
  -----------------------------------------------------------
    Make Client exec: <make client>
    Make Server exec: <make server>
    Clean Executables: <make clean>

  Launch Server:
  -----------------------------------------------------------
  ./bs_server

  Launch Client (two instances):
  -----------------------------------------------------------
  ./bs_client <IP_ADDRESS> <PORT_NUMBER>

    ARGV:
    ---------------------------------------------------------
    IP_ADDRESS  : Host Ip address or local loop back (127.0.0.1)
    PORT_NUMBER : 4700

Rules:
===============================================================================
- Place Ship
- Fire
- Sink enemy ship < 3 units >
- Win ( or lose if you're bad ;) )

!!! NOTE !!!
-------------------------------------------------------------
  The game is turn based. This means your actions are limited
  to the opposing player. (ie if you place your ship you will
    not be able to fire until the opposing user places their
    ship. This applies to firing your shots.)

How is player 1 and player 2 determined?
-------------------------------------------------------------
  Player 1 => first user connect to server  
  player 2 => second user to connect to server  

Key Bindings:
===============================================================================
1) Placing initial Ship Steps:
--------------------------------------------------------------

    *ref* -> Move Around (Loop back if ESC_KEY Pressed during placement)
    KEY_LEFT  : move left
    KEY_RIGHT : move right
    KEY_UP    : move up
    KEY_DOWN  : move down

    SPACE (1st press) : place initial position then use the
                        left and right arrow keys to rotate
                        ship.
    ----------------------------------------------------------
    KEY_LEFT  : rotate ship <counter clock wise>
    KEY_RIGHT : rotate ship <clock wise>

    ESC_KEY   : step back to placing initial position
                (reverts back to step *ref* -> Move Around)
                note after user presses ESC_KEY he or she
                can then use the four arrow keys to move
                around to place initial piece again.

    SPACE(2nd press) : confirm ship placement after ship has
                        been rotated to desired possition

2) Firing On Enemy Steps:
--------------------------------------------------------------
    KEY_LEFT  : move left  
    KEY_RIGHT : move right  
    KEY_UP    : move up  
    KEY_DOWN  : move down  

    SPACE_BAR : Fire  

3) Endgame/Show Winner Screen:
--------------------------------------------------------------
    Q_KEY : exit game (q key on keyboard)
            (all you can do during this screen is exit the game
            by hitting the 'q' key)
