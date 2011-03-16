#ifndef CommandFileParser_H
#define CommandFileParser_H

//TODO (thibaud): optimize commands for space eg: if a relay is already on and we see another instruction to turn it on delete that instruction

#include <fstream>
#include <list>
#include <string>

#include "Board.h"
#include "Error.h"

/** \brief A central place for checking the command file for errors.
  */
class CommandFileParser
{
    public:
        CommandFileParser();
        CommandFileParser(Board board);

        bool parseSimpleLine(std::string line);
        bool parseRepeatLine(std::string line);

        void setBoard(Board _board);

        bool isValidTime(std::string time);
        bool isValidDate(std::string date, bool accept_null_dates = false);
        bool isValidRelayID(std::string id);
        bool isValidAction(std::string action);
        bool isWait(std::string s);

        std::string purgeWhitespace(std::string &line);

        void addError(const char* message, int type);
        void addWarning(const char* message, int type);

        void clearErrors();
        void clearWarnings();

        std::list<Error> getErrors();
        std::list<Error> getWarnings();

        void printErrors();

	private:
		int _line;
		//>0 if parsing file, -1 if parsing line

        std::ifstream _in_file;
        std::list<Error> _errors, _warnings;
        std::string _command_file;
        Board _board;
};

namespace COMMAND_TYPE
{
    enum {SIMPLE, REPEAT};
};

#endif // CommandFileParser_H
