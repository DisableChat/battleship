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


void draw_top_matrix(vector<vector<int> > &board,
		     int cur_row,
		     int cur_col) {

  for (int j=0;j<4;j++) {
      move(0,2*j);
      printw("+-");
    }
    move(0,2*4);
    printw("+");
  for (int i=0;i<4;i++) {
    for (int j=0;j<4;j++) {
      move(2*i+1,2*j);
      printw("|");
      move(2*i+1,2*j+1);
      if (board[i][j] == 0) {
	printw(" ");
      } else {
	printw("X");
      }
    }
    move(2*i+1,2*4);
    printw("|");
    for (int j=0;j<4;j++) {
      move(2*i+2,2*j);
      printw("+-");
    }
    move(2*i+2,2*4);
    printw("+");
  }
  move(2*cur_row+1,2*cur_col+1);
}




main() {
  int rows;
  int cols;
  int cur_row=0;
  int cur_col=0;
  int ch;

  vector<vector<int> > board;
  for (int i=0;i<4;i++) {
    vector<int> t;
    for (int j=0;j<4;j++) {
      t.push_back(1);
    }
    board.push_back(t);
  }

  // Screen initialization

  initscr();
  // Clear the screen

  clear();

  // Get the size of the window!
  getmaxyx(stdscr,rows,cols);


  cbreak();  // Pass all characters to this program!

  keypad(stdscr, TRUE); // Grab the special keys, arrow keys, etc.

  // Paint the row and column markers.
  //paint_markers(rows,cols,10,0,0);
  // Redraw the screen.
  refresh();


  draw_top_matrix(board,0,0);


  // I/O Loop....
  // Stop when the q Key is hit.
  //

  while ((ch = getch())!='q') {
    switch (ch) {
    case ' ':  board[cur_row][cur_col]=0;
      draw_top_matrix(board,cur_row,cur_col);
      // Redraw the screen.
      refresh();

      break;
    case KEY_RIGHT:
      cur_col++;
      cur_col%=4;
      draw_top_matrix(board,cur_row,cur_col);
      // Redraw the screen.
      refresh();
      break;
    case KEY_LEFT:
      cur_col--;
      cur_col = (4+cur_col)%4;
      draw_top_matrix(board,cur_row,cur_col);
      // Redraw the screen.
      refresh();
      break;
    case KEY_UP:
      cur_row--;
      cur_row=(4+cur_row) % 4;
      draw_top_matrix(board,cur_row,cur_col);

      //      paint_markers(rows,cols,10,cur_row,cur_col);
      // Redraw the screen.
      refresh();
      break;
    case KEY_DOWN:
      cur_row++;
      cur_row%=4;
      draw_top_matrix(board,cur_row,cur_col);
            //paint_markers(rows,cols,10,cur_row,cur_col);
      // Redraw the screen.
      refresh();
      break;


    }

  }

  endwin();
}
