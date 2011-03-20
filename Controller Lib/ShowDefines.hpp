#ifndef SHOWDEFINES_HPP_INCLUDED
#define SHOWDEFINES_HPP_INCLUDED

#include <iostream>

void showDefines()
{
#ifdef DEBUG_NO_SERIAL
	std::cout << "defined DEBUG_NO_SERIAL" << std::endl;
#endif
#ifdef SHELL_MODE
	std::cout << "defined SHELL MODE" << std::endl;
#endif
}

#endif // SHOWDEFINES_HPP_INCLUDED
