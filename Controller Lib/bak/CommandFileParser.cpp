#include <cctype>

#include "CommandFileParser.h"

/** Constructor.
  * \param file  The filestream object of the input file. The file should already be opened
  * \param board The board object for the board we are targeting
  */
CommandFileParser::CommandFileParser(ifstream file, Board board)
    : _in_file(file), _board(board)
{
    //ctor
}

/***********************************************************************************
 **            End ctor -- Start Member Functions
 ***********************************************************************************/
/** Parses the file. THis function doesn't do any of the actual parsing, it simply
  * delegates the tasks to the apropriate functions : parseSimpleLine and
  * ParseRepeatLine.
  */
bool CommandFileParser::ParseFile()
{
    int mode = COMMAND_TYPE::SIMPLE, line_number = 0;
    bool errors = false;
    while (!_in_file.eof())
    {
        line_number++;
        char* line_text[100];
        _in_file.getline(line_text, 99); //TODO : Use get not getline

/* When We parse the command file we first need to know if we are looking at the "simple commands"
 * section or the "repeat commands" section. These two sections are seperated by a line
 * consiting of 0xFF0xFF0xFF\n readline strips the trailing \n and replaces it with a \0.
 * A normal line starts with the date fields which occupies the first 6 bitsand will not in the
 * near future become 0xFF0xFF0xFF so first we check if the 4 character of the string is \0, if
 * it is we look at first 3 bytes and check if they are 0xFF0xFF0xFF. If they are we have found
 * the seperator.
 */

        if (line_text[3] == '\0' && line_text[0] == 0xFF &&
                        line_text[1] == 0xFF && line_text[2] == 0xFF)
            mode = COMMAND_TYPE::REPEAT;

        if (mode == COMMAND_TYPE::SIMPLE)
            if (ParseSimpleLine(line_text, line_number)) errors = true;
        else
            if (ParseRepeatLine(line_text, line_number)) errors = true;
    }

    return errors;
}

/** Parses a Simple Line. The name hes nothing to do with the complexity of the line itself. A simple line is just not a repeat line. A simple line contains an abritrary number of commands preceded by the time they should be run at. Each line is terminated witha \n character
  * \param line_text The actual command read from the input file
  * \param line_number The line number. Used when generating the error messages to indicate where the error occured
  */
bool CommandFileParser::ParseSimpleLine(char* line_text, int line_number)
{
    bool errors = false;
    if (!isValidTime(line_text, line_number))
    {
        errors = true;
    }

    if ( true /*isFarFromPCDate(line_text) */)
    {
     //TODO (thibaud): get the time on the controller board
     //TODO replace itoa with sprintf

        _warnings.push_back("WARNING: The time on line " + itoa(line_number) +
                            "is in the past realtive to the time on your computer.");
        errors = true;
    }

    for (unsigned int i = 6 ; line_text[i] != '\0' ; i++)
    {
        if (!isLegalRelayId())
        {
            _errors.push_back("ERROR: The relay id for command " + itoa(line_text[i] - 6) +
                              " on line " + itoa (line_number) + "is more than the available"
                              "number of relays on your board (" itoa(board.getNumberofRelays())
                             + ").");
            errors = true;
        }
    }
    return errors;
}
/** To be implmented
  */
bool CommandFileParser::ParseRepeatLine(char* line_text, int line_number)
{
	bool errors;

    if (!isValidTime(line_text))
		errors = true;

	/* isValidTime assumes that the date it must check starts at the begining of the string.
	 * To be able to parse the second date item of a repeat command we must pass a pointer to
	 * the the 7th char of the string.
	 */
	if (!isValidTime((line_text+6)))
		errors = true

	for (unsigned int i = 12 ; line_text[i] != '\0' ; i++)
	{
		if (!isLegalRelayId())
			errors = true;
	}

    return errors;
}

/***********************************************************************************************
 **            End line Parsing Functions -- Start Helper Functions
 ***********************************************************************************************/

/** Checks date values to ensure that they are possible dates. If a value in the date field which is impossible is found a descriptive error message is printed to the errors list
  * \param date The date that the function is checking
  * \param line_number The number of the line the date is on. Used when generating the error message
  */
bool CommandFileParser::isValidTime(char* date, int line_number)
{
    bool errors = false;

    if (date[0] > 24 || date[0] < 0) //hours
    {
        _errors.push_back("ERROR: The hours field (byte 1 of the time section of the command)"
                          " on line " + itoa(line_number) + "is invalid. It must be a value "
                          "between 0 and 24.");
        errors = true;
    }

    if (date[1] > 59 || date[1] < 0) //minutes
    {
        _errors.push_back("ERROR: The minutes field (byte 2 of the time section of the command)"
                          " on line " + itoa(line_number) + "is invalid. It must be a value "
                          "between 0 and 60.");
        errors = true;
    }

    if (date[2] > 59 || date[2] < 0) //seconds
    {
        _errors.push_back("ERROR: The seconds field (byte 3 of the time section of the command)"
                          " on line " + itoa(line_number) + "is invalid. It must be a value "
                          "between 0 and 60.");
        errors = true;
    }

    if (date[4] > 12 || date[4] < 1) //months
    {
        _errors.push_back("ERROR: The month field (byte 5 of the time section of the command)"
                          " on line " + itoa(line_number) + "is invalid. It must be a value "
                          "between 1 and 12.");
        errors = true;
    }

    if (date[5] < 0) //years since 2000
    {
        _errors.push_back("ERROR: The year field (byte 6 of the time section of the command)"
                          " on line " + itoa(line_number) + "is invalid. It must be a number "
                          "greater than 0");
        errors = true;
    }

    //we add an emtypy month at the start so that jamuary is 1 not 0
    unsigned short monthlen[]={0, 31,28,31,30,31,30,31,31,30,31,30,31};
    if ((!(year%4) && (year%100) || !(year%400)) && month==2) //isLeapYear?
		monthlen[2]++;

	if (date[3] >  monthlen[month])
	{
            _errors.push_back("ERROR: The month field (byte 4 of the time section of the command"
                              ") online " + itoa(line_number) + "is invalid. " +
                              itoa((int) line[4]) + '/' + itoa((int) line[5])  + "only has " +
                              itoa(monthlen[line[4]]) +'.');
            errors = true;
	}

	if (date[3] < 0)
    {
        _errors.push_back("ERROR: The days field (byte 4 of the time section of the command)"
                          " on line " + itoa(line_number) + "is invalid. It must be a number "
                          "greater than 0");
        errors = true;
    }

    return errors;
}
// Is this any use ? shouldn't this be in Board ?
bool CommandFileParser::isFarFromPCDate()
{
    //TODO (thibuad): this function
    return false;
}

/** Checks if the relayId of the command is coherent with the relay count of the connected board
  */
bool CommandFileParser::isLegalRelayId(int relayId)
{
    //TODO (thibaud): This is broken : need to shift the number 2 bits left to account for the command bits
    if (_board.getNumberOfRelays() <= relayId)
        return true;
    else
        return false;
}

