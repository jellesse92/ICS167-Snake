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
std::string message_to_process[2];
time_t time_received[2];
char last_move[2];

time_t delay_time1 = time(NULL);
time_t delay_time2 = time(NULL);
int delay_send1 = time(NULL);
int delay_send2 = time(NULL);

int last_score1 = 0;
int last_score2 = 0;

int resend_count = 0;

bool update_clients = false;

void emptyQueue() {
	int size = message_queue[0].size();
	for (int i = 0; i < size; i++)
		message_queue[0].pop();
	size = message_queue[1].size();
	for (int i = 0; i < size; i++)
		message_queue[1].pop();
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
		message_to_process[0] = "";
		message_to_process[1] = "";
		emptyQueue();
		last_move[0] = 'D';
		last_move[1] = 'A';
		resend_count = 0;
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

void ProcessMessages() {
	time_t t1 = time(NULL);

	int index = 0;
	if (message_to_process[0].find("AVGL") != std::string::npos) {
		index = message_to_process[0].find("AVGL");
	}
	else {
		if (message_to_process[0].find("COMMAND") != std::string::npos) {
			index = message_to_process[0].find("COMMAND");
			InterpretCommand(0, message_to_process[0].substr(index));
		}
		else if (message_to_process[0].find("NewPlayer") != std::string::npos) {
			index = message_to_process[0].find("NewPlayer");
			InterpretCommand(0, message_to_process[0].substr(index));
		}

		time_t t2 = time(NULL);

		stringstream ss;
		ss << "t0:" << message_to_process[0].substr(4, message_to_process[0].length() - (message_to_process[0].length() - index + 5)) << ";t1:" << time_received[0] << ";t2:" << t2;
		server.wsSend(0, ss.str());
		delay_time1 = time(NULL) + (rand() % 2);
	}

	index = 0;

	if (message_to_process[1].find("AVGL") != std::string::npos) {
		index = message_to_process[1].find("AVGL");
		int delay = convertToInt(message_to_process[1].substr(index + 5));
	}
	else {
		if (message_to_process[1].find("COMMAND") != std::string::npos) {
			index = message_to_process[1].find("COMMAND");
			InterpretCommand(1, message_to_process[1].substr(index));
		}
		else if (message_to_process[1].find("NewPlayer") != std::string::npos) {
			index = message_to_process[1].find("NewPlayer");
			InterpretCommand(1, message_to_process[1].substr(index));
		}
		time_t t2 = time(NULL);

		stringstream ss;
		ss << "t0:" << message_to_process[1].substr(4, message_to_process[1].length() - (message_to_process[1].length() - index + 5)) << ";t1:" << time_received[1] << ";t2:" << t2;
		server.wsSend(1, ss.str());
		delay_time2 = time(NULL) + (rand() % 2);
	}
}

void messageDelay() {
	time_t t1 = time(NULL);

	if(message_to_process[0] == "")
		if (message_queue[0].size() > 0){
			message_to_process[0] = message_queue[0].front();
			message_queue[0].pop();
			time_received[0] = t1;
		}

	if (message_to_process[1] == "")
		if (message_queue[1].size() > 0) {
			message_to_process[1] = message_queue[1].front();
			message_queue[1].pop();
			time_received[1] = t1;
		}

}

/* called once per select() loop */
void periodicHandler() {
	vector<int> clientIDs = server.getClientIDs();

		if (last_score1 != snakeState.GetPlayerScore(0) || last_score2 != snakeState.GetPlayerScore(1) ||
			last_move[0] != snakeState.GetPlayerDirection(0) || last_move[1] != snakeState.GetPlayerDirection(1) ||
			resend_count >= 5) {
			
			if (last_score1 != snakeState.GetPlayerScore(0) || last_score2 != snakeState.GetPlayerScore(1)) {
				snakeState.SetPlayerInput(0, 'D');
				snakeState.SetPlayerInput(1, 'A');
			}
			
			emptyQueue();
			message_to_process[0] = "";
			message_to_process[1] = "";
			last_score1 = snakeState.GetPlayerScore(0);
			last_score2 = snakeState.GetPlayerScore(1);

			last_move[0] = snakeState.GetPlayerDirection(0);
			last_move[1] = snakeState.GetPlayerDirection(1);

			ostringstream ss;
			ostringstream score1;
			ostringstream score2;
			ostringstream ssMove;

			snakeState.UpdateBoardState();

			ss << "GB:" << snakeState.GetBoardState();
			score1 << "1:" << player1 + " score: " << snakeState.GetPlayerScore(0);
			score2 << "2:" << player2 + " score: " << snakeState.GetPlayerScore(1);
			ssMove << "MOVE:" << snakeState.GetPlayerDirection(0) << snakeState.GetPlayerDirection(1);

			for (int i = 0; i < clientIDs.size(); i++) {
				server.wsSend(clientIDs[i], ss.str());
				server.wsSend(clientIDs[i], score1.str());
				server.wsSend(clientIDs[i], score2.str());
				server.wsSend(clientIDs[i], ssMove.str());
			}

			resend_count = 0;
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
				if (message_to_process[0] != "" && message_to_process[1] != "")
				{
					ProcessMessages();
					message_to_process[0] = "";
					message_to_process[1] = "";
					resend_count++;
				}
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
