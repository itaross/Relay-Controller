
#include <string>

#include "../include/tclap/CmdLine.h"

//TODO (thibaud): check that at least one option is set, else print nothing to
//					be done, exiting.

using namespace std;
using namespace TCLAP;

int main(int argc, char argv**)
{
	try {
	CmdLine cmd("Relay Controller", "0.1");
	/* Declare command line parameters */
	SwitchArg 		 arg_shell("s", "shell", "start in interactive shell mode", false,
						cmd);
	SwitchArg		 arg_board_info("b", "boardinfo", "query the board for details \
						regarding it's configuration and print them", false, cmd);
	SwitchArg		 arg_relay_state("r", "relay-state", "Query the board for state of \
						all the relays and print it", false, cmd);
	SwitchArg		 arg_yes("y", "assume-yes", "assume that the answer to all prompts \
						is yes", false, cmd);
	ValueArg<string> arg_port("p", "port", "the serial port the controller board is \
						connected to", true, cmd);
	ValueArg<string> arg_upload("u", "upload-file", "check the file passed as argument \
						for compatibility with the command file format and upload it to \
						the controller board", false, "", "file name", cmd);
	ValueArg<string> arg_check("c", "check-file", "check the file passed as argument for \
						compatibility and print any errors that were found", false, "",
						"file name", cmd);
	ValueArg<int> 	arg_relays("r", "number-of-relays", "specify the number of available \
						relays if the board is not available to be interogated directly \
						or you want to override the board settings", false, "", "number \
						of relays", cmd);
	/* End Command line parameter declarations */

	cmd.Parse(argc, argv);

    if (argc == 1 /* || No args set */)
    {
        //TODO : use ExitWithError
        cout  << "You must set a least one option on the command line for this program to do"
                 " anything. For more information use \"--help\". \n"
                 "    Nothing to be done ... exiting." << endl;
        exit(1);
    }
	/* Any args that require board access cannot function if the serial port
	 * the board is connected to is not given to the program with the -p
	 * option. In this case we must abort. */

	if ((arg_boardinfo.isSet() || arg_upload.isSet() || arg_relay_state.isSet())
		&& !arg_port.isSet())
		exitWithError("you forgot to specify the serial port the controller \
			board is plugged into. Use -p <serial port> or use --help for \
			more information");


	/* If the user set -s, shell mode, but forgot to specify whiche the serial port the
	 * controller board was connected to we show a message warning him about the issue
	 * and ask him to confirm that he wants to continue */
	if (arg_shell.isSet() && !arg_port.isSet())
	{
		cout << "You have requested to enter shell mode but did not specify which serial \
				port you connected the controller board to. If you decide to continue \
				some functions will be unavailable. \nContinue Anyway ? [y/n]";
		char tmp;
		cin >> tmp;
		if (!(tmp == 'y' || tmp == 'Y'))
			exit(1);
	}

	/* Solve conflicting demands : */
	if (arg_check.isSet() && arg_upload.isSet())
	{
		cout << "conflicting options : you set 'check' and 'upload'. Use --help for more information." << endl;
	}

	/* If the user set the number of relays and specfied the serial port we need to take
	 * of conflicts between the user reported value and the board reported value */
	//TODO (thibaud): interogate the board to retrieve it's number of relays and only
	//				  warn if there is a conflict
	//TODO (thibaud): only check if this is an issue. eg not if we only want boardinfo
	//				  or relaystate
	if (arg_port.isSet() && arg_relays.isSet())
		cout << "WARNING: you set the relays option and specified the serial port, \
				the relays option will override the value retrieved from the board"
			 << endl;
	}

	if (arg_boardinfo.isSet())
	{
		relay_board.getBoardInfo();
		exit(0);
	}
	if (arg_relay-state.isSet())
	{
		relay_board.getState();
		exit(0);
	}
	if (arg_check.isSet())
	{
		//run file Compiler and output errors
	}
	if (arg_upload.isSet())
	{
		//run file Compiler
		// if there are any errors print them and abort
		// else upload to board
	}

}
