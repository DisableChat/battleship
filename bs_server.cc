//******************************************************************************
// File:        bs_client.cc
// Author:      Wesley Ryder
// Description:
//******************************************************************************

#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>

//using namespace std;

using namespace std;
using boost::asio::ip::tcp;


int place_shot(vector<vector<int> > &enemy_board, string data)
{
  int x = 0;
  int y = 0;

  int hit  = 2;
  int miss = 3;

  string tmp;
  int pos = 0;

  string delimiter = "-";

  tmp = data.substr(pos, data.find(delimiter));
  y = stoi(tmp);
  pos += data.find(delimiter) + 1;

  tmp = data.substr(pos, data.find(delimiter));
  x = stoi(tmp);
  pos += data.find(delimiter) + 1;

  if(enemy_board[y][x] == 1)
  {
    return 2;
  }
  else{return 3;}


}

void place_ship(vector<vector<int> > &board, string data)
{
  int x1 = 0;
  int x2 = 0;
  int x3 = 0;
  int y1 = 0;
  int y2 = 0;
  int y3 = 0;

  string tmp;
  int pos = 0;

  string delimiter = "-";

  for (int i=0;i<4;i++) {
    vector<int> t;
    for (int j=0;j<4;j++) {
      t.push_back(0);
    }
    board.push_back(t);
  }

  tmp = data.substr(pos, data.find(delimiter));
  y1 = stoi(tmp);
  pos += data.find(delimiter) + 1;

  tmp = data.substr(pos, data.find(delimiter));
  x1 = stoi(tmp);
  pos += data.find(delimiter) + 1;

  tmp = data.substr(pos, data.find(delimiter));
  y2 = stoi(tmp);
  pos += data.find(delimiter) + 1;

  tmp = data.substr(pos, data.find(delimiter));
  x2 = stoi(tmp);
  pos += data.find(delimiter) + 1;

  tmp = data.substr(pos, data.find(delimiter));
  y3 = stoi(tmp);
  pos += data.find(delimiter) + 1;

  tmp = data.substr(pos, data.find(delimiter));
  x3 = stoi(tmp);
  pos += data.find(delimiter) + 1;

  board[y1][x1] = 1;
  board[y2][x2] = 1;
  board[y3][x3] = 1;

  /*
  for (int i=0;i<4;i++) {
    for (int j=0;j<4;j++) {
      cout << board[i][j];
    }
    cout << endl;
  }
  */

}

void RSP() {

    bool game_over = false;

    vector<vector<int> > player_one_board;
    vector<vector<int> > player_two_board;
    vector<vector<int> > player_one_fire;
    vector<vector<int> > player_two_fire;


    boost::asio::io_service my_service;

    // Accept connections on
    // Advertise a service on port 4700
    tcp::acceptor acceptor(my_service, tcp::endpoint(tcp::v4(), 4700));

    tcp::socket socket1(my_service);
    // Wait for a connection from
    acceptor.accept(socket1);
    // Connection Established!!!

    // Read from Socket 1 until newline
    boost::asio::streambuf buf;
    boost::asio::read_until( socket1, buf, "\n" );
    string data = boost::asio::buffer_cast<const char*>(buf.data());

    place_ship(player_one_board, data);

    place_ship(player_two_board, data);

    while (!game_over)
    {
      int p1_response;
      int p2_response;
      boost::asio::streambuf buf;
      boost::asio::read_until( socket1, buf, "\n" );
      string data = boost::asio::buffer_cast<const char*>(buf.data());

      p1_response = place_shot(player_two_board, data);
      cout << p1_response << endl;
    }
}

int main()
{
  RSP();

  return 0;
}
