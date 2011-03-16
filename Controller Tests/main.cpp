
#define DEBUG
#define RUN_SINGLE_TEST

#ifdef RUN_SINGLE_TEST
	#define TEST_TO_RUN_SUITE "CommandFileParserTests_parseRepeatLine"
	#define TEST_TO_RUN_NAME "validLine_nullDate"
#endif

#include "UnitTest++.h"
#include "Test.h"
#include "TestReporterStdout.h"

#include <iostream>

using namespace std;

bool isTestToBeRun(const UnitTest::Test* test)
{
#ifdef RUN_SINGLE_TEST
	if (test->m_details.suiteName == TEST_TO_RUN_SUITE
		&& test->m_details.testName == TEST_TO_RUN_NAME)
		return true;
	else
		return true;
#else
	return true;
#endif
}

int main()
{
	UnitTest::TestReporterStdout reporter;
	UnitTest::TestRunner runner(reporter);
	return runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, isTestToBeRun, 0);
	//return UnitTest::RunAllTests();
}
