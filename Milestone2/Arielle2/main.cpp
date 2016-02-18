#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include "Snake.h"

using namespace std;

webSocket server;

string player1 = "";
string player2 = "";

SnakeGame snakeState;

/* called when a client connects */
void openHandler(int clientID) {

	server.wsSend(clientID, "Welcome!");

	ostringstream game_width;
	ostringstream game_height;
	ostringstream game_board;

	game_width << "GW:" << snakeState.GetBoardWidth();
	game_height << "GH:" << snakeState.GetBoardHeight();
	game_board << "GB:" << snakeState.GetBoardState();

	server.wsSend(clientID, game_width.str());
	server.wsSend(clientID, game_height.str());
	server.wsSend(clientID, game_board.str());

	vector<int> clientIDs = server.getClientIDs();

	if (clientIDs.size() == 2) {
		Sleep(50);
		snakeState.StartNewGame();
	}

}

/* called when a client disconnects */
void closeHandler(int clientID) {
	ostringstream os;
	std::string player_name;
	if (clientID == 0)
		player_name = player1;
	else
		player_name = player2;
	os << player_name << " has left.";

	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); i++) {
		if (clientIDs[i] != clientID)
			server.wsSend(clientIDs[i], os.str());
	}
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message) {
	bool named = false;

	ostringstream os;
	ostringstream os2;

	if (message.find("PlayerName:") == 0) {
		if (message.length() > 11){
			if (player1 == "")
				player1 = message.substr(11);
			else
				player2 = message.substr(11);
		}
		named = true;
	}

	if (!named) {
		std::cout << message;
		std::cout << "AFTER NAMED";
		if (message.find("COMMAND:") == 0)
			std::cout << "RECIEVED:" << message << std::endl;
			if (message.length() > 7)
				snakeState.SetPlayerInput(clientID, message[8]);
	}

}

/* called once per select() loop */
void periodicHandler() {
	vector<int> clientIDs = server.getClientIDs();

		static time_t next = time(NULL)+10;
		time_t current = time(NULL);
		
		if (clientIDs.size() > 1) {
			if (current  + 8.5 >= next) {
				snakeState.UpdateBoardState();

				ostringstream ss;
				ostringstream score1;
				ostringstream score2;
				ss << "GB:" << snakeState.GetBoardState();
				score1 << "1:" << snakeState.GetPlayerScore(0);
				score2 << "2:" << snakeState.GetPlayerScore(1);

				system("CLS");
				snakeState.DisplayState();

				for (int i = 0; i < clientIDs.size(); i++){
					server.wsSend(clientIDs[i], ss.str());
					server.wsSend(clientIDs[i], score1.str());
					server.wsSend(clientIDs[i], score2.str());
				}

				next = time(NULL) + 10;
			}
		}
}

int main(int argc, char *argv[]) {
	int port;

	cout << "Please set server port: ";
	cin >> port;

	snakeState.StartNewGame();
	snakeState.UpdateBoardState();

	/* set event handler */
	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(messageHandler);
	server.setPeriodicHandler(periodicHandler);

	/* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
	server.startServer(port);
	return 1;
}
