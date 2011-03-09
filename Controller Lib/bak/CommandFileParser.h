#ifndef CommandFileParser_H
#define CommandFileParser_H

//TODO (thibaud): optimize commands for space eg: if a relay is already on and we see another instruction to turn it on delete that instruction

#include <fstream>
#include <list>
#include <string>

#include "Board.h"

/** \brief A central place for checking the command file for errors.
  */
class CommandFileParser
{
    public:
        CommandFileParser(std::ifstream file, Board board);

        bool ParseFile();
        void WriteCommandsToFile(char* file_name);

        //get errors
        //get warnings

    private:
/* ParseSimpleLine and ParseRepeatLine write all their warnings and errors into the _errors and
 * _warinigs lists. Since they return false if errors OR warnings have been found you should check
 * the lists by hand if you want to discard warnings.
 */
        bool ParseSimpleLine(char * line_text, int line_number);
        bool ParseRepeatLine();

        bool isValidTime(char* date, int line_number);
        bool isFarFromPCDate();
        //Note (thibaud) : something about discarding if user chose not o reset board time
        //Note (thibaud) : return different values if date is oka but ime is not
        bool isLegalRelayId(int relayId);

        std::ifstream _in_file;
        std::list<std::string> _errors, _warnings;
        std::string _command_file;
        Board _board;
};

namespace COMMAND_TYPE
{
    enum {SIMPLE, REPEAT};
};

#endif // CommandFileParser_H
