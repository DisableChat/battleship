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

const int BOARD_ONE_OFFSET = 1;
const int BOARD_TWO_OFFSET = 12;


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
     	} else {
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
	int rotation 					= 0;
	bool turn 						= true;
	bool ship_placement		= false;
	bool valid_placement	= true;
	int x 								= 0;
	int y 								= 0;


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
	mvprintw(11, 0, "- Enemy -");
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
			if (!ship_placement) {
				draw_matrix(enemy_board, 0 ,0, BOARD_TWO_OFFSET);
				board[cur_row][cur_col]= 1;
				draw_matrix(board,cur_row,cur_col, BOARD_ONE_OFFSET);
				refresh();
			}
			else
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
						break;
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
