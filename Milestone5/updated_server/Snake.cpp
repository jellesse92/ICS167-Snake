#include "Snake.h"

SnakeGame::SnakeGame() {
	StartNewGame();
}

SnakeGame::~SnakeGame() {
}

void SnakeGame::StartNewGame()
{
	board = std::string(BOARD_WIDTH*BOARD_HEIGHT, '=');
	playerScore[0] = 0;
	playerScore[1] = 0;
	ResetBoardState();
}

void SnakeGame::SetPlayerInput(int player, char input)
{
	
	switch (input)
	{
	case 'A':
		if (playerMov[player] != 1)
			playerMov[player] = -1;
		break;
	case 'D':
		if (playerMov[player] != -1)
			playerMov[player] = 1;
		break;
	case 'W':
		if (playerMov[player] != BOARD_WIDTH)
			playerMov[player] = BOARD_WIDTH*-1;
		break;
	case 'S':
		if (playerMov[player] != BOARD_WIDTH*-1)
			playerMov[player] = BOARD_WIDTH * 1;
		break;
	}
}

void SnakeGame::UpdateBoardState()
{
	bool gameover = false;

	int p1Next = snakeBody[0].front() + playerMov[0];
	snakeBody[0].insert(snakeBody[0].begin(), p1Next);

	int p2Next = snakeBody[1].front() + playerMov[1];
	snakeBody[1].insert(snakeBody[1].begin(), p2Next);

	if (CheckHitBoundary(snakeBody[0].front(), playerMov[0])) {
		playerScore[1]++;
		gameover = true;
	}
	if (CheckHitBoundary(snakeBody[1].front(), playerMov[1])) {
		playerScore[0]++;
		gameover = true;
	}

	if (!gameover) {
		if (fruitPos != snakeBody[0].front())
			snakeBody[0].pop_back();
		else {
			RandomSpawnFood();
			playerScore[0]++;
		}

		if (fruitPos != snakeBody[1].front())
			snakeBody[1].pop_back();
		else {
			RandomSpawnFood();
			playerScore[1]++;
		}
	}

	ClearBoard();
	if (gameover || !UpdateBoard())
		ResetBoardState();
}

std::string SnakeGame::GetBoardState()
{
	return board;
}

int SnakeGame::GetPlayerScore(int player)
{
	return playerScore[player];
}

int SnakeGame::GetBoardHeight()
{
	return BOARD_HEIGHT;
}

int SnakeGame::GetBoardWidth()
{
	return BOARD_WIDTH;
}

char SnakeGame::GetPlayerDirection(int player)
{
	if (playerMov[player] == -1)
		return 'A';
	if (playerMov[player] == 1)
		return 'D';
	if (playerMov[player] == BOARD_WIDTH*-1)
		return 'W';
	if (playerMov[player] == BOARD_WIDTH*1)
		return 'S';
	return 'A';
}

void SnakeGame::DisplayState()
{
	for (int i = 0; i < board.length(); i++) {
		std::cout << board[i];
		if (i > 0 && (i + 1) % (BOARD_WIDTH) == 0)
			std::cout << std::endl;
	}
	std::cout << "\tScore1: " << playerScore[0] << std::endl;
	std::cout << "\tScore2: " << playerScore[1] << std::endl;
}

void SnakeGame::RandomSpawnFood() {
	int examine = rand() % BOARD_WIDTH*BOARD_HEIGHT + 1;

	while (board[examine] != '*') {
		examine = rand() % BOARD_WIDTH*BOARD_HEIGHT + 1;
	}
	fruitPos = examine;
}

void SnakeGame::ClearBoard() {
	for (int i = 0; i < board.length(); i++) {
		board[i] = '*';
	}
}

void SnakeGame::ResetBoardState() {
	ClearBoard();
	snakeBody[0].clear();
	snakeBody[1].clear();
	playerMov[0] = 1;
	playerMov[1] = -1;
	snakeBody[0].push_back(BOARD_WIDTH*BOARD_HEIGHT / 2);
	snakeBody[1].push_back(BOARD_HEIGHT*BOARD_WIDTH / 2 + BOARD_WIDTH - 1);
	RandomSpawnFood();
}

bool SnakeGame::CheckHitBoundary(int move, int direction)
{
	return ((move % (BOARD_WIDTH) == 0 && direction == 1) ||
		((move + 1) % (BOARD_WIDTH) == 0 && direction == -1) ||
		(move >= BOARD_HEIGHT*BOARD_HEIGHT) ||
		move < 0);
}

bool SnakeGame::UpdateBoard()
{
	bool gameOver = false;

	if (snakeBody[0][0] == snakeBody[1][0])
		gameOver = true;
	else if (playerMov[0] == playerMov[1]*-1 && snakeBody[0][0] == snakeBody[1][0] - playerMov[1]) {
		gameOver = true;
	}
	else {
		board[fruitPos] = 'F';
		board[snakeBody[0][0]] = '1';
		board[snakeBody[1][0]] = '2';
		for (int i = 1; i < snakeBody[0].size(); i++) {
			if (board[snakeBody[0][i]] == '1') {
				playerScore[1]++;
				gameOver = true;
			}
			if (board[snakeBody[0][i]] == '2') {
				playerScore[0]++;
				gameOver = true;
			}
			board[snakeBody[0][i]] = '1';
		}

		for (int i = 1; i < snakeBody[1].size(); i++) {
			if (board[snakeBody[1][i]] == '2') {
				playerScore[0]++;
				gameOver = true;
			}
			if (board[snakeBody[1][i]] == '1') {
				playerScore[1]++;
				gameOver = true;
			}
			board[snakeBody[1][i]] = '2';
		}

	}
	return !gameOver;
}

