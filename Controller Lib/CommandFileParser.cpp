
#include <cstring>
#include <ctime>
#include <sstream>
#include <algorithm>

#include "CommandFileParser.h"

using namespace std;
/** Constructor. */
CommandFileParser::CommandFileParser()
{
    //ctor
}

/** Constructor. */
CommandFileParser::CommandFileParser(Board board)
	: _board (board)
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

	if (line[line.size()] != ' ') line.erase(line.size(), 1);
	unsigned int c = line.find_first_of(' ');
	if (isValidTime(line.substr(0, c)) == false) errors = true;
	line.erase(0, c + 1);

	c = line.find_first_of(' ');
	if(!isValidDate(line.substr(0, c))) errors = true;
	line.erase(0, c + 1);

	int i = 0;
	while (1)
	{
		/* At this stage we should have at least 2 tokens seperated by a space. The first is the
		 * relay id, the second is the action.
		 * we look for the first space. if we find a space we have a relay id
		 *								if we don't find a space we parse the first part of the string
		 *									as a relay id and then exit.
		 */
		c = line.find_first_of(' ');       //look for the space
		if (c == string::npos)             //if we couldn't find a space :
		{
			if(!isValidRelayID(line)) errors = true;            //Parse the relay id
			//because we consider what is left of the string as the relay id we don't need any substr stuff
			addError("Found relay ID without command", ERROR_TYPE::UNKNOWN); //add the error
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
bool CommandFileParser::parseRepeatline(string line)
{
	return false;
}


/***********************************************************************************
 **            Date and Time checking
 **********************************************************************************/

/** Reads a time in hh:mm:ss format and parses it for errors. If it finds any errors it writes them to
  * the errors list (retrieve them with getErrors()). There is no guaratee that this function will
  * find every error on it's first pass. For simplicity if the time is malformed the function quits
  * before trying to understand the values.
  * \param time the time the function will parse
  * \return true if no errors where encountere,
  *         false if an errors was encountered.
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
  * \return true if no errors where encountere,
  *         false if an errors was encountered.
  */
bool CommandFileParser::isValidDate(string date)
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

	//NOTE : because none of the fields can be negative we don't need to check anything for year

	return !errors;
}
/***********************************************************************************
 **           Other Checks
 **********************************************************************************/

/** Checks that the id starts with r or R and that the rest of the string is a non negative number
  * lower than the number of relays on the board
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

/** Checks that the action passed as parameter is valid. Actions are on, off and toggle.
  * they can be written in any case
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

void CommandFileParser::printErrors()
{
	for (list<Error>::iterator it = _errors.begin() ; it != _errors.end() ; it++)
		cout << "ERROR : " << *it << endl;
}

void CommandFileParser::clearErrors()
{
	_errors.clear();
}

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