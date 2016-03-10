/*
TEAM MEMBERS
Arielle Chongco 45137728
Jasmine Hunter  51994784
Akihiro Izumi   69616804
Jaysen Gan	25399148
*/

#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <conio.h>
#include <windows.h>

const int BOARD_WIDTH = 50;
const int BOARD_HEIGHT = 50;

class SnakeGame {

public:
	SnakeGame();
	~SnakeGame();

	void StartNewGame();							//Resets game entirely
	void SetPlayerInput(int player, char input);	//Gets player input
	void UpdateBoardState();						//Updates board state. CALL AFTER GETTING PLAYER INPUTS
	std::string GetBoardState();					//Gets the state of the board
	int GetPlayerScore(int player);					//Gets player score
	int GetBoardHeight();							//Gets board height
	int GetBoardWidth();							//Gets board width
	char GetPlayerDirection(int player);			//Gets the direction the player is moving
	
	void DisplayState();			//TEMPORARY outputs board state

private:

	void RandomSpawnFood();								//Randomly spawns fruit
	void ClearBoard();									//Clears the game board
	void ResetBoardState();								//Resets board state
	bool CheckHitBoundary(int move, int direction);		//Checks if player hit boundary
	bool UpdateBoard();									//Checks for gameover and moves actors

	std::string board;				//String that keeps track of board state
	int playerMov[2];				//Player movement direction
	int playerScore[2];				//Keeps track of player score
	int fruitPos;					//Fruit position

	std::vector<int> snakeBody[2];	//Snake bodies
};

#endif