#include <iostream>
#include <cstring>

#include <luabind/luabind.hpp>
#include <lua.hpp>

#include "../Controller Lib/Board.h"
#include "../Controller Lib/ShowDefines.hpp"


using namespace std;
using namespace luabind;

void boardBindings(lua_State*);

lua_State* L = NULL;

/** Show inital information message */
void sayHi()
{
	cout << "Relay Controller Interpreter Test                                 |\n"
			"                                                                  |\n"
			"Unless you are a Developper you should not be seeing this message |\n"
			"------------------------------------------------------------------+\n"
		 << endl;
}

/** initialise lua environment and bindings */
void init()
{
	L = lua_open();
	if (L == NULL) {
		cerr << "Error -- Could not initialise lua. Not enough memory available" << endl;
		exit(EXIT_FAILURE);
	}

	//atexit(lua_close(L));
	sayHi();

	luaL_openlibs(L);
	luabind::open(L);

	boardBindings(L);
}

/** Run lua scripts necessary for the correct setup of the environment
  * \return  This function does not return anything. If an error occurs whilst loading the files the
  *			 app prints an error message and exits
  */
void init_scripts()
{
	if (luaL_dofile(L, "scripts/init.lua")) {
		cerr << "Error -- Failed to load file \"scripts/init.lua\". Aborting" << endl;
		exit(EXIT_FAILURE);
	}
	if (luaL_dofile(L, "scripts/relay.lua")) {
		cerr << "ERROR -- Failed to load file \"scripts/relay.lua\". Aborting" << endl;
		exit(EXIT_FAILURE);
	}

	//initialise relays
	if (luaL_dostring(L, "init_relays()")) {
		cerr << "ERROR -- an error occured whilst running \"init_relays\" (" << lua_tostring(L, -1)
		     << endl;
		lua_pop(L, -1);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv)
{
	init();
	init_scripts();

	showDefines();

	char buff[256];
	int error = 0;

	do {
		printf("> ");
		fgets(buff, sizeof(buff), stdin);
		if (strcmp(buff, "exit") || strcmp(buff, "quit"))
			exit(EXIT_SUCCESS);
		error = luaL_loadbuffer(L, buff, strlen(buff), "line") || lua_pcall(L, 0, 0, 0);
		if (error)
		{
			fprintf(stderr, "%s\n", lua_tostring(L, -1));
			lua_pop(L, 1);  /* pop error message from the stack */
		}
	} while (true);

	lua_close(L);
	return EXIT_SUCCESS;
}

