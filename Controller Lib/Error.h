#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <cstdio>
/** \brief a simple error class used for error reporting. */
class Error
{
	friend std::ostream& operator<<(std::ostream &output, const Error &e)
	{
		if (e._line != -1) //if line is -1 no line was specified
		{				  //if a line was specified we want to print the line number
			char buff[10];
			sprintf(buff, "%d", e._line);
			output << "(line : " << buff << ") ";
		}
		output << e._message;
		return output;
	}

	public:
		/** constructor
		  * \param message The error message
		  * \param type The error type (see ERROR_TYPE::TYPE)
		  * \param line THe line the error the line occured on. -1 if no line was specified
		  */
		Error(const char* message, int type, int line = -1)
			: _message(message), _type(type), _line(line) {}

		std::string getMessage()   {return _message;}
		int getErrorType() {return _type;   }

	private:
		std::string _message;
		int _type;
		int _line;
};

namespace ERROR_TYPE
{
	enum {TIME, DATE, ID, ACTION, UNKNOWN};

}

#endif // ERROR_H
