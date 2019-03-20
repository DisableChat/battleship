//******************************************************************************
// File:					bs_client.cc
// Author:				Wesley Ryder
// Date:					3/20/19
// Description:		This is the battleship server. The server is essentialy just
//                the refere of the game and recieves packets of data from the
//                clients and handles the information accordangly. Sending back
//                information to clients and evetually anoucing the winner.
//******************************************************************************

#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;

//******************************************************************************
// Function:      send_back_cor()
// Arguments:     string cor
// Description:   Pops the passed string and appends a "-" for parsing later
// Return Val:    string cor
//******************************************************************************
string send_back_cor(string cor)
{
  cor.pop_back();
  cor.append("-");
  return cor;
}

//******************************************************************************
// Function:      endgameMsg()
// Arguments:     int game_state
// Description:   Matches corrosponding game_state value with end game msg
//                which is either a win, loss, or tie.
//                *TODO* possibly remove tie - maybe not though because that
//                would be lame
// Return Val:    string msg
//******************************************************************************
string endgameMsg(int game_state)
{
  string msg;
  switch(game_state)
  {
    case 1:
      msg = "Player 1 Won!";
      break;
    case 2:
      msg = "Player 2 won!";
      break;
    case 3:
      msg = " --- Tie --- ";
      break;
  }
  return msg;
}

//******************************************************************************
// Function:      game_status()
// Arguments:     int state
// Description:   To update the status of the game being over
//                True:   Over
//                False:  Not Over
// Return Val:    Bool
//******************************************************************************
bool game_status(int state)
{
    if(state == 1 || state == 2 || state == 3)
    {
      return true;
    }
    else{return false;}
}

//******************************************************************************
// Function:      determine_winner()
// Arguments:     int p1
//                int p2
// Description:   Determine the winner of the match where return value
//                corrosponds to the proceding logic:
//                1:  Player 1 win
//                2:  Player 2 win
//                3:  Tie
// Return Val:    int
//******************************************************************************
int determine_winner(int p1, int p2)
{
  if(p1 == 3 && p2 == 3) {
    return 3;
  }
  else if(p1 == 3 && p2 < p1) {
    return 1;
  }
  else if(p2 == 3 && p1 < p2) {
    return 2;
  }
  else{return 0;}
}

//******************************************************************************
// Function:      update_player_points()
// Arguments:     int& player_points
//                int p1_response
// Description:   If the player hits enemy ship (value of p_response) then
//                increment the user point total
// Return Val:    None
//******************************************************************************
void update_player_points(int &player_points, int p_response)
{
  if(p_response == 2)
  {
    player_points++;
  }
}

//******************************************************************************
// Function:      place_shot()
// Arguments:     vector<vector<int> > &enemy_board
//                string data
// Description:   Takes given cordinates, which in this case is a string and
//                determines if the shot is a hit or not where
//                2:  hit
//                3:  miss
// Return Val:    int
//******************************************************************************
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

//******************************************************************************
// Function:      place_ship()
// Arguments:     vector<vector<int> > &board
//                string data
// Description:   Creates user1 and user2 board and then places the ship for
//                the corrosponding user ship
// Return Val:    None
//******************************************************************************
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

  // The proceding section just parses the string to obtain the corrosponding
  // cordinates of the ship
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

  // Placing ship (server side board)
  board[y1][x1] = 1;
  board[y2][x2] = 1;
  board[y3][x3] = 1;

}


//******************************************************************************
// Function:      RSP()
// Arguments:     None
// Description:   Basically the meat and potatoes of the server. Where it opens
//                sockets, allows clients to connect to the server, recieves
//                ship info placement and where the users shoot at enemy
//                locations. It Returns to the clients if they Hit or Miss
//                the enemy, while also notfiying where the users own board was
//                was shot at. After one client is succesfull in sinking a ship
//                the server returns the winner to both clients
//                -----------------------
//                | TLDR: Refs the game |
//                -----------------------
// Return Val:    None
//******************************************************************************
void RSP() {
    // Declarying variables that will be used to keep track of nessary info
    // in the running of this server
    bool game_over = false;
    int game_state = 0;
    string winner = "";

    // Point trackker
    int p1_pts = 0;
    int p2_pts = 0;

    // Boards used for each player
    vector<vector<int> > player_one_board;
    vector<vector<int> > player_two_board;

    boost::asio::io_service my_service;

    // Accept connections on
    // Advertise a service on port 4700
    tcp::acceptor acceptor(my_service, tcp::endpoint(tcp::v4(), 4700));

    //Wait for a connection on Socket #1
    tcp::socket socket1(my_service);
    acceptor.accept(socket1);
    // Connection Established!!!

    cout << "Player 1 Connected!" << endl;

    //Wait for a connection on Socket #2
    tcp::socket socket2(my_service);
    acceptor.accept(socket2);
    // Connection Established!!!!

    cout << "Player 2 Connected!" << endl;

    // Read from Socket 1 until newline
    boost::asio::streambuf buf;
    boost::asio::read_until( socket1, buf, "\n" );
    string data = boost::asio::buffer_cast<const char*>(buf.data());

    // Read from Socket 2 until newline
    boost::asio::streambuf buf2;
    boost::asio::read_until( socket2, buf2, "\n" );
    string data2 = boost::asio::buffer_cast<const char*>(buf2.data());

    // Placing ships for each user on their corrosponding board
    place_ship(player_one_board, data);
    place_ship(player_two_board, data2);

    // This is just a block for placing a target hit location for enemy board
    // after ship placement. Everything proceeding the '_|' is meaningless
    // and is there as a joke
    string gatez = "1| We about to play a mean game of batlle ship my guy";
    string gatez2 = "2| What lies behind this gate kappa ripppp";

    // Write the info to the clients
    boost::asio::write( socket1, boost::asio::buffer(gatez) );
    boost::asio::write( socket2, boost::asio::buffer(gatez2) );

    // While the game is not over run loop (firing on enemy part of game)
    while (!game_over)
    {
      // Respone shows the value of if the user detected a hit or miss etc
      int p1_response;
      int p2_response;
      string buf1_;
      string buf2_;

      // Reading player one information on where they shot cordinate wise
      boost::asio::streambuf buf;
      boost::asio::read_until( socket1, buf, "\n" );
      string data = boost::asio::buffer_cast<const char*>(buf.data());

      // Reading player two information on where they shot cordinate wise
      boost::asio::streambuf buf2;
      boost::asio::read_until( socket2, buf2, "\n" );
      string data2 = boost::asio::buffer_cast<const char*>(buf2.data());

      // Calculating player one hit detection and points
      p1_response = place_shot(player_two_board, data);
      update_player_points(p1_pts, p1_response);
      buf1_ = to_string(p1_response);
      data = send_back_cor(data);

      // Calculating player two hit detection and points
      p2_response = place_shot(player_one_board, data2);
      update_player_points(p2_pts, p2_response);
      buf2_ = to_string(p2_response);
      data2 = send_back_cor(data2);

      // Determine if game over and if so whos the winner
      game_state = determine_winner(p1_pts, p2_pts);
      game_over = game_status(game_state);

      // Server displaying points in game for each user
      cout << "P1 pts: " << p1_pts << " P2 pts: " << p2_pts << endl;

      // Atach enemy hit detection and location of where they shot to each
      // users buffer
      buf1_.append("\n").append(data2);
      buf2_.append("\n").append(data);
      buf1_.append(to_string(p2_response)).append("-");
      buf2_.append(to_string(p1_response)).append("-");

      // If game is over send final buffer info - unique in the sense that -1
      // will trigger special handle for buffer
      if(game_over == true)
      {
        // *REF* ref from client.cc, but as you can see for some reason i append
        // data2 and data (the cordinates) and then i append the response (if
        // its a hit or not to the enemy user for last minute update for ally
        // board to show info of how game was lost)
        buf1_ = "-1";
        buf1_.append("\n").append(endgameMsg(game_state)).append("*").append(data2);
        buf2_ = "-1";
        buf2_.append("\n").append(endgameMsg(game_state)).append("*").append(data);

        buf1_.append(to_string(p2_response)).append("-");
        buf2_.append(to_string(p1_response)).append("-");

        cout << "---Game Over---" << endl;
        cout << endgameMsg(game_state) << endl;
      }

      // Finally send back buffer to each client
      boost::asio::write( socket1, boost::asio::buffer(buf1_) );
      boost::asio::write( socket2, boost::asio::buffer(buf2_) );
    }
}

//******************************************************************************
// Function:      main()
// Arguments:     None
// Description:   To run main and call RSP()
// Return Val:    int
//******************************************************************************
int main()
{
  RSP();

  return 0;
}
