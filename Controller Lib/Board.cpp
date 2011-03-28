
#include <cstdio>
#include <iostream>

#include <lua.hpp>

#include "Board.h"

//TODO : pass board serial port in ctor
//TODO : reconnect routine

using namespace std;

extern lua_State* L;

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

/** Toggle a relay's state
  * \param relay The id of the relay we want to toggle
  * \return true if all went well. If an error occurs (what error ?) the function doesn't exit but
  *			throws an exception.
  */
//TODO : testme
bool Board::toggleRelay(int relay)
{
	//do something on the serial
	_relay_state[relay - 1] = (_relay_state[relay - 1])? 0 : 1;

	//if something went wrong throw error
#ifdef SHELL_MODE
	lua_RelayState();
#endif
	return true;
}

//TODO : this message v
/** Toggle a relay's state
  * \param relay The id of the relay we want to toggle
  * \return true if all went well. If an error occurs (what error ?) the function doesn't exit but
  *			throws an exception.
  */
//TODO : testme
bool Board::commandRelay(int relay, int action)
{
	if (action == RELAY_STATE::ON)
	{
		//do something
		_relay_state[relay - 1] = 1;
	}
	else if (action == RELAY_STATE::OFF)
	{
		//do something else
		_relay_state[relay - 1] = 0;
	}
	else
		throw ("Not a valid action.");

#ifdef SHELL_MODE
	lua_RelayState();
	cout << "I commanded this bitch to do what you wanted dude !" << endl;
#endif

	return true;
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

//string Board::getRelayState()
//{
//    return "0011001000"; //TODO : this
//}

int Board::getRelayState(int relay)
{
	return (_relay_state.c_str()[relay - 1] - 48);
}

#ifdef SHELL_MODE
//NOTE : I really don't think that I need this
//Relay State should be updated after each action on the relays.
void Board::lua_RelayState()
{
	lua_newtable(L);

	for (int i = 1 ; i < _relays ; i++)
	{
	/*
     * To put values into the table, we first push the index, then the
     * value, and then call lua_rawset() with the index of the table in the
     * stack. Let's see why it's -3: In Lua, the value -1 always refers to
     * the top of the stack. When you create the table with lua_newtable(),
     * the table gets pushed into the top of the stack. When you push the
     * index and then the cell value, the stack looks like:
     *
     * <- [stack bottom] -- table, index, value [top]
     *
     * So the -1 will refer to the cell value, thus -3 is used to refer to
     * the table itself. Note that lua_rawset() pops the two last elements
     * of the stack, so that after it has been called, the table is at the
     * top of the stack.
     */

		lua_pushnumber(L, i);
		lua_pushnumber(L, _relay_state[i]);
		lua_rawset(L, -3);
	}

	lua_setglobal(L, "relayState");
}
#endif

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
    _relay_state ="1001011010";
}
