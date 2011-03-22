
#include <cstring>
#include <ctime>
#include <sstream>
#include <algorithm>

#include "CommandFileParser.h"

using namespace std;
/** Constructor. */
CommandFileParser::CommandFileParser()
	: _line(-1)
{
    //ctor
}

/** Constructor. */
CommandFileParser::CommandFileParser(Board board)
	: _line(-1), _board (board)
{

}

/***********************************************************************************
 **           	Line Parsing
 **********************************************************************************/

/** Parse a simple line. Simple lines are not defined by complexity. A simple line is command line
  * that has the following format : hh:mm:ss DD:MM:YYYY [relayid action]
  * If an error occurs it will be written to the errors list. You can retrieve errors with getErrors()
  * \param line The line the function will attempt to parse
  * \return true if no errors were encountered, false otherwise
  */
bool CommandFileParser::parseSimpleLine(string line)
{
	 /* To parse the line we first read it until the first space. The text up until this point is
	  * the time. We get the size of this field with string.find(' '). Now we make a new string
	  * containing this field with string.sustr(0, c) where c is the size of the field. To
	  * simplify future operations we remove the field from the input string with
	  * string.erase(0, c+1). We use c + 1 instead of c to remove the space character too.
	  * Next we check if the first character is whitespace. if it is we discard it. Repeat until
	  * first character is non whitespace.
	  * Next we read the date using the same method.
	  * The string now only contains relay ids and their associated commands
	  * There are 2 possible situations :
	  * 	we have just one element --> error : we should have at leat 2 elements
	  * 	we have 2 elements -> great let's check that the first is a relay id and that the
	  *								second is a command
	  *		we have more tham 2 elements -> read the first check that it is a relay id, read the
	  *								second, check that it's a command. we are in the same situation
	  *								as before so we just repeat the algorithm until we have parsed
	  *								all the blocks in the line.
	  */
	bool errors = false;

	purgeWhitespace(line); //strip duplicate spaces
	if (line.empty())
	{
		addError("empty line", ERROR_TYPE::UNKNOWN);
		return false;
	}
	//Not any more you don't -- HOUSTON, We have a problem ... what if the user is stupid enough to pass a way to short string that makes find() return npos very early on ? you didn't think about that did you ?
	//if (line[line.size()] != ' ') line.erase(line.size(), 1);
	unsigned int c = line.find_first_of(' ');
	if (isValidTime(line.substr(0, c)) == false) errors = true;
	line.erase(0, c + 1);

	if (line.empty())
	{
		addError("Incomplete line. Missing date and commands", ERROR_TYPE::UNKNOWN);
		return false;
	}

	c = line.find_first_of(' ');
	if (c == string::npos) //the date is the last field on the line
	{
		addError("Incomplete line. No commnds", ERROR_TYPE::UNKNOWN);
		isValidDate(line);
		errors = true;
	}
	if(!isValidDate(line.substr(0, c))) errors = true;
	line.erase(0, c + 1);

	int i = 0;
	while (1)
	{
		if (line.empty())
		{
			addError("No relay IDs or commands to parse.", ERROR_TYPE::UNKNOWN);
			return false;
		}
		/* At this stage we should have at least 2 tokens seperated by a space. The first is the
		 * relay id, the second is the action.
		 * we look for the first space. if we find a space we have a relay id
		 *								if we don't find a space we parse the first part of the string
		 *									as a relay id and then exit.
		 */
		c = line.find_first_of(' ');       //look for the space
		if (c == string::npos)             //if we couldn't find a space :
		{
			isValidRelayID(line);    //Parse the relay id
			//since we're returning an error at the end of the block we don't need to check the return value
			//because we consider what is left of the string as the relay id we don't need any substr stuff
			addError("Found relay ID without command", ERROR_TYPE::UNKNOWN); //add the error
			errors = true;
			break; //we are at the end of the line, nothing more to be done
		}

		/* i serves as a counter, if we never increment i that means that we exited the loop just above
		 * after checking c == npos, If i == 0 at the end of the loop we kmow that there where no
		 * commands in the line
		 */
		i++;

		//c != npos --> We found a space. We have at least 2 tokens in our line left
		string relayid = line.substr(0, c);
		line.erase(0, c + 1); //erase from line what we just put into relayid and erase the space
		if (!isValidRelayID(relayid)) errors = true;

		//we parsed the relay ID. If we are here we have at least one token left in the string
		c = line.find_first_of(' ');
		if (c == string::npos) //we're at the end of the line, we parse the rest of the line as the token and exit
		{
			if(!isValidAction(line)) errors = true;
			break;
		}

		//we have more than one token left, read until next space and parse that as the token
		if (!isValidAction(line.substr(0, c))) errors = true;
		line.erase(0, c + 1);
	}

	if (i == 0)
		addWarning("line contained no commands", ERROR_TYPE::UNKNOWN);

	return !errors;
}

/** Parse a repeat line. Repeat lines have the following format :
  * 	hh:mm:ss DD/MM/YYYY wait hh:mm:ss DD/MM/YYYY r7 on r10 toggle
  * \param line The line the function will attempt to parse
  * \return true if no errors were reported, false otherwise
  */
//TODO : allow the user to not specify an empty date
bool CommandFileParser::parseRepeatLine(string line)
{
	bool errors = false;

	purgeWhitespace(line);
	if (line.empty())
	{
		addError("empty line", ERROR_TYPE::UNKNOWN);
		return false;
	}

	//date and time parsing
	unsigned int c = line.find_first_of(' ');					 //get the first field (time)
	if (isValidTime(line.substr(0, c)) == false) errors = true;//check it
	line.erase(0, c + 1);										 //erase it from the string

	if (line.empty())
	{
		addError("Incomplete line. Missing date, delay and commands", ERROR_TYPE::UNKNOWN);
		return false;
	}

	c = line.find_first_of(' ');								 //get the second field (date)
	if(!isValidDate(line.substr(0, c))) errors = true;	         //check it
	line.erase(0, c + 1);										 //erase it from the string

	if (line.empty())
	{
		addError("Incomplete line. Missing delay and commands", ERROR_TYPE::UNKNOWN);
		return false;
	}

	c = line.find_first_of(' ');								 //get the third field (wait)
	if(!isWait(line.substr(0, c))) errors = true;			     //check it
	line.erase(0, c + 1);										 // erase it from the string

	if (line.empty())
	{
		addError("Incomplete line. Missing delay and commands", ERROR_TYPE::UNKNOWN);
		return false;
	}

	c = line.find_first_of(' ');								 //get the fourth field (delay - time)
	if(!isValidTime(line.substr(0, c))) errors = true;			 //check it
	line.erase(0, c + 1);										 // erase it from the string

	if (line.empty())
	{
		addError("Incomplete line. Line contains no commands and delay is malformed. if your delay is less than a day you can write 00/00/0000 for the date delay.", ERROR_TYPE::UNKNOWN);
		return false;
	}

	c = line.find_first_of(' ');								 //get the fith field (delay - date)
	if(!isValidDate(line.substr(0, c), true)) errors = true;			 //check it
	line.erase(0, c + 1);										 // erase it from the string

	if (line.empty())
	{
		addError("Incomplete line. Line contains no commands.", ERROR_TYPE::UNKNOWN);
		return false;
	}

	//we have now parsed the fixed part of the command
	// now we only have to take care of the variable length section
	int i = 0;
	while (1)
	{
		if (line.empty())
		{
			addError("No relay IDs or commands to parse.", ERROR_TYPE::UNKNOWN);
			return false;
		}
		/* At this stage we should have at least 2 tokens seperated by a space. The first is the
		 * relay id, the second is the action.
		 * we look for the first space. if we find a space we have a relay id
		 *								if we don't find a space we parse the first part of the string
		 *									as a relay id and then exit.
		 */
		c = line.find_first_of(' ');       //look for the space
		if (c == string::npos)             //if we couldn't find a space :
		{
			isValidRelayID(line);    //Parse the relay id
			//since we're returning an error at the end of the block we don't need to check the return value
			//because we consider what is left of the string as the relay id we don't need any substr stuff
			addError("Found relay ID without command", ERROR_TYPE::UNKNOWN); //add the error
			errors = true;
			break; //we are at the end of the line, nothing more to be done
		}

		/* i serves as a counter, if we never increment i that means that we exited the loop just above
		 * after checking c == npos, If i == 0 at the end of the loop we kmow that there where no
		 * commands in the line
		 */
		i++;

		//c != npos --> We found a space. We have at least 2 tokens in our line left
		string relayid = line.substr(0, c);
		line.erase(0, c + 1); //erase from line what we just put into relayid and erase the space
		if (!isValidRelayID(relayid)) errors = true;

		//we parsed the relay ID. If we are here we have at least one token left in the string
		c = line.find_first_of(' ');
		if (c == string::npos) //we're at the end of the line, we parse the rest of the line as the token and exit
		{
			if(!isValidAction(line)) errors = true;
			break;
		}

		//we have more than one token left, read until next space and parse that as the token
		if (!isValidAction(line.substr(0, c))) errors = true;
		line.erase(0, c + 1);
	}

	if (i == 0)
		addWarning("line contained no commands", ERROR_TYPE::UNKNOWN);

	return !errors;
}

/***********************************************************************************
 **            Line Compiling
 **********************************************************************************/

/** Build a previously parsed line and inserts it into the commands list at the apropriate
  * location
  * \param
  * \return
  */
bool CommandFileParser::buildSimpleLine(string line)
{
	uint8_t* command[256];
	/* If the line is correctly formed (which it should if the frontend developper is not brain dead
	* and read the comment just above this one) we know that the entire date field is the text before
	* the second space character. A generic line has the following format :
	*          10:40:20 11/10/2012 r7 on
	*                             ^
	*                          char - 20
	*/
	//FIXME : This is a memory leak ...
	uint8_t date* = compileDateTime(line.substr(0, 19).c_str());
	line.erase(0, 20);

	/* The rest of the line is now a sequence of relay ids and actions */
	while (!line.empty())
	{
		int found = line.find(' ');
		int rid = line.substr(0, found);
		line.erase(0, found);

		found = line.find(' ');
		int action = line.substr(0, found);
		line.erase(0, found);

		uint8_t rid = compileRidAction(rid, action);
	}

	/* Now we only need to find where to insert the command in the list */
	if (_command_file_simple_lines.empty())
		_command_file_simple_lines.push_back(command);


}

/** Build a previously parsed line and inserts it into the commands list at the apropriate
  * location
  * \param
  * \return
  */
bool buildRepeatLine(string line)
{

}

/***********************************************************************************
 **            Date and Time checking
 **********************************************************************************/

/** Reads a time in hh:mm:ss format and parses it for errors. If it finds any errors it writes them to
  * the errors list (retrieve them with getErrors()). There is no guaratee that this function will
  * find every error on it's first pass. For simplicity if the time is malformed the function quits
  * before trying to understand the values.
  * \param time the time the function will parse
  * \return true if no errors where encountered, false otherwise
  */
bool CommandFileParser::isValidTime(string time)
{
	bool errors = false;
	int hours, minutes, seconds;

	/* We start by checking that the time appears to be ok : 3 series of 2 numbers seperated by a
	 * colon and is 8 characters long                                                            */
	if (time.size() != 8)
	{
		addError("Not a valid time. A time must be 8 characters long", ERROR_TYPE::TIME);
		errors = true;
	}

	if (!(time[2] == ':' && time[5] == ':'
		  && isdigit(time[0]) && isdigit(time[1]) && isdigit(time[3])
		  && isdigit(time[4]) && isdigit(time[6]) && isdigit(time[7])))
	{
		addError("Not a valid Time. A valid time is a sequence of 3 numbers seperated by colons (':')", ERROR_TYPE::TIME);
		errors = true;
	}

	/* We exit here if we had some errors because for simplicity we assume that things are at the right
	 * place. eg all fields are 2 digits long and we only have 2 ':'. If this wasn't the case the
	 * next steps of the function would be more complicated
	 */
	if (errors)
		return false;

	/* Now we build the actual fields of the time and check them for sanity */
	time[2] = ' ';
	time[5] = ' ';

	stringstream ss;
	ss << time;
	ss >> hours >> minutes >> seconds;

	if (hours > 23 || hours < 0)
	{
		addError("Not a valid time. hours can only be between 0 and 23 (included).", ERROR_TYPE::TIME);
		errors = true;
	}
	if (minutes > 59 || minutes < 0)
	{
		addError("Not a valid time. minutes can only be between 0 and 59 (included).", ERROR_TYPE::TIME);
		errors = true;
	}
	if (seconds > 59 || seconds <0)
	{
		addError("Not a valid time. seconds can only be between 0 and 59 (included).", ERROR_TYPE::TIME);
		errors = true;
	}

	return !errors;
}

/** Reads a date in dd/mm/yyyy format and parses it for errors. If it finds any errors it writes them to
  * the errors list (retrieve them with getErrors()). There is no guaratee that this function will
  * find every error on it's first pass. For simplicity if the date is malformed the function quits
  * before trying to understand the values.
  * \param date the date the function will parse
  * \return true if no errors where encountered, false otherwise
  */
//TODO :  remove necessity that dates be 10 chars long

bool CommandFileParser::isValidDate(string date, bool accept_null_dates)
{
	bool errors = false;
	int day, month, year;

	/* We use the same approach to check the date that we used to check the time.
	 * First we check that the date appears to be correct : it is 10 characters long and is composed
	 * of 2 numbers, a slash, 2 more numbers, a slach and 4 numbers
	 */

	 if (date.size() != 10)
	 {
	 	addError("Not a valid date. A date must be 10 characters long", ERROR_TYPE::DATE);
	 	errors = true;
	 }

	if (!(date[2] == '/' && date[5] == '/'
		  &&isdigit(date[0]) && isdigit(date[1]) && isdigit(date[3]) && isdigit(date[4])
		  && isdigit(date[6]) &&isdigit(date[7]) && isdigit(date[8]) && isdigit(date[9])))
	{
		addError("Not a valid date. A date must be written as : dd/mm/yyyy", ERROR_TYPE::DATE);
		errors = true;
	}

	/* as before we exit here if there were any errors to simplify processing of the date afterwards */
	if (errors)
		return false;

	date[2] = ' ';
	date[5] = ' ';

	stringstream ss;
	ss << date;
	ss >> day >> month >> year;

	//Check month length
	//NOTE : as '-' is not a number nothing can be negative
	//we add an emtypy month at the start so that jamuary is 1 not 0
    unsigned short monthlen[]={0, 31,28,31,30,31,30,31,31,30,31,30,31};
    string months[] = {"", "January", "Febuary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    if (month == 2 && ((year % 400  == 0) || (!(year % 100 == 0) && (year % 4 == 0)))) //isLeapYear?
		monthlen[2]++;

	if (day > monthlen[month])
	{
		string error_message = "Not a valid date. " + months[month] + " only contains ";
		if (monthlen[month] == 30) error_message += "30 days";
		else if (monthlen[month] == 31) error_message += "31 days";
		else if (monthlen[month] == 29) error_message += "29 days";
		else error_message += "28 days";

		addError(error_message.c_str(), ERROR_TYPE::DATE);
		errors = true;
	}

	if (month > 12)
	{
		addError("Not a valid date. A year only contains 12 months", ERROR_TYPE::DATE);
		errors = true;
	}

	if (!accept_null_dates && (day == 0 || month == 0 || year == 0))
	{
		addError("This date cannot contain null fields", ERROR_TYPE::DATE);
		errors = true;
	}

	//NOTE : because none of the fields can be negative we don't need to check anything for year

	return !errors;
}
/***********************************************************************************
 **           Other Checks
 **********************************************************************************/

/** Checks that the id starts with r or R and that the rest of the string is a non negative number
  * lower than the number of relays on the board
  * \param id The relay id to check
  * \return True if the relay id is valdi, false otherwise
  */
bool CommandFileParser::isValidRelayID(string id)
{
	if (id[0] != 'r' && id[0] != 'R')
	{
		addError("Not a relay id. A relay id must start with 'r'", ERROR_TYPE::ID);
		return false;
	}
	/* By quitting if the id does not start with an 'r' we can assume for the rest of the function
	 * the id does in fact start with an 'r' and that we can discard the first character
	 */

	 for (unsigned int i = 1 ; i < id.size() ; i++)
		if (!isdigit(id[i]))
		{
			addError("Not a relay id. A relay id is a positive integer preceded by r", ERROR_TYPE::ID);
			return false;
		}
	/* If the id contains a non number then there's no sense in going forward. we won't be able to
	 * parse the number
	 * Now we parse the number:
	 */

	/* To discard the leading 0s we build a new string containing only non 0 elements. We
	* initialize i to 1 to not have to deal with the leading r
	* //NOTE : doesn't the stringstream already strip leading 0s ?
	*/
	string s_id;
	for (unsigned int i = 1 ; i < id.size() ; i++)
	{
		if (id[i] != 0) s_id += id[i];
	}

	stringstream ss;
	ss << s_id;
	int r_id;
	ss >> r_id;

	if (r_id > _board.getNumberOfRelays())
	{
		string r;
		ss << _board.getNumberOfRelays();
		ss >> r;
		string error_message = "Not a valid relay id. This board (" + _board.getFullInfo() + ") "
				"only has " + r + "relays.";
		addError(error_message.c_str(), ERROR_TYPE::ID);
		return false;
	}

	return true;
}

/** Checks that the action passed as parameter is valid. Actions are "on", "off" and "toggle".
  * The check is case insensitive
  * \param action The string that will be parsed
  * \return true if the action is valid, false otherwise
  */
bool CommandFileParser::isValidAction(string action)
{
	string l_action;
	l_action.resize(action.size());

	transform(action.begin(), action.end(), l_action.begin(), ::tolower);
	if ( l_action == "on" || l_action == "off" || l_action == "toggle")
		return true;
	else
	{
		string error_message = "Not a valid action." + action + " is not a valid action.";
		addError(error_message.c_str(), ERROR_TYPE::ACTION);
		return false;
	}
}

/** Check that the string is the text "wait". The check is case insensitive
  * \param s The string that will be tested
  * \return True if the string is "wait", false otherwise
  */
bool CommandFileParser::isWait(string s)
{
	string s_transformed;
	s_transformed.resize(s.size());

	transform(s.begin(), s.end(), s_transformed.begin(), ::tolower);
	if (s_transformed != "wait")
	{
		string error_message = "expected \2wai\2 as block 3, found : " + s;
		addError(error_message.c_str(), ERROR_TYPE::UNKNOWN);
		return false;
	}
	return true;
}

/** Remove whitespace at the start of the string, at the end of the string and any duplicate spaces
  * in the string
  * \param &line a reference to the line that we are parsing
  * \return the produced string. This is redundant as we are operating on a reference
  */
string CommandFileParser::purgeWhitespace(string &line)
{
	//strip whitespace from start and end of line
	while (isspace(line[0])) line.erase(0, 1);
	while (isspace(line[line.size() - 1])) line.erase(line.size() - 1, 1);

	for (unsigned int i = 0 ; i < line.size() ; i++)
	{
		if (isspace(line[i]))
		{
			while (isspace(line[i+1])) line.erase(i + 1, 1);
		}
	}

	return line;
}

/***********************************************************************************
 **            Compilation Functions
 **********************************************************************************/

/** Compiles the Date and TIme into 6 bytes.
  * This function does no error checking ! All input should be checked by the caller
  * (se CommandFileParser::isValidDate and CommandFileParser::isValidTime). To achieve this
  * each item in the time and date is shoved into a single byte. The only exception is the date
  * 2000 > 512 so we measure year in years since 2000
  * \param text A string containing only the date and time
  * \return An array containing the 6 bytes of the compiled field
  */
const uint8_t* CommandFileParser::compileDateTime(char* text)
{
	int hour, minute, second, year, month, day;
}

/** Compiles the relay id and action into a single byte.
  * This function does no error checking whatsoever ! all input should be checked by the caller.
  * To be able to compile the 2 pieces of information into a byte we suppose that the relay id
  * is less than 65 (max 64).
  * \param rid The relay id
  * \param action
  * \return A single byte containing the relay id and the action
  */
uint8_t CommandFileParser::compileRidAction(int rid, int action)
{
	/* We cast the id into an int and shift it 2 bits left.
	 * At this stage the rightmost 2 bits are 0
	 * We just need to OR them with the action in binary form to produce the finished byte
	 */
	uint8_t result = (((unit8_t) rid) << 2) | ((action ==  ACTION::ON) 0b00000011 : 0b00000010);
	return result;
}

/***********************************************************************************
 **            Error and Warning management
 **********************************************************************************/

/** Add an Error to the error list.
  * \param message The error message
  * \param type    The error type (see ERROR_TYPE::TYPE)
  */
void CommandFileParser::addError(const char* message, int type)
{
	if (_line == -1)
		_errors.push_back(Error(message, type));
	else
		_errors.push_back(Error(message, type, _line));
}

/** Add a warning to the warnings list.
  * \param message The error message
  * \param type    The error type (see ERROR_TYPE::TYPE)
  */
void CommandFileParser::addWarning(const char* message, int type)
{
	if (_line == -1)
		_warnings.push_back(Error(message, type));
	else
		_warnings.push_back(Error(message, type, _line));
}

/** Print errors to stdout. DOes not clear errors.
  */
void CommandFileParser::printErrors()
{
	for (list<Error>::iterator it = _errors.begin() ; it != _errors.end() ; it++)
		cerr << "ERROR : " << *it << endl;
}

/** Clears all errors.
  */
void CommandFileParser::clearErrors()
{
	_errors.clear();
}

/** Clears all warnings.
  */
void CommandFileParser::clearWarnings()
{
	_warnings.clear();
}
/***********************************************************************************
 **           Getters
 **********************************************************************************/

list<Error> CommandFileParser::getErrors()
{
	return _errors;
}

list<Error> CommandFileParser::getWarnings()
{
	return _warnings;
}

/***********************************************************************************
 **           Setters
 **********************************************************************************/

void CommandFileParser::setBoard(Board board)
{
	_board = board;
}
