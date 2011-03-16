
#include <luabind/luabind.hpp>

void boardBindings(lua_State* L)
{
	luabind::module(L) [
		luabind::class<Board>("Board")
		.def(luabind::constructor<>())
		.property("Type", &Board::getType)
		.property("Revision", &Board::getRevision)
		.property("Program", &Board::getProgram)

		]
}
