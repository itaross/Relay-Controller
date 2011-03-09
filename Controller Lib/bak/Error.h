#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <cstdio>

class Error
{
	friend std::ostream& operator<<(std::ostream &output, const Error &e);

	public:
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
	enum error_type {TIME, DATE, UNKNOWN};

}

std::ostream& operator<<(std::ostream& output, const Error& e)
{
	output << "ERROR : ";
	if (e._line != -1) //if line is -1 no line was specified
	{				  //if a line was specified we want to print the line number
		char buff[10];
		sprintf(buff, "%d", e._line);
		output << "(line : " << buff << ") ";
	}
	output << e._message;
	return output;
}

#endif // ERROR_H
