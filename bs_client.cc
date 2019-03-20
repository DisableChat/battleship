//******************************************************************************
// File:        bs_client.cc
// Author:      Wesley Ryder
// Description:
//******************************************************************************

#include <iostream>
#include <string>
#include <vector>
#include <ncurses.h>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;

const int BOARD_ONE_OFFSET	= 1;
const int BOARD_TWO_OFFSET	= 12;
const int BOARD_BOTTOM			= 24;
const int ATACK_START_Y			= 13;
const int ATACK_START_X			= 1;
bool GAMEOVER 							= false;



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

void update_enemy_board(vector<vector<int> > &board,
												vector<vector<int> > &enemy_board,
												int x,
												int y,
												string answer,
												string &anoucment)
{
	string tmp;
	string endDelimiter = "*";
	string delimiter = "\n";
	string delimiter2 = "-";
	int hit = -1;
	int pos = 0;
	int score = 0;
	int tmp_pos = 0;

	tmp = answer.substr(pos, answer.find(delimiter));
	score = stoi(tmp);

	if(score != -1)	{
		enemy_board[y][x] = score;
		int x2;
		int y2;
		int y2_ = -11;
		tmp_pos = answer.find(delimiter);
		pos = answer.find(delimiter2, tmp_pos);
		//cout << "poss value " << pos<<endl;
		tmp = answer.substr(tmp_pos, pos - tmp_pos);
		//cout << "y cordinate " << tmp << endl;
		y2 = stoi(tmp);

		tmp_pos = pos + 1;
		pos = answer.find(delimiter2, tmp_pos);
		tmp = answer.substr(tmp_pos, pos - tmp_pos);
		//cout << "poss value " << pos<<endl;
		//cout << "x cordinate " << tmp << endl;
		x2 = stoi(tmp);

		tmp_pos = pos + 1;
		pos = answer.find(delimiter2, tmp_pos);
		tmp = answer.substr(tmp_pos, pos - tmp_pos);
		//cout << "poss value " << pos<<endl;
		//cout << "hit cordinate " << tmp << endl;
		hit = stoi(tmp);
		board[y2][x2] = hit;

	}
	else
	{
		pos += answer.find(delimiter) + 1;
		enemy_board[y][x] = 2;
		anoucment = answer.substr(pos, answer.find(endDelimiter) - 3);
		GAMEOVER = true;

	}
}

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

// Draw the maxtrix
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

main(int argc, char* argv[]) {
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

	string anoucment = "";
	bool continue_gate = false;
	const char* player_number = "Player ?";
	const char* numVal;
	string p_v =							"?";


	// Blob information
	string ship_placement_cor	= "";
	int x_cord_ally		= 0;
	int y_cord_ally		= 0;
	int x_cord_enemy	= 0;
	int y_cord_enemy	= 0;

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


  cbreak();  // Pass all characters to this program!

  keypad(stdscr, TRUE); // Grab the special keys, arrow keys, etc.

  // Paint the row and column markers.
  // paint_markers(rows,cols,10,0,0);
  // Redraw the screen.
  refresh();

	// Drawing initial boards
	mvprintw(0, 0, "+ Allies +");
	mvprintw(BOARD_TWO_OFFSET - 1, 0, "- Enemy -");

	mvprintw(0, 12, "==============");
	mvprintw(1, 12, "][");
	mvprintw(1, 15, player_number);
	mvprintw(1, 24, "][");
	mvprintw(2, 12, "==============");
	refresh();
	draw_matrix(enemy_board, 0, 0, BOARD_TWO_OFFSET);
  draw_matrix(board,0, 0, BOARD_ONE_OFFSET);

  // I/O Loop
  // Stop when the q Key is hit.

  while ((ch = getch())!='q') {
    switch (ch) {
    case ' ':
			turn = true;
			x = cur_col;
			y = cur_row;

			mvprintw(BOARD_BOTTOM - 2, 0,  "                              ");
			mvprintw(BOARD_BOTTOM - 1, 0,  "                              ");
			mvprintw(BOARD_BOTTOM, 0,  "                              ");

			if (!ship_placement) {
				draw_matrix(enemy_board, 0 ,0, BOARD_TWO_OFFSET);
				board[cur_row][cur_col]= 1;
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
				refresh();
			}
			else
			{
				if(p_v == "1"){mvprintw(1, 22, "1");}
				else if(p_v == "2"){mvprintw(1, 22, "2");}
				if(!check_repeat_fire(enemy_board, x, y))
				{
					string target_loc = "";
					string tmp_x = "";
					string tmp_y = "";
					draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
					enemy_board[cur_row][cur_col]= 1;
					draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
					refresh();
					tmp_y = to_string(cur_row);
					tmp_x = to_string(cur_col);
					target_loc.append(tmp_y).append("-").append(tmp_x).append("\n");
					boost::asio::write( socket, boost::asio::buffer(target_loc) );


					// Get the response from the server!
					boost::asio::streambuf response_value;
					boost::asio::read_until( socket, response_value, "\n" );
					string answer = boost::asio::buffer_cast<const char*>(response_value.data());
					update_enemy_board(board, enemy_board, x, y, answer, anoucment);
					// CURSOR NEEDS MOVED AGAIN AFTER THIS DRAW MATTRIX!!!!
					draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
					
					draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);

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
					}

					refresh();
				}
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
			while ((ch2 = getch()) != 'r' && turn == true && ship_placement == false) {
				switch(ch2) {
					case KEY_LEFT:
					rotation--;
					//cout << rotation << endl;
					if (rotation < 1){
						rotation = 8;
					}
					//cout << "in left key" << endl;
					if (rotation == 8) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							ship_clean(board, x, y);
							board[cur_row-1][cur_col+1] = 1;
							board[cur_row-2][cur_col+2] = 1;
						}
						else{rotation = 7;}
						//else{rotation = 1;}
					}
					if (rotation == 7) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							ship_clean(board, x, y);
							board[cur_row-1][cur_col] = 1;
							board[cur_row-2][cur_col] = 1;
						}
						else{rotation = 6;}
						//else{rotation--;}
					}
					if (rotation == 6) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							ship_clean(board, x, y);
							board[cur_row-1][cur_col-1] = 1;
							board[cur_row-2][cur_col-2] = 1;
						}
						else{rotation = 5;}
						//else{rotation--;}
					}
					if (rotation == 5) {
						valid_placement = check_move(x, y, rotation);
						if(valid_placement == true){
							//cout << "here" << endl;
							ship_clean(board, x, y);
							board[cur_row][cur_col-1] = 1;
							board[cur_row][cur_col-2] = 1;
						}
						else{rotation = 4;}
						//else{rotation--;}
					}
						if (rotation == 4) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col-1] = 1;
								board[cur_row+2][cur_col-2] = 1;
							}
							else{rotation = 3;}
							//else{rotation--;}
						}
						if (rotation == 3) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col] = 1;
								board[cur_row+2][cur_col] = 1;
							}
							else{rotation = 2;}
							//else{rotation--;}
						}
						if (rotation == 2) {
							valid_placement = check_move(x, y, rotation);
							if(valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row+1][cur_col+1] = 1;
								board[cur_row+2][cur_col+2] = 1;
							}
							else{rotation = 1;}
							//else{rotation--;}
						}
						if (rotation == 1) {
							valid_placement = check_move(x, y, rotation);
							if (valid_placement == true){
								ship_clean(board, x, y);
								board[cur_row][cur_col +1] = 1;
								board[cur_row][cur_col +2] = 1;
							}
							else{rotation = 0;}
							//else{rotation--;}
						}
						draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
						refresh();
						break;

					case KEY_RIGHT:
						rotation++;
						//cout << rotation << endl;
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
					case ' ':
						//turn = false;
						board[cur_row][cur_col]= 1;
						draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
						refresh();
						ship_placement = true;
						ship_placement_cor = get_ship_place_cor(board);

						// Send ship placement location to server
						boost::asio::write( socket, boost::asio::buffer(ship_placement_cor) );

						while(!continue_gate)
						{
							continue_gate = true;
							boost::asio::streambuf response_block;
							boost::asio::read_until( socket, response_block, "|" );
							string block = boost::asio::buffer_cast<const char*>(response_block.data());
							string tmp = block.substr(0, block.find("|"));
							p_v = tmp;
						}
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
			// start sending stuff back ya

      break;
    case KEY_RIGHT:
      cur_col++;
      cur_col%=4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
				// Redraw the screen.
			}
			else {
				draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
				draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
			}
			refresh();
      break;

    case KEY_LEFT:
      cur_col--;
      cur_col = (4+cur_col)%4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
				// Redraw the screen.
			}
			else {
				draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
				draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
			}
			refresh();
      break;
    case KEY_UP:
      cur_row--;
      cur_row=(4+cur_row) % 4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
				// Redraw the screen.
			}
			else {
				draw_matrix(board, 0, 0, BOARD_ONE_OFFSET);
				draw_matrix(enemy_board, cur_row, cur_col, BOARD_TWO_OFFSET);
			}
			refresh();
      break;
    case KEY_DOWN:
      cur_row++;
      cur_row%=4;
			if (!ship_placement) {
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
				// Redraw the screen.
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
