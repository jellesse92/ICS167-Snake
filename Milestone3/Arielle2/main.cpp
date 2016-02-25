/*
	Arielle Chongco 45137728
	Jasmine Hunter  51994784
	Akihiro Izumi   69616804
	Jaysen Gan	25399148
*/


#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include "Snake.h"
#include <queue>

using namespace std;

webSocket server;
string player1 = "";
string player2 = "";

SnakeGame snakeState;
bool gameStarted;

std::queue<std::string> message_queue;
int delay_time = time(NULL);

/* called when a client connects */
void openHandler(int clientID) {
	vector<int> clientIDs = server.getClientIDs();

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
	

	if (clientIDs.size() == 2) {
		gameStarted = true;
		snakeState.StartNewGame();
		return;
	}
	else if (clientIDs.size() > 2)
		server.wsClose(clientID);
	else
		gameStarted = false;

}

/* called when a client disconnects */
void closeHandler(int clientID) {
	vector<int> clientIDs = server.getClientIDs();

	if (clientIDs.size() <= 2)
		gameStarted = false;

	ostringstream os;
	std::string player_name;
	if (clientID == 0)
		player_name = player1;
	else
		player_name = player2;
	os << player_name << " has left.";


	for (int i = 0; i < clientIDs.size(); i++) {
		if (clientIDs[i] != clientID)
			server.wsSend(clientIDs[i], os.str());
	}
}

void InterpretCommand(int clientID, std::string message) {
	bool named = false;
	vector<int> clientIDs = server.getClientIDs();

	ostringstream os;
	ostringstream os2;

	if (message.find("NewPlayer:") == 0) {
		if (message.length() > 10) {
			if (player1 == "")
				player1 = message.substr(10);
			else
				player2 = message.substr(10);
		}
		named = true;
	}

	if (!named) {
		vector<int> clientIDs = server.getClientIDs();
		if (message.length() > 7) {
			if (clientID == clientIDs[0]) {
				snakeState.SetPlayerInput(0, message[8]);
			}
			else { snakeState.SetPlayerInput(1, message[8]); }
		}
	}
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message) {
	stringstream ss;
	ss << clientID << ":" << message;
	message_queue.push(ss.str());
}

void messageDelay() {
	time_t t1 = time(NULL);

	if (message_queue.size() > 0 && t1 >= delay_time) {
		std::string message = message_queue.front();
		int clientID = message[0] - '0';
		int index = 0;

		if (message.find("COMMAND") != std::string::npos) {
			index = message.find("COMMAND");
			InterpretCommand(clientID, message.substr(index));
		}
		else if (message.find("NewPlayer") != std::string::npos){
			index = message.find("NewPlayer");
			InterpretCommand(clientID, message.substr(index));
		}

		time_t t2 = time(NULL);

		stringstream ss;
		std::cout << message;
		ss << "t0:" << message.substr(4, message.length() - (message.length() - index + 5)) << ";t1:" << t1 << ";t2:" << t2;
		server.wsSend(clientID, ss.str());


		delay_time = time(NULL) + (rand() % 5);
		message_queue.pop();
	}
		

}

/* called once per select() loop */
void periodicHandler() {
	vector<int> clientIDs = server.getClientIDs();

		static time_t next = time(NULL)+1;
		time_t current = time(NULL);
		messageDelay();
		
		if (gameStarted) {
			if (current   >= next) {
				snakeState.UpdateBoardState();

				ostringstream ss;
				ostringstream score1;
				ostringstream score2;
				ss << "GB:" << snakeState.GetBoardState();
				score1 << "1:" << player1 + " score: " << snakeState.GetPlayerScore(0);
				score2 << "2:" << player2 + " score: " << snakeState.GetPlayerScore(1);

				for (int i = 0; i < clientIDs.size(); i++){
					server.wsSend(clientIDs[i], ss.str());
					server.wsSend(clientIDs[i], score1.str());
					server.wsSend(clientIDs[i], score2.str());
				}

				next = time(NULL) + 1;
			}
		}
}

int main(int argc, char *argv[]) {
	int port;

	cout << "Please set server port: ";
	cin >> port;

	//snakeState.StartNewGame();
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
