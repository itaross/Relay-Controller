
#include <cstdio>
#include <iostream>

#include "Board.h"

//TODO : pass board serial port in ctor
//TODO : reconnect routine

using namespace std;

/** Constructor.
  */
Board::Board()
    : _connected (false)
{

    try {
#ifdef DEBUG_NO_SERIAL
        fakeInit();
#else
        connectToBoard();
        readBoardData();
#endif //define DEBUG_NO_SERIAL
    }
    catch (...) {}; //TODO (thibaud): catch the exceptions !

}

/**************************************************************************************************
 **         End Ctor -- Start Member Functions
 *************************************************************************************************/
/** Attempts to connect to the board
  */
void Board::connectToBoard()
{
    //implement This
}

/** Writes a series a bytes on the serial port if we managed to connect
  */
void Board::writeToBoard()
{
    if (!_connected)
        //throw error
    ;

    //Implement This
}

/** Listens on the serial port for a response after a successful write
  */
void Board::listenToBoard()
{
    //Implement This
}

/** Prompts the board to send it's information over the serial connection if we manged to connect
  */
void Board::readBoardData()
{
    //Implement This
}

/***************************************************************************************************
 **         End Memeber Functions -- Start getters
 **************************************************************************************************/

/** Returns a string containing all the information we have about the board :
  * the number of relays on board, the bord time, it's type and revision, the
  * program it has on board.
  * Does NOT return the current ratings for the individual relays
  */
string Board::getFullInfo()
{
    char relays[10];
    sprintf(relays, "%d", _relays);
    return (_board_type + ", " + _board_rev + " - " + relays + " Relay Board");
}

string Board::getType()
{
    return _board_type;
}

string Board::getRevision()
{
    return _board_rev;
}

string Board::getProgram()
{
    return _board_program;
}

int Board::getNumberOfRelays()
{
    return _relays;
}
int Board::getRelayState(int relay)
{
    return 0; //TODO : this
}

string Board::getRelayState()
{
    return "0000000000"; //TODO : this
}

bool Board::isConnected()
{
    return _connected;
}

/*************************************************************************************************
 **         End Getters -- Start Testing Functions (w/o Serial Access)
 ************************************************************************************************/

void Board::fakeInit()
{
	_relays = 10;
    _connected = true;
    _board_time = time(NULL);
    _board_type = "Fake Board";
    _board_rev = "v0.0";
    _board_program = "";
    _current_ratings = "I probably should decide how this data should be represented";
}
