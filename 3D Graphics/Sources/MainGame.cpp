#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"
#include "FileReader.h"
#include <sstream>

using namespace Webfoot;

MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

MainGame::MainGame()
{
	fileReader = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();
 
   fileReader = frog_new FileReader();
   fileReader->PixAInit();
   fileReader->PixBInit();
   fileReader->PixCInit();
   
}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{  
	fileReader->PixADeinit();
	//fileReader->PixBDeinit();
	//fileReader->PixCDeinit();
    Inherited::Deinit();
}

//-----------------------------------------------------------------------------

const char* MainGame::GUILayerNameGet()
{
   return GUI_LAYER_NAME;
}

//-----------------------------------------------------------------------------

void MainGame::Update()
{
   Inherited::Update();

   unsigned int dt = theClock->LoopDurationGet();
   fileReader->PixAUpdate();
   // Return to the previous menu if the escape key is pressed.
   if(!theStates->StateChangeCheck() && theKeyboard->KeyJustPressed(KEY_ESCAPE))
   {
      theMainGame->StateChangeTransitionBegin(true);
      theStates->Pop();
   }
}

//-----------------------------------------------------------------------------
void MainGame::Draw()
{
	fileReader->PixADraw();
	//fileReader->PixBDraw();
	//fileReader->PixCDraw();
}

//------------------------------------------------------------------------------

void FileReader::PixAInit()
{
	//theScreen->MatrixSet()
	ifstream pixAFile("PIXA.DAT");
	if (pixAFile.is_open())
	{
		row = 0;
		while (getline(pixAFile, line))
		{
			stringstream ss(line);
			if (line != "J")
			{
				ss >> PixA_x[row];
				ss >> PixA_y[row];
				ss >> PixA_z[row];
				row++;
			}
			if (line == "J")
			{
				PixA_x[row] = 0;
				PixA_y[row] = 0;
				PixA_z[row] = 0;
				row++;
			}
		}
		pixAFile.close();
		for (int i = 0; i < 61; i++)
		{
			if (PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			{

			}
			else
			{
				PixA_x[i] = PixA_x[i] / 8 + 500;
				PixA_y[i] = PixA_y[i] / 8 + 500;
				PixA_z[i] = PixA_z[i] / 8 + 500;
			}
			
		}
	}
}

void FileReader::PixBInit()
{
	ifstream pixBFile("PIXB.DAT");
	if (pixBFile.is_open())
	{
		row = 0;
		while (getline(pixBFile, line))
		{
			stringstream ss(line);
			if (line != "J")
			{
				ss >> PixB_x[row];
				ss >> PixB_y[row];
				ss >> PixB_z[row];
				row++;
			}
			if (line == "J")
			{
				PixB_x[row] = 0;
				PixB_y[row] = 0;
				PixB_z[row] = 0;
				row++;
			}
		}
		pixBFile.close();
		for (int i = 0; i < 659; i++)
		{
			PixB_x[i] = PixB_x[i] / 5 + 500;
			PixB_y[i] = PixB_y[i] / 5 + 500;
			PixB_z[i] = PixB_z[i] / 5 + 500;
		}
	}
}

void FileReader::PixCInit()
{
	ifstream pixCFile("PIXC.DAT");
	if (pixCFile.is_open())
	{
		row = 0;
		while (getline(pixCFile, line))
		{
			stringstream ss(line);
			if (line != "J")
			{
				ss >> PixC_x[row];
				ss >> PixC_y[row];
				ss >> PixC_z[row];
				row++;
			}
			if (line == "J")
			{
				PixC_x[row] = 0;
				PixC_y[row] = 0;
				PixC_z[row] = 0;
				row++;
			}
		}
		pixCFile.close();
		/*for (int i = 0; i < 2408; i++)
		{
			PixC_x[i] = PixC_x[i] / 8 + 500;
			PixC_y[i] = PixC_y[i] / 8 + 500;
			PixC_z[i] = PixC_z[i] / 8 + 500;
		}*/
	}
}
//=================================================================================================================
void FileReader::PixADeinit()
{
	delete this;
}

void FileReader::PixBDeinit()
{
	delete this;
}

void FileReader::PixCDeinit()
{
	delete this;
}
//=================================================================================================================
void FileReader::PixAUpdate()
{
	if (theKeyboard->KeyPressed(KEY_LEFT))
	{
		for (int i = 0; i < 61; i++)
		{
			if (PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			{

			}
			else
			{
				PixA_x[i] -= 20;
			}
		}
	}
	if (theKeyboard->KeyPressed(KEY_RIGHT))
	{
		for (int i = 0; i < 61; i++)
		{
			if (PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			{

			}
			else
			{
				PixA_x[i] += 20;
			}
		}
	}
	if (theKeyboard->KeyPressed(KEY_UP))
	{
		for (int i = 0; i < 61; i++)
		{
			if (PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			{

			}
			else
			{
				PixA_y[i] -= 20;
			}
		}
	}
	if (theKeyboard->KeyPressed(KEY_DOWN))
	{
		for (int i = 0; i < 61; i++)
		{
			if (PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			{

			}
			else
			{
				PixA_y[i] += 20;
			}
		}
	}
	if (theKeyboard->KeyPressed(KEY_PLUS))
	{
		for (int i = 0; i < 62; i++)
		{
			if (PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			{

			}
			else
			{
				PixA_x[i] = ((PixA_x[i] - PixA_x[1]) * 1.05) + PixA_x[1];
				PixA_y[i] = ((PixA_y[i] - PixA_y[1]) * 1.05) + PixA_y[1];
				//PixA_z[i] *= 1.05;
			}
		}
	}
	if (theKeyboard->KeyPressed(KEY_MINUS))
	{
		for (int i = 0; i < 61; i++)
		{
			if (PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			{

			}
			else
			{
				PixA_x[i] = ((PixA_x[i] - PixA_x[1]) * (1 / 1.05)) + PixA_x[1];
				PixA_y[i] = ((PixA_y[i] - PixA_y[1]) * (1 / 1.05)) + PixA_y[1];
				//PixA_x[i] *= (1 / 1.05);
				//PixA_y[i] *= (1 / 1.05);
				//PixA_z[i] *= 0.99;
			}
		}
	}

	if (theKeyboard->KeyPressed(KEY_A))
	{
		theScreen->MatrixScale(Point2F::Create(1.05f, 1.05f));
	}

	if (theKeyboard->KeyPressed(KEY_Z))
	{
		theScreen->MatrixRotate(1.25f);
	}
}
void FileReader::PixADraw()
{
	
	for (int i = 0; i < 60; i++)
	{
		if ((PixA_x[i] == 0 && PixA_y[i] == 0 && PixA_z[i] == 0)
			|| (PixA_x[i + 1] == 0 && PixA_y[i + 1] == 0 && PixA_z[i + 1] == 0))
		{
			
		}
		else
		{
			theScreen->LineDraw(Point2F::Create(PixA_x[i], PixA_y[i]),
				Point2F::Create(PixA_x[i + 1], PixA_y[i + 1]), COLOR_RGBA8_GREEN, 1.0f, 1.0f);
			//theScreen->LineDraw(Point2F::Create((PixA_x[i] / 8) + 500, (PixA_y[i] / 8) + 500),
			//	Point2F::Create((PixA_x[i + 1] / 8) + 500, (PixA_y[i + 1] / 8) + 500), COLOR_RGBA8_GREEN, 1.0f, 1.0f);
		}
	}
}

void FileReader::PixBDraw()
{
	for (int i = 0; i < 658; i++)
	{
		if ((PixB_x[i] == 500 && PixB_y[i] == 500 && PixB_z[i] == 500)
			|| (PixB_x[i + 1] == 500 && PixB_y[i + 1] == 500 && PixB_z[i + 1] == 500))
		{

		}
		else
		{
			theScreen->LineDraw(Point2F::Create(PixB_x[i], PixB_y[i]),
				Point2F::Create(PixB_x[i + 1], PixB_y[i + 1]), COLOR_RGBA8_BLUE, 1.0f, 1.0f);
		}
	}
}

void FileReader::PixCDraw()
{
	for (int i = 0; i < 2407; i++)
	{
		if ((PixC_x[i] == 500 && PixC_y[i] == 500 && PixC_z[i] == 500)
			|| (PixC_x[i + 1] == 500 && PixC_y[i + 1] == 500 && PixC_z[i + 1] == 500))
		{

		}
		else
		{
			theScreen->LineDraw(Point2F::Create(PixC_x[i], PixC_y[i]),
				Point2F::Create(PixC_x[i + 1], PixC_y[i + 1]), COLOR_RGBA8_RED, 1.0f, 1.0f);
		}
	}
}