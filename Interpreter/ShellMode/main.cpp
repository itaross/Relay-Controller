#include <iostream>

#include <luabind/luabind.hpp>
#include <lua.h>

using namespace std;
using namespace luabind;

int main()
{
	lua_State* L = lua_open();
	luabind::open(L);

	luaL_dostring(L, "function add(o1, o2) return o1 + o2 end");
	cout << "Result : " << luabind::call_function<int>(L, "add", 2, 3) << endl;
	lua_close(L);

	return 0;
}
