
#include "UnitTest++.h"
#include "../Controller Lib/CommandFileParser.h"
#include "../Controller Lib/Board.h"
#include "../Controller Lib/Error.h"

#include <iostream>
using namespace std;

namespace
{
SUITE(CommandFileParserTests_isValidTime)
{
	TEST(realTime)
	{
		CommandFileParser p;
		p.printErrors();
		CHECK_EQUAL(true, p.isValidTime("10:15:45"));
	}

	TEST(hoursOutOfRange)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("24:15:45"));
	}

	TEST(HoursNegative)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("-1:15:45"));
	}

	TEST(minutesOutOfRange)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("10:60:45"));
	}

	TEST(MinutesNegative)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("25:-15:45"));
	}


	TEST(secondsOutOfRange)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("10:15:60"));
	}

	TEST(SecondsNegative)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("25:15:-45"));
	}

	TEST(noColons)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("01232323"));
	}

	TEST(oneColon)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("01:2345"));
	}

	TEST(tooManyColons)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("01:2:3:3"));
	}

	TEST(toolong)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidTime("001:34:56"));
	}
}
SUITE(CommandFileParserTests_isValidDate)
{
	TEST(realDate)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidDate("12/04/2011"));
	}

	TEST(tooManyDays)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("42/04/2011"));
	}

	TEST(tooManyDays_in30dayMonth)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("31/04/2011"));
	}

	TEST(days31in31daymonth)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidDate("31/03/2011"));
	}

	TEST(tooManyDaysInFeb)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("30/02/2011"));
	}

	TEST(tooManyDaysInFebNonLeapYear)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("29/02/2011"));
	}

	TEST(okDaysInFebLeapYearDivisibleBy400)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidDate("29/02/2000"));
	}

	TEST(okDaysInFebNonLeapYearDivisibleBy100)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("29/02/1900"));
	}

	TEST(okDaysInFebLeapYearDivisibleBy4)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidDate("29/02/2004"));
	}

	TEST(inexistantMonth)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("02/13/2011"));
	}

	TEST(notEnoughSlashes)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("10004/2011"));
	}

	TEST(tooManSlashes)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("/2/04/2011"));
	}

	TEST(tooShort)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("42//2011"));
	}

	TEST(tooLong)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidDate("012/0023/2001"));
	}
}

SUITE(CommandFileParserTests_isValidRelayID)
{
	//Using fakeinit
	TEST(validId)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.isValidRelayID("r3"));
	}

	TEST(validIDWith0sBeforeID)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.isValidRelayID("r0003"));
	}

	TEST(validEqualToMax)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.isValidRelayID("r10"));
	}
	TEST(validIDwithRupperCase)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.isValidRelayID("R03"));
	}

	TEST(relayIDTooHigh)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(false, p.isValidRelayID("r50"));
	}

	TEST(relayIDWithoutrBeforeID)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(false, p.isValidRelayID("3"));
	}
}

SUITE(CommandFileParserTests_isValidAction)
{
	TEST(ValidActionOn)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("On"));
	}

	TEST(ValidActionOnLowerCase)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("on"));
	}

	TEST(validActionOnAllCaps)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("ON"));
	}

	TEST(ValidActionOff)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("Off"));
	}

	TEST(ValidActionOffLowerCase)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("off"));
	}

	TEST(validActionOffAllCaps)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("OFF"));
	}

	TEST(ValidActionToggle)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("Toggle"));
	}

	TEST(ValidActionToggleLowerCase)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("toggle"));
	}

	TEST(validActionToggleAllCaps)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("TOGGLE"));
	}

	TEST(validActionToggleMixedCase)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("tOGgle"));
	}

	TEST(isValidActionToggleMixedCase2)
	{
		CommandFileParser p;
		CHECK_EQUAL(true, p.isValidAction("TOggLE"));
	}

	TEST(Gibberish)
	{
		CommandFileParser p;
		CHECK_EQUAL(false, p.isValidAction("gibberish"));
	}
}

SUITE(CommandFileParserTests_purgeWhitespace)
{
	TEST(spaceAtStartOfLine)
	{
		CommandFileParser p;
		string l = "    this is a string";
		p.purgeWhitespace(l);
		CHECK_EQUAL("this is a string", l);
	}

	TEST(spaceAtEndOfLine)
	{
		CommandFileParser p;
		string l = "this is a string    ";
		p.purgeWhitespace(l);
		CHECK_EQUAL("this is a string", l);
	}

	TEST(spaceInMiddleOfLine)
	{
		CommandFileParser p;
		string l = "this    is a string";
		p.purgeWhitespace(l);
		CHECK_EQUAL("this is a string", l);
	}
}


SUITE (CommandFileParserTests_parseSimpleLine)
{
	TEST(ValidLine1Command)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.parseSimpleLine("10:40:20 11/10/2012 r7 on"));
	}

	TEST(ValidLineMultipleCommands)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.parseSimpleLine("10:40:20 11/10/2012 r7 on r8 on r9 on"));
	}

	TEST(MultipleNonIdenticalCommands)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.parseSimpleLine("10:40:20 11/10/2012 r7 on r9 toggle r10 off"));
	}

	TEST(MultipleNonIdenticalCommandsErrorOnCommand2)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(false, p.parseSimpleLine("10:40:20 11/10/2012 r7 on r9 killer r10 off"));
	}

	TEST(ImpossibleDate)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(false, p.parseSimpleLine("10:40:20 32/10/2012 r7 on"));
	}

	TEST(ImpossibleTime)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(false, p.parseSimpleLine("29:40:20 11/10/2012 r7 on"));
	}

	TEST(ImpossibleRelayID)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(false, p.parseSimpleLine("10:40:20 11/10/2012 r290 on"));
	}

	TEST(NonExistentAction)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(false, p.parseSimpleLine("10:40:20 11/10/2012 r7 fuck_me"));
	}

	TEST(ValidExtraSpaces)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.parseSimpleLine("  10:40:20    11/10/2012 r7 on    "));
	}
}

SUITE(CommandFileParserTests_parseRepeatLine)
{
	TEST(validline)
	{
		Board b;
		CommandFileParser p(b);
		CHECK_EQUAL(true, p.parseRepeatLine("10:40:20 11/10/2012 wait 02:10: r7 "))
		//isnulldate
	}
}
}
