//******************************************************************************
// File:					bs_client.cc
// Author:				Wesley Ryder
// Date:					3/20/19
// Description:		This is the battleship client, where the user controls
//								his/hers ship, and firing locations to try and sink enemy
//								ship. This implementation uses Sockets to make a client
//								and server based application. Where the server is essentialy
//								the ref and the cients being the players
//******************************************************************************

#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <ncurses.h>

using namespace std;
using boost::asio::ip::tcp;


// Creating Global Variales
const int BOARD_ONE_OFFSET	= 1;
const int BOARD_TWO_OFFSET	= 12;
const int BOARD_BOTTOM			= 24;
const int ATACK_START_Y			= 13;
const int ATACK_START_X			= 1;
bool GAMEOVER 							= false;



//******************************************************************************
// Function:      check_repeat_fire()
// Arguments:     vector<vector<int> > &enemy_board
//								int x
//								in y
// Description:   Checks if the firing location has already been fired on where
//								true:		Fired on this location already
//								false: 	Have not fired on this location thus far
// Return Val:    bool
//******************************************************************************
bool check_repeat_fire(vector<vector<int> > &enemy_board,
												int x,
												int y)
{
	if (enemy_board[y][x] == 3 || enemy_board[y][x] == 2)
	{
		return true;
	}
	else{return false;}
}

//******************************************************************************
// Function:      update_board()
// Arguments:     vector<vector<int> > &board
//								vector<vector<int> > &enemy_board
//								int x
//								int y
//								string answer
//								string &anoucment
// Description:   updates boards after reicieving information based on previous
//								turn, this updates both client side's ally board and enemy
//								board
// Return Val:    None
//******************************************************************************
void update_board(vector<vector<int> > &board,
												vector<vector<int> > &enemy_board,
												int x,
												int y,
												string answer,
												string &anoucment)
{

	// Declaring local vars
	string tmp;
	string endDelimiter = "*";
	string delimiter		= "\n";
	string delimiter2 	= "-";
	int hit 						= -1;
	int pos 						= 0;
	int score 					= 0;
	int tmp_pos 				= 0;

	// Parsing score
	tmp = answer.substr(pos, answer.find(delimiter));
	score = stoi(tmp);

	// If not game over (-1 is a unique indentifier in this case)
	if(score != -1)	{
		// Updating client enemy board
		enemy_board[y][x] = score;

		int x2;
		int y2;

		// The proceeding code just parses the cordinates and hit value
		tmp_pos = answer.find(delimiter);
		pos = answer.find(delimiter2, tmp_pos);
		tmp = answer.substr(tmp_pos, pos - tmp_pos);
		y2 = stoi(tmp);

		tmp_pos = pos + 1;
		pos = answer.find(delimiter2, tmp_pos);
		tmp = answer.substr(tmp_pos, pos - tmp_pos);
		x2 = stoi(tmp);

		tmp_pos = pos + 1;
		pos = answer.find(delimiter2, tmp_pos);
		tmp = answer.substr(tmp_pos, pos - tmp_pos);

		hit = stoi(tmp);
		board[y2][x2] = hit;

	}
	// Game over case
	else
	{
		pos += answer.find(delimiter) + 1;
		enemy_board[y][x] = 2;
		anoucment = answer.substr(pos, answer.find(endDelimiter) - 3);

		// I will admit this section right here is bad because I added cordinates
		// to end mssg after end game siuation when I already add them to the string
		// server side but if the games over for some reason I add them to the end.
		// Idk what I was thinking at the time.
		// REFER TO REFERENCE *REF* on server.cc
		string tmp_x;
		string tmp_y;
		string tmp_h;
		tmp_y = answer.substr(answer.find(endDelimiter) + 1, 1);
		tmp_x = answer.substr(answer.find(endDelimiter) + 3, 1);
		tmp_h = answer.substr(answer.find(endDelimiter) + 5, 1);
		board[stoi(tmp_y)][stoi(tmp_x)] = stoi(tmp_h);

		GAMEOVER = true;
	}
}

//******************************************************************************
// Function:      get_ship_place_cor()
// Arguments:     vector<vector<int> > &board
// Description:   Gets the ship placement cordinates and returns it as a string
// Return Val:    string
//******************************************************************************
string get_ship_place_cor(vector<vector<int> > &board)
{
	string coridinates		= "";

	string x = "";
	string y = "";

	for (int i=0;i<4;i++) {
		for (int j=0;j<4;j++) {
			if( board[i][j] == 1){
				x = to_string(i);
				y = to_string(j);
				coridinates.append(x).append("-").append(y).append("-");
			}
		}
	}
	coridinates.pop_back();
	coridinates.append("\n");
	return coridinates;
}

//******************************************************************************
// Function:      ship_clean()
// Arguments:     vector<vector<int> > &board
//								int x
//								int y
// Description:   Sets values of previous ship placement to 0 ie empty
// Return Val:    None
//******************************************************************************
void ship_clean(vector<vector<int> > &board, int x, int y)
{
	for (int i=0;i<4;i++) {
		for (int j=0;j<4;j++) {
			if( board[i][j] == 1){
				board[i][j] = 0;
			}
		}
	}
	board[y][x] = 1;
}

//******************************************************************************
// Function:      check_move()
// Arguments:     int x
//								int y
//								int rotation
// Description:   Checks if the rotation of the ship is a valid option where
//								true:		valid option
//								false:	invalid option
// Return Val:    bool
//******************************************************************************
bool check_move(int x, int y,int rotation) {
	switch(rotation) {
		case 0:
			if((x + 2) >= 4 || (y -2) < 0){return false;}
			else{return true;}
		case 1:
			if((x + 2) >= 4){return false;}
			else{return true;}
			break;
		case 2:
			if((x + 2) >= 4 || (y + 2) >= 4){return false;}
			else{return true;}
			break;
		case 3:
			if((y + 2) >= 4){return false;}
			else{return true;}
			break;
		case 4:
			if((x -2) < 0 || (y + 2) >= 4){return false;}
			else{return true;}
			break;
		case 5:
			if((x - 2) < 0){return false;}
			else{return true;}
			break;
		case 6:
			if((x - 2) < 0 || (y - 2) < 0){return false;}
			else{return true;}
			break;
		case 7:
			if((y - 2) < 0){return false;}
			else{return true;}
			break;
		case 8:
			if((x+2) >= 4 || (y - 2) < 0){return false;}
			else{return true;}
			break;
	}
}

//******************************************************************************
// Function:      draw_matrix()
// Arguments:     vector<vector<int> > &board
//								int cur_row
//								int cur_col
//								int y_start (offset for second board)
// Description:   Draws the grid
// Return Val:    None
//******************************************************************************
void draw_matrix(vector<vector<int> > &board,
		     int cur_row,
		     int cur_col,
			 	 int y_start ) {

		for (int j=0;j<4;j++) {
    	move(y_start, 2*j);
     	printw("+-");
   }
   move(y_start, 2*4);
   printw("+");
 	for (int i=0;i<4;i++) {
   	for (int j=0;j<4;j++) {
    	move(2*i+1+y_start, 2*j);
     	printw("|");
     	move(2*i+1+y_start, 2*j+1);
     	if (board[i][j] == 0) {
				printw(" ");
     	} else if(board[i][j] == 2){
				printw("H");
			}
			else if(board[i][j] == 3) {
				printw("M");
			}
			else {
				printw("X");
     }
   	}
   	move(2*i+1+y_start, 2*4);
   	printw("|");

   	for (int j=0;j<4;j++) {
     	move(2*i+2 + y_start, 2*j);
     	printw("+-");
   	}
   	move(2*i+2+y_start, 2*4);
   	printw("+");
 	}
 	move(2*cur_row+1+y_start, 2*cur_col+1);

}

//******************************************************************************
// Function:      main()
// Arguments:     int argc
//								char* argv[]
// Description:  	Basically the meat and potatoes of the client, where main
//								calls the nessary functions to setup the game and send/recieve
//								the information from server regarding the state of the game
//								and updates the local side for the user
// Return Val:    None
//******************************************************************************
main(int argc, char* argv[]) {
	// Declarying a lot of local variables which is probably bad rip
  int rows;
  int cols;
  int cur_row=0;
  int cur_col=0;
  int ch;

	int ch2;
	int rotation					= 0;
	bool turn							= true;
	bool ship_placement		= false;
	bool valid_placement	= true;
	int x									= 0;
	int y									= 0;
	bool continue_gate 		= false;
	bool exit_console			= false;
 	int exit_;

	string anoucment 					= "";
	string p_v 								= "?";
	const char* player_number = "Player ?";
	const char* numVal;


	// Blob information
	string ship_placement_cor	= "";
	int x_cord_ally		= 0;
	int y_cord_ally		= 0;
	int x_cord_enemy	= 0;
	int y_cord_enemy	= 0;

	// Setting up empty boards for ally and enemy
  vector<vector<int> > board;
	vector<vector<int> > enemy_board;
  for (int i=0;i<4;i++) {
    vector<int> t;
    for (int j=0;j<4;j++) {
      t.push_back(0);
    }
    board.push_back(t);
		enemy_board.push_back(t);
  }

	// Setting up client connection
	int portno = atoi(argv[2]);
  // Standard boost code to connect to a server.
  // Comes from the boost tutorial
  boost::asio::io_service my_service;
	tcp::resolver resolver(my_service);

	// Find the server/port number.
	//  tcp::resolver::results_type endpoints = resolver.resolve(argv[2], argv[3]);
	tcp::socket socket(my_service);
	socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(argv[1]),portno));

  // Screen initialization
  initscr();
  // Clear the screen
  clear();
  // Get the size of the window!
  getmaxyx(stdscr,rows,cols);
	// Pass all characters to this program!
  cbreak();
	// Grab the special keys, arrow keys, etc.
  keypad(stdscr, TRUE);

  // Paint the row and column markers.
  // paint_markers(rows,cols,10,0,0);
  // Redraw the screen.
  refresh();

	// Drawing initial boards with added pizaz
	mvprintw(0, 0, "+ Allies +");
	mvprintw(BOARD_TWO_OFFSET - 1, 0, "- Enemy -");

	// Display player number
	mvprintw(0, 12, "==============");
	mvprintw(1, 12, "][");
	mvprintw(1, 15, player_number);
	mvprintw(1, 24, "][");
	mvprintw(2, 12, "==============");
	refresh();

	// Draw the enemy and ally board
	draw_matrix(enemy_board, 0, 0, BOARD_TWO_OFFSET);
  draw_matrix(board,0, 0, BOARD_ONE_OFFSET);

  // I/O Loop
  // Stop when the q Key is hit.
  while (exit_console == false && (ch = getch())!='q') {
    switch (ch) {
    case ' ':
			turn = true;
			x = cur_col;
			y = cur_row;

			// Clearing section of screen for case when user repeat fires
			mvprintw(BOARD_BOTTOM - 2, 0,  "                              ");
			mvprintw(BOARD_BOTTOM - 1, 0,  "                              ");
			mvprintw(BOARD_BOTTOM, 0,  "                              ");

			// Handle based on if ship is placed yet or not
			if (!ship_placement) {
				draw_matrix(enemy_board, 0 ,0, BOARD_TWO_OFFSET);
				board[cur_row][cur_col]= 1;
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
				refresh();
			}
			// Fire on enemy handle
			else
			{
				if(p_v == "1"){mvprintw(1, 22, "1");}
				else if(p_v == "2"){mvprintw(1, 22, "2");}
				if(!check_repeat_fire(enemy_board, x, y))
				{
					// local vars
					string target_loc	= "";
					string tmp_x 			= "";
					string tmp_y			= "";

					// Draw and send information on fire location
					draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
					enemy_board[cur_row][cur_col]= 1;
					draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
					refresh();
					tmp_y = to_string(cur_row);
					tmp_x = to_string(cur_col);
					target_loc.append(tmp_y).append("-").append(tmp_x).append("\n");
					boost::asio::write( socket, boost::asio::buffer(target_loc) );


					// Get the response from the server
					boost::asio::streambuf response_value;
					boost::asio::read_until( socket, response_value, "\n" );
					string answer = boost::asio::buffer_cast<const char*>(response_value.data());

					// Update boards and draw boards based on recieved information from
					// Server regarding the shot user just took
					update_board(board, enemy_board, x, y, answer, anoucment);
					draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
					draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);

					// If Game over display end game
					if (GAMEOVER == true)
					{
						const char *endG_anoucment;
						endG_anoucment = anoucment.c_str();
						mvprintw(BOARD_BOTTOM - 2, 0, "#################");
						mvprintw(BOARD_BOTTOM - 1, 0, "# ");
						mvprintw(BOARD_BOTTOM - 1, 2, endG_anoucment);
						mvprintw(BOARD_BOTTOM - 1, 16, "#");
						mvprintw(BOARD_BOTTOM, 0, "#################");
						move(BOARD_BOTTOM - 3, 8);

						// Once game is over Make user only be able to hit q to quit
						while((exit_ = getch())!='q')
						{
							mvprintw(BOARD_BOTTOM - 3, 8, " ");
							move(BOARD_BOTTOM - 3, 8);
						}
						exit_console = true;
					}
					refresh();

				}
				// If user trys to fire on already fired location handle
				else
				{
					mvprintw(BOARD_BOTTOM - 2, 0, "##############################");
					mvprintw(BOARD_BOTTOM - 1, 0, "# ");
					mvprintw(BOARD_BOTTOM - 1, 2 , "!!! Already Fired Here !!!");
					mvprintw(BOARD_BOTTOM - 1, 29, "#");
					mvprintw(BOARD_BOTTOM, 0, "##############################");
					draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
					refresh();
				}
			}
			// Rotating ship, for ship placement handle
			while (exit_console == false && turn == true && (ch2 = getch()) != 'r' && ship_placement == false) {

				// Proceding code implementation works by having a rotation value based
				// on the ship orientation, based on KEY_LEFT or KEY_RIGHT we update
				// rotation and place the ship
				switch(ch2) {
					// Key Left case
					case KEY_LEFT:
					rotation--;
					if (rotation < 1){
						rotation = 8;
					}
					if (rotation == 8) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							ship_clean(board, x, y);
							board[cur_row-1][cur_col+1] = 1;
							board[cur_row-2][cur_col+2] = 1;
						}
						else{rotation = 7;}
					}
					if (rotation == 7) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							ship_clean(board, x, y);
							board[cur_row-1][cur_col] = 1;
							board[cur_row-2][cur_col] = 1;
						}
						else{rotation = 6;}
					}
					if (rotation == 6) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							ship_clean(board, x, y);
							board[cur_row-1][cur_col-1] = 1;
							board[cur_row-2][cur_col-2] = 1;
						}
						else{rotation = 5;}
					}
					if (rotation == 5) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							ship_clean(board, x, y);
							board[cur_row][cur_col-1] = 1;
							board[cur_row][cur_col-2] = 1;
						}
						else{rotation = 4;}
					}
						if (rotation == 4) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col-1] = 1;
								board[cur_row+2][cur_col-2] = 1;
							}
							else{rotation = 3;}
						}
						if (rotation == 3) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col] = 1;
								board[cur_row+2][cur_col] = 1;
							}
							else{rotation = 2;}
						}
						if (rotation == 2) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col+1] = 1;
								board[cur_row+2][cur_col+2] = 1;
							}
							else{rotation = 1;}
						}
						if (rotation == 1) {
							valid_placement = check_move(x, y, rotation);
							if (valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row][cur_col +1] = 1;
								board[cur_row][cur_col +2] = 1;
							}
							else{rotation = 0;}
						}
						draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
						refresh();
						break;

					// Proceding code implementation works by having a rotation value based
					// on the ship orientation, based on KEY_LEFT or KEY_RIGHT we update
					// rotation and place the ship
					// Key Right case
					case KEY_RIGHT:
						rotation++;
						if (rotation > 8){
							rotation = 1;
						}
						if (rotation == 1) {
							valid_placement = check_move(x, y, rotation);
							if (valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row][cur_col +1] = 1;
								board[cur_row][cur_col +2] = 1;
							}
							else{rotation++;}
						}
						if (rotation == 2) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col+1] = 1;
								board[cur_row+2][cur_col+2] = 1;
							}
							else{rotation++;}
						}
						if (rotation == 3) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col] = 1;
								board[cur_row+2][cur_col] = 1;
							}
							else{rotation++;}
						}
						if (rotation == 4) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col-1] = 1;
								board[cur_row+2][cur_col-2] = 1;
							}
							else{rotation++;}
						}
						if (rotation == 5) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row][cur_col-1] = 1;
								board[cur_row][cur_col-2] = 1;
							}
							else{rotation++;}
						}
						if (rotation == 6) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row-1][cur_col-1] = 1;
								board[cur_row-2][cur_col-2] = 1;
							}
							else{rotation++;}
						}
						if (rotation == 7) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row-1][cur_col] = 1;
								board[cur_row-2][cur_col] = 1;
							}
							else{rotation++;}
						}
						if (rotation == 8) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row-1][cur_col+1] = 1;
								board[cur_row-2][cur_col+2] = 1;
							}
							else{rotation = 0;}
						}
						draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
						refresh();
						break;
					// Space Bar case (placement within rotation)
					case ' ':
						board[cur_row][cur_col]= 1;
						draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
						refresh();
						ship_placement = true;
						ship_placement_cor = get_ship_place_cor(board);

						// Send ship placement location to server
						boost::asio::write( socket, boost::asio::buffer(ship_placement_cor) );

						// Waiting on the ship placement notifcation from other client to
						// this creates a barrier for players to wait on eachother
						while(!continue_gate)
						{
							continue_gate = true;
							boost::asio::streambuf response_block;
							boost::asio::read_until( socket, response_block, "|" );
							string block = boost::asio::buffer_cast<const char*>(response_block.data());
							string tmp = block.substr(0, block.find("|"));
							p_v = tmp;
						}
						// Moving the curor back to new location
						move(ATACK_START_Y, ATACK_START_X);
						cur_row = 0;
						cur_col = 0;
						break;

					// Esc key case
					case 27:
						turn = false;
						ship_clean(board, x, y);
						refresh();
						board[y][x] = 0;
						draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
					}
				}
      break;

		// The proceding arrow key cases are for moving a single unit location
		// Key Right case
    case KEY_RIGHT:
      cur_col++;
      cur_col%=4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
			}
			else {
				draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
				draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
			}
			refresh();
      break;
		// Key Left case
    case KEY_LEFT:
      cur_col--;
      cur_col = (4+cur_col)%4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
			}
			else {
				draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
				draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
			}
			refresh();
      break;
		// Key Up case
    case KEY_UP:
      cur_row--;
      cur_row=(4+cur_row) % 4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
			}
			else {
				draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
				draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
			}
			refresh();
      break;
		// Key Down case
    case KEY_DOWN:
      cur_row++;
      cur_row%=4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
			}
			else {
				draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
				draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
			}
			refresh();
      break;
    }
  }
  endwin();
}
