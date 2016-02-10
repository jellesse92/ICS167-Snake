#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"

using namespace std;

webSocket server;

int score1 = 0;
int score2 = 0;
string player1 = "";
string player2 = "";

/* called when a client connects */
void openHandler(int clientID){
	score1 = 0;
	score2 = 0;
    server.wsSend(clientID, "Welcome!");
}

/* called when a client disconnects */
void closeHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has leaved.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
	bool scored = false;
	bool named = false;

    ostringstream os;
	ostringstream os2;

	if (message.find("Player1:")==0) {
		if(message.length()>8)
			player1 = message.substr(8);
		named = true;
	}
	if (message.find("Player2:")==0) {
		if (message.length()>8)
			player2 = message.substr(8);
		named = true;
	}


	if(!named){
		if (message == "1: addScore") {
			score1++;
			scored = true;
		}
		else if (message == "2: addScore") {
			score2++;
			scored = true;
		}

		if(scored){
			
			os << ("1:" + player1 + " Score: ")<<score1;
			os2 << ("2:" + player2 + " Score: " )<< score2;
		}
		vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++) {
			server.wsSend(clientIDs[i], os.str());
			server.wsSend(clientIDs[i], os2.str());
		}
	}

}

/* called once per select() loop */
void periodicHandler(){
    static time_t next = time(NULL) + 10;
    time_t current = time(NULL);
    if (current >= next){
        ostringstream os;
        string timestring = ctime(&current);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<int> clientIDs = server.getClientIDs();
        for (int i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());

        next = time(NULL) + 10;
    }
}

int main(int argc, char *argv[]){
    int port;

    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);
    return 1;
}
