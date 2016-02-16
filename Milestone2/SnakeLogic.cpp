#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <conio.h>
#include <windows.h>


const int BOARD_WIDTH = 20;
const int BOARD_HEIGHT = 20;

//Handles player movmement if it goes over the boarder
bool HitBoundary(int move, int direction) {
	return ((move%(BOARD_WIDTH) == 0 && direction == 1) ||
		((move+1) % (BOARD_WIDTH) == 0 && direction == -1) ||
		(move >= BOARD_HEIGHT*BOARD_HEIGHT) ||
		move < 0);
}

//Clears the board string
void ClearBoard(std::string &b) {
	for (int i = 0; i < b.length(); i++) {
		b[i] = '*';
	}
}

//Updates the board string including gameover and scores
bool UpdateBoard(std::string &b, std::vector<int> p1, std::vector<int> p2, int &fPos, int &score1, int &score2, int mov1, int mov2) {
	bool gameOver = false;
	
	if (p1[0] == p2[0])
		gameOver = true;
	else if (mov1 == mov2*-1 && p1[0] == p2[0] - mov2) {
		gameOver = true;
	}
	else {
		b[fPos] = 'F';
		b[p1[0]] = '1';
		b[p2[0]] = '2';
		for (int i = 1; i < p1.size(); i++) {
			if (b[p1[i]] == '1') {
				score2++;
				gameOver = true;
			}
			if (b[p1[i]] == '2') {
				score1++;
				gameOver = true;
			}
			b[p1[i]] = '+';
		}
	}
	return !gameOver;
}

//TEMPORARY = Displays board in C++
void DisplayState(std::string b,int score1, int score2) {
	for (int i = 0; i < b.length(); i++) {
		std::cout << b[i];
		if (i > 0 && (i+1) %(BOARD_WIDTH)  == 0)
			std::cout << std::endl;
	}
	std::cout << "\tScore1: " << score1 << std::endl;
	std::cout << "\tScore2: " << score2 << std::endl;
}

//Spawns fruit at random positions
void RandomSpawn(int &fPos, std::string b) {
	int examine = rand() % BOARD_WIDTH*BOARD_HEIGHT+1;

	while (b[examine] != '*') {
		examine = rand() % BOARD_WIDTH*BOARD_HEIGHT+1;
	}
	fPos = examine;
}

//Resets the board state but not the score
void ResetBoard(std::string &b, std::vector<int> &p1, std::vector<int> &p2, int &fPos, int &p1Mov, int &p2Mov) {
	ClearBoard(b);
	p1.clear();
	p2.clear();
	p1Mov = 1;
	p2Mov = -1;
	p1.push_back(BOARD_WIDTH*BOARD_HEIGHT / 2);
	p2.push_back(BOARD_HEIGHT*BOARD_WIDTH/2 + BOARD_WIDTH-1);
	RandomSpawn(fPos, b);
}



int main(int argc, char **argv) {

	std::string board = std::string(BOARD_WIDTH*BOARD_HEIGHT, '=');

	int p1Move = 1;
	int p2Move = -1;
	int p1Score = 0;
	int p2Score = 0;
	int fruitPos = 0;

	std::vector<int> player1;
	std::vector<int> player2;

	ResetBoard(board, player1, player2, fruitPos, p1Move, p2Move);


	bool quit = false;

	while (!quit) {
		//REMOVE FOR FINAL CODE
		system("cls");

		//REMOVE FOR FINAL CODE
		//Change to receive client comamnds
		if (_kbhit())
		{
			switch (_getch())
			{
			//Player 1 Movement
			case 'a': 
				if(p1Move != 1)
					p1Move = -1;
				break;
			case 'd':
				if(p1Move != -1)
					p1Move = 1;
				break;
			case 'w': 
				if(p1Move != BOARD_WIDTH)
					p1Move = BOARD_WIDTH*-1;
				break;
			case 's': 
				if(p1Move != BOARD_WIDTH*-1)
					p1Move = BOARD_WIDTH*1;
				break;

			//Player 2 Movement
			case 75:
				if(p2Move != 1)
					p2Move = -1;
				break;
			case 77:
				if(p2Move != -1)
					p2Move = 1;
				break;
			case 72:
				if(p2Move != BOARD_WIDTH)
					p2Move = BOARD_WIDTH*-1;
				break;
			case 80: 
				if(p2Move != BOARD_WIDTH*-1)
				p2Move = BOARD_WIDTH * 1;
				break;

			}
		}

		bool gameover = false;

		int p1Next = player1.front() + p1Move;
		player1.insert(player1.begin(),p1Next);

		int p2Next = player2.front() + p2Move;
		player2.insert(player2.begin(), p2Next);

		if (HitBoundary(player1.front(), p1Move)) {
			p2Score++;
			gameover = true;
		}
		if (HitBoundary(player2.front(), p2Move)) {
			p1Score++;
			gameover = true;
		}

		if (!gameover) {
			if (fruitPos != player1.front())
				player1.pop_back();
			else {
				RandomSpawn(fruitPos, board);
				p1Score++;
			}

			if (fruitPos != player2.front())
				player2.pop_back();
			else {
				RandomSpawn(fruitPos, board);
				p2Score++;
			}
		}

		ClearBoard(board);
		if(gameover || !UpdateBoard(board, player1,player2,fruitPos,p1Score,p2Score,p1Move,p2Move))
			ResetBoard(board, player1, player2, fruitPos, p1Move, p2Move);


		//REMOVE FOR FINAL CODE
		DisplayState(board,p1Score,p2Score);
		Sleep(100);
	}


	
	return 0;
}
