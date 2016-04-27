#ifndef FileReader_HEADER
#define FileReader_HEADER
#include "MainGame.h"
#include <fstream>
#include <iostream>

using namespace std;

class FileReader
{
public:

	void Level2Init();
	void Level2Deinit();
	void Level2Update();
	void Level2Draw();

	void Level3Init();
	void Level3Deinit();
	void Level3Update();
	void Level3Draw();

	string line;
	int row, col;
	//int Level2_x[6000];
	//int Level3_x[6000];
	int bricknum[20][20];
	int bricknum2[20][20];
	ifstream Level2File;
	ifstream Level3File;
	float Level2_x[100][100];
	float Level3_x[100][100];
	bool A, B, C, D, J;
};
#endif