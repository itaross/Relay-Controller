
#include <luabind/luabind.hpp>
#include "../Controller Lib/Board.h"

void boardBindings(lua_State* L)
{
	luabind::module(L) [
		luabind::class_<Board>("Board")
		.def(luabind::constructor<>())
		//.def("toggleRelay" &Board::toggleRelay) //TODO : understand why i can't uncomment you
		.def("commandRelay", &Board::commandRelay)
		.property("Type", &Board::getType)
		.property("Revision", &Board::getRevision)
		.property("Program", &Board::getProgram)
		.property("Connected", &Board::isConnected)
		//.property("Time", &Board::getTime) //Not implemented yet
		.property("RelayState", &Board::getRelayState)
		.property("NumberOfRelays", &Board::getNumberOfRelays)
		];
}
