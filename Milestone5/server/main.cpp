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
#include <queue>
#include "Snake.h"

using namespace std;

webSocket server;
string player1 = "";
string player2 = "";

SnakeGame snakeState;
bool gameStarted;
std::queue<std::string> message_queue[2];
std::queue<std::string> send_queue[2];

time_t delay_time1 = time(NULL);
time_t delay_time2 = time(NULL);
int delay_send1 = time(NULL);
int delay_send2 = time(NULL);

int max_latency = 1;
int last_score1 = 0;
int last_score2 = 0;

void emptyQueue(std::queue<std::string> m_queue) {
	int size = m_queue.size();
	for (int i = 0; i < size; i++)
		m_queue.pop();
}

/* called when a client connects */
void openHandler(int clientID) {
	vector<int> clientIDs = server.getClientIDs();

	server.wsSend(clientID, "Welcome!");
	
	ostringstream game_width;
	ostringstream game_height;
	ostringstream game_board;
	ostringstream ssMove;
	ostringstream playerID;

	game_width << "GW:" << snakeState.GetBoardWidth();
	game_height << "GH:" << snakeState.GetBoardHeight();
	game_board << "GB:" << snakeState.GetBoardState();
	ssMove << "MOVE:" << snakeState.GetPlayerDirection(0) << snakeState.GetPlayerDirection(1);
	playerID << "ID:" << clientID;

	server.wsSend(clientID, playerID.str());
	server.wsSend(clientID, game_width.str());
	server.wsSend(clientID, game_height.str());
	server.wsSend(clientID, game_board.str());
	server.wsSend(clientID, ssMove.str());

	if (clientIDs.size() == 2) {
		gameStarted = true;
		emptyQueue(message_queue[0]);
		emptyQueue(message_queue[1]);
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

			ostringstream ss;
			ostringstream ssMove;

			ss << "GB:" << snakeState.GetBoardState();
			ssMove << "MOVE:" << snakeState.GetPlayerDirection(0) << snakeState.GetPlayerDirection(1);

			for (int i = 0; i < clientIDs.size(); i++) {
				server.wsSend(clientIDs[i], ss.str());
				server.wsSend(clientIDs[i], ssMove.str());
			}
		}
	}
}

int convertToInt(std::string str) {
	int num = 0;
	std::stringstream ss;
	ss << str;
	ss >> num;

	return num;
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message) {
	vector<int> clientIDs = server.getClientIDs();

	stringstream ss;
	ss << clientID << ":" << message;
	if(clientID == 0)
		message_queue[0].push(ss.str());
	else
		message_queue[1].push(ss.str());
}

void messageDelay() {
	time_t t1 = time(NULL);


	if (message_queue[0].size() > 0 && t1 >= delay_time1) {
		
		std::string message = message_queue[0].front();
		
		int clientID = message[0] - '0';

		int index = 0;
		if (message.find("AVGL") != std::string::npos) {
			index = message.find("AVGL");

		}
		else {
			if (message.find("COMMAND") != std::string::npos) {
				index = message.find("COMMAND");
				InterpretCommand(clientID, message.substr(index));
			}
			else if (message.find("NewPlayer") != std::string::npos) {
				index = message.find("NewPlayer");
				InterpretCommand(clientID, message.substr(index));
			}

			time_t t2 = time(NULL);

			stringstream ss;
			ss << "t0:" << message.substr(4, message.length() - (message.length() - index + 5)) << ";t1:" << t1 << ";t2:" << t2;
			server.wsSend(clientID, ss.str());

			delay_time1 = time(NULL) + (rand() % 2);
		}
		
		message_queue[0].pop();
	}

	if (message_queue[1].size() > 0 && t1 >= delay_time2) {
		std::string message = message_queue[1].front();
		int clientID = message[0] - '0';
		int index = 0;

		if (message.find("AVGL") != std::string::npos) {
			index = message.find("AVGL");
			int delay = convertToInt(message.substr(index+5));
		}
		else{
			if (message.find("COMMAND") != std::string::npos) {
				index = message.find("COMMAND");
				InterpretCommand(clientID, message.substr(index));
			}
			else if (message.find("NewPlayer") != std::string::npos) {
				index = message.find("NewPlayer");
				InterpretCommand(clientID, message.substr(index));
			}
			time_t t2 = time(NULL);

			stringstream ss;
			ss << "t0:" << message.substr(4, message.length() - (message.length() - index + 5)) << ";t1:" << t1 << ";t2:" << t2;
			server.wsSend(clientID, ss.str());
			delay_time2 = time(NULL) + (rand() % 2);
		}

		message_queue[1].pop();
	}
		

}

/* called once per select() loop */
void periodicHandler() {
	vector<int> clientIDs = server.getClientIDs();

		if (last_score1 != snakeState.GetPlayerScore(0) || last_score2 != snakeState.GetPlayerScore(1)) {
			emptyQueue(message_queue[0]);
			emptyQueue(message_queue[1]);
			std::cout << "SIZE: " << message_queue[0].size() << std::endl;
			last_score1 = snakeState.GetPlayerScore(0);
			last_score2 = snakeState.GetPlayerScore(1);

			ostringstream ss;
			ostringstream score1;
			ostringstream score2;
			ss << "GB:" << snakeState.GetBoardState();
			score1 << "1:" << player1 + " score: " << snakeState.GetPlayerScore(0);
			score2 << "2:" << player2 + " score: " << snakeState.GetPlayerScore(1);

			for (int i = 0; i < clientIDs.size(); i++) {
				server.wsSend(clientIDs[i], ss.str());
				server.wsSend(clientIDs[i], score1.str());
				server.wsSend(clientIDs[i], score2.str());
			}
		}
		messageDelay();

		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			cout << "QueryPerformanceFrequency failed!\n";
		static double freq = double(li.QuadPart) / 1000.0;
		QueryPerformanceCounter(&li);
		__int64 current = li.QuadPart;
		static __int64 interval = (double)500 * freq; // 500 ms
		static __int64 next = current + interval;
		
		if (gameStarted) {
			if (current   >= next) {
				snakeState.UpdateBoardState();
				next = current + interval;
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
