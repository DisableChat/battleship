# battleship
Socket based battleship

How To Launch:
===============================================================================
  Makefile:
  -----------------------------------------------------------
    <make client>
    <make server>
    <make clean>

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
    *ref* -> Move Around
    KEY_LEFT  : move left
    KEY_RIGHT : move right
    KEY_UP    : move up
    KEY_DOWN  : move down

    SPACE (1st press) : place initial position
    ----------------------------------------------------------
    KEY_LEFT  : rotate ship <counter clock wise>
    KEY_RIGHT : rotate ship <clock wise>

    ESC_KEY   : step back to placing initial position
                (reverts back to step *ref* -> Move Around)

    SPACE(2nd press) : confirm ship placement

2) Firing On Enemy Steps:
--------------------------------------------------------------
    KEY_LEFT  : move left  
    KEY_RIGHT : move right  
    KEY_UP    : move up  
    KEY_DOWN  : move down  

    SPACE_BAR : Fire  

3) Endgame:
--------------------------------------------------------------
    ESC_KEY : exit game
