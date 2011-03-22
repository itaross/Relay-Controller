#ifndef CommandFileParser_H
#define CommandFileParser_H

//TODO (thibaud): optimize commands for space eg: if a relay is already on and we see another instruction to turn it on delete that instruction
//Todo : find dups in command list

#include <fstream>
#include <list>
#include <string>

#include "Board.h"
#include "Error.h"

/** \brief A central place for checking the command file for errors and building it.
  */
class CommandFileParser
{
    public:
        CommandFileParser();
        CommandFileParser(Board board);

        bool parseSimpleLine(std::string line);
        bool parseRepeatLine(std::string line);

        //TODO : remove dups
        void buildSimpleLine(std::string line);
        void buildRepeatLine(std::string line);

        void setBoard(Board board);

		//Testing functions
        bool isValidTime(std::string time);
        bool isValidDate(std::string date, bool accept_null_dates = false);
        bool isValidRelayID(std::string id);
        bool isValidAction(std::string action);
        bool isWait(std::string s);

        //Compilation Functions
        const uint8_t* compileDateTime(char* text);
        uint8_t compileRidAction(int rid, int action);

		//Usefull string manipulation stuff
        std::string purgeWhitespace(std::string &line);

		//Error mangement
        void addError(const char* message, int type);
        void addWarning(const char* message, int type);
        void clearErrors();
        void clearWarnings();
		void printErrors();

		//getters
        std::list<Error> getErrors();
        std::list<Error> getWarnings();

        std::string getCommandFile();

	private:
		int _line;
		//>0 if parsing file, -1 if parsing line

        std::ifstream _in_file;
        std::list<Error> _errors, _warnings;
        std::list <std::string> _command_file_simple_lines, _command_file_repeat_lines;
        Board _board;
};

namespace COMMAND_TYPE
{
    enum {SIMPLE, REPEAT};
};

namespace COMMAND_TYPE
{
	enum {ON, OFF, TOGGLE};
};

#endif // CommandFileParser_H
