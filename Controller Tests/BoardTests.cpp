/** Tests for the Board Class **/

#include "UnitTest++.h"
#include "../Controller Lib/Board.h"

#ifndef DEBUG_NO_SERIAL
#define DEDUG_NO_SERIAL
#endif


namespace
{
SUITE (BoardTests_NoSerial)
{
	TEST(getFullInfo)
	{
		Board board;
		CHECK_EQUAL("Fake Board, v0.0 - 10 Relay Board", board.getFullInfo());
	}

	TEST(getType)
	{
		Board board;
		CHECK_EQUAL("Fake Board", board.getType());
	}

	TEST(getRevision)
	{
		Board board;
		CHECK_EQUAL("v0.0", board.getRevision());
	}

	TEST(getProgram)
	{
		Board board;
		CHECK_EQUAL("", board.getProgram());
	}

	TEST(getNumberOfRelays)
	{
		Board board;
		CHECK_EQUAL(10, board.getNumberOfRelays());
	}

	TEST(isConnected)
	{
		Board board;
		CHECK_EQUAL(true, board.isConnected());
	}

	//TODO : TEST get board time
	//TODO : TEST getRelayState
}
}
