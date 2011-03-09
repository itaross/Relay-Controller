#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <ctime>

class Board
{
    public:
        Board();

		//TODO set serial port
        std::string getFullInfo();
        std::string getType();
        std::string getRevision();
        std::string getProgram();

        int getNumberOfRelays();

//NOTE : relay state changes when board is used in tethered mode
        std::string getRelayState();
        int         getRelayState(int relay);

        int getBoardTime();

        bool isConnected();

    private:
        void connectToBoard();
        void readBoardData();
        void writeToBoard();
        void listenToBoard();

        void fakeInit();

        bool _connected;

        int _relays;
        time_t _board_time;
        std::string _board_type, _board_rev, _board_program, _current_ratings;
};

namespace RELAY_STATE {
    enum {OFF, ON};
};

#endif // BOARD_H
