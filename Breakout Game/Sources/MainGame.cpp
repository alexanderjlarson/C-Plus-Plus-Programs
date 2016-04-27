#include "Frog.h"
#include "Windows.h"
#include "MainGame.h"
#include "MainUpdate.h"
#include "FileReader.h"
#include <sstream>
#include <cstdlib>
#include <iostream>
#define LASER "Laser"

using namespace Webfoot;

/// Speed of the ball along a given axis in pixels per second.
#define BALL_AXIS_SPEED 350.0f

MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

//-----------------------------------------------------------------------------

MainGame::MainGame()
{
   Userpaddle = NULL;
   Power = NULL;
   brick = NULL;
   ball = NULL;
   fileReader = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();

   // Create and initialize the ball.
   
   ball = frog_new Ball();
   ball->Init();

   Power = frog_new PowerUp();
   Power->Init();

   Userpaddle = frog_new UserPaddle();
   Userpaddle->Init();

   /*fileReader = frog_new FileReader();
   fileReader->Level2Init();
   fileReader->Level3Init();*/

   brick = frog_new Brick();
   brick->Init();

   gamePaused = true;
   allowCollision = true;
   newGame = true;

}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{
   // Deinitialize and delete the ball.
   if(ball)
   {
      ball->Deinit();
      frog_delete ball;
      ball = NULL;
   }
   if (Userpaddle)
   {
	   Userpaddle->Deinit();
	   frog_delete Userpaddle;
	   Userpaddle = NULL;
   }
   if (brick)
   {
	   brick->Deinit();
	   frog_delete brick;
	   brick = NULL;
   }
   fileReader->Level2Deinit();
   fileReader->Level3Deinit();
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
   unsigned int dd = theClock->LoopTickCountGet();
   Userpaddle->Update(dt);
   //brick->Update();
   // The position of the ball corresponds to its center.  We want to keep the
   // whole ball on-screen, so figure out the area within which the center must
   // stay.
   Point2F ballSize = Point2F::Create(ball->image->SizeGet());
   Point2F ball2Size = Point2F::Create(ball->image2->SizeGet());
   Point2F ball3Size = Point2F::Create(ball->image3->SizeGet());
   Point2F halfBallSize = ballSize / 2.0f;
   Point2F halfBall2Size = ball2Size / 2.0f;
   Point2F halfBall3Size = ball3Size / 2.0f;
   Box2F ballArea = { halfBallSize.x, halfBallSize.y,
	   (float)theScreen->WidthGet() - ballSize.x,
	   (float)theScreen->HeightGet() - ballSize.y };
   Box2F ball2Area = { halfBall2Size.x, halfBall2Size.y,
	   (float)theScreen->WidthGet() - ball2Size.x,
	   (float)theScreen->HeightGet() - ball2Size.y };
   Box2F ball3Area = { halfBall3Size.x, halfBall3Size.y,
	   (float)theScreen->WidthGet() - ball3Size.x,
	   (float)theScreen->HeightGet() - ball3Size.y };


   // If the ball has gone too far in any direction, make sure its velocity
   // will bring it back.
   if (collisionAtTick + 100 <= dd)
   {
	   allowCollision = true;
   }
   //Check for collision of the ball and the paddle
   bool intersect = intersects(Userpaddle, ball);
   if ((intersect) && (allowCollision))
   {
	   collisionAtTick = theClock->LoopTickCountGet();
	   ball->velocity.x *= 1.0f;
	   ball->velocity.y *= -1.0f;
	   ball->positive = false;
	   allowCollision = false;
   }
   bool intersect2 = intersects2(Userpaddle, ball);
   if ((intersect2) && (allowCollision))
   {
	   collisionAtTick = theClock->LoopTickCountGet();
	   ball->velocity2.x *= 1.0f;
	   ball->velocity2.y *= -1.0f;
	   ball->positive2 = false;
	   allowCollision = false;
   }
   bool intersect3 = intersects3(Userpaddle, ball);
   if ((intersect3) && (allowCollision))
   {
	   collisionAtTick = theClock->LoopTickCountGet();
	   ball->velocity3.x *= 1.0f;
	   ball->velocity3.y *= -1.0f;
	   ball->positive3 = false;
	   allowCollision = false;
   }
   brick->level2 = true;
   brick->level3 = true;
   brick->level4 = true;
   for (int i = 0; i < 20; i++)
   {
	   for (int j = 0; j < 20; j++)
	   {
		   if (brick->brickGridX[i][j] > -300 && brick->bricknum[i][j] != 4)
		   {
			   brick->level2 = false;
			   goto exitforloop;
		   }
	   }
   }

exitforloop:

   for (int i = 0; i < 20; i++)
   {
	   for (int j = 0; j < 20; j++)
	   {
		   if (brick->brickGridX2[i][j] > -300 && brick->bricknum2[i][j] !=4)
		   {
			   brick->level3 = false;
			   goto exitforloop2;
		   }
	   }
   }

exitforloop2:

   for (int i = 0; i < 20; i++)
   {
	   for (int j = 0; j < 20; j++)
	   {
		   if (brick->brickGridX3[i][j] > -300)
		   {
			   brick->level4 = false;
			   goto exitforloop3;
		   }
	   }
   }

exitforloop3:

   if (brick->level1)
   {
	   //Collision detection for the bricks
	   for (int i = 0; i < 20; i++)
	   {
		   for (int j = 0; j < 20; j++)
		   {
			   if (ball->position.x >= brick->brickGridX[i][j] && ball->position.x <= brick->brickGridX[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position.y - ball->image->HeightGet() / 2 >= brick->brickGridY[i] && ball->position.y - ball->image->HeightGet() / 2 <= brick->brickGridY[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity.x *= 1.0f;
					   ball->velocity.y *= -1.0f;
					   ball->positive = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX[i][j];
							   Power->powerpos.y = brick->brickGridY[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum[i][j] == 1)
						   brick->brickGridX[i][j] = -300;
					   if (brick->bricknum[i][j] == 2)
						   brick->bricknum[i][j] = 1;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum[i][j] = 4;
				   }
			   }
			   if (ball->position2.x >= brick->brickGridX[i][j] && ball->position2.x <= brick->brickGridX[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position2.y - ball->image2->HeightGet() / 2 >= brick->brickGridY[i] && ball->position2.y - ball->image2->HeightGet() / 2 <= brick->brickGridY[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity2.x *= 1.0f;
					   ball->velocity2.y *= -1.0f;
					   ball->positive2 = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX[i][j];
							   Power->powerpos.y = brick->brickGridY[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum[i][j] == 1)
						   brick->brickGridX[i][j] = -300;
					   if (brick->bricknum[i][j] == 2)
						   brick->bricknum[i][j] = 1;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum[i][j] = 4;
				   }
			   }
			   if (ball->position3.x >= brick->brickGridX[i][j] && ball->position3.x <= brick->brickGridX[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position3.y - ball->image3->HeightGet() / 2 >= brick->brickGridY[i] && ball->position3.y - ball->image3->HeightGet() / 2 <= brick->brickGridY[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity3.x *= 1.0f;
					   ball->velocity3.y *= -1.0f;
					   ball->positive3 = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX[i][j];
							   Power->powerpos.y = brick->brickGridY[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum[i][j] == 1)
						   brick->brickGridX[i][j] = -300;
					   if (brick->bricknum[i][j] == 2)
						   brick->bricknum[i][j] = 1;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum[i][j] = 4;
				   }
			   }
			   if (Power->bulletpos.x >= brick->brickGridX[i][j] && Power->bulletpos.x <= brick->brickGridX[i][j] + brick->brickImage->WidthGet())
			   {
				   if (Power->bulletpos.y <= brick->brickGridY[i])
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   Power->bulletpos.x = -100.0f;
					   Power->bulletImg = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX[i][j];
							   Power->powerpos.y = brick->brickGridY[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum[i][j] == 1)
						   brick->brickGridX[i][j] = -300;
					   if (brick->bricknum[i][j] == 2)
						   brick->bricknum[i][j] = 1;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum[i][j] = 4;
				   }
			   }

			   //===================================================================================================================================================================================
			   if (ball->position.x >= brick->brickGridX[i][j] && ball->position.x <= brick->brickGridX[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position.y + ball->image->HeightGet() / 2) >= (brick->brickGridY[i] - brick->brickImage->HeightGet() / 2) && (ball->position.y) <= brick->brickGridY[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity.x *= 1.0f;
					   ball->velocity.y *= -1.0f;
					   ball->positive = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX[i][j];
							   Power->powerpos.y = brick->brickGridY[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum[i][j] == 1)
						   brick->brickGridX[i][j] = -300;
					   if (brick->bricknum[i][j] == 2)
						   brick->bricknum[i][j] = 1;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum[i][j] = 4;
				   }
			   }
			   if (ball->position2.x >= brick->brickGridX[i][j] && ball->position2.x <= brick->brickGridX[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position2.y + ball->image2->HeightGet() / 2) >= (brick->brickGridY[i] - brick->brickImage->HeightGet() / 2) && (ball->position2.y) <= brick->brickGridY[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity2.x *= 1.0f;
					   ball->velocity2.y *= -1.0f;
					   ball->positive2 = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX[i][j];
							   Power->powerpos.y = brick->brickGridY[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum[i][j] == 1)
						   brick->brickGridX[i][j] = -300;
					   if (brick->bricknum[i][j] == 2)
						   brick->bricknum[i][j] = 1;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum[i][j] = 4;
				   }
			   }
			   if (ball->position3.x >= brick->brickGridX[i][j] && ball->position3.x <= brick->brickGridX[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position3.y + ball->image3->HeightGet() / 2) >= (brick->brickGridY[i] - brick->brickImage->HeightGet() / 2) && (ball->position3.y) <= brick->brickGridY[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity3.x *= 1.0f;
					   ball->velocity3.y *= -1.0f;
					   ball->positive3 = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX[i][j];
							   Power->powerpos.y = brick->brickGridY[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum[i][j] == 1)
						   brick->brickGridX[i][j] = -300;
					   if (brick->bricknum[i][j] == 2)
						   brick->bricknum[i][j] = 1;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum[i][j] == 3)
						   brick->bricknum[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum[i][j] = 4;
				   }
			   }

		   }
	   }
   }
   
   //=============================================================================================================================================================================================================================================================================================================
   //=======================================================================      __       __             __  ====================================================================================================================================================================================================
   //======================================================================= |	 |__ \	/ |__ \  / |      __| ====================================================================================================================================================================================================
   //======================================================================= |__ |__  \/  |__  \/  |__   |__  ====================================================================================================================================================================================================
   //=======================================================================                                  ====================================================================================================================================================================================================
   //=============================================================================================================================================================================================================================================================================================================
   //Collision detection for the bricks
   if (brick->level2)
   {
	   for (int i = 0; i < 20; i++)
	   {
		   for (int j = 0; j < 20; j++)
		   {
			   if (ball->position.x >= brick->brickGridX2[i][j] && ball->position.x <= brick->brickGridX2[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position.y - ball->image->HeightGet() / 2 >= brick->brickGridY2[i] && ball->position.y - ball->image->HeightGet() / 2 <= brick->brickGridY2[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity.x *= 1.0f;
					   ball->velocity.y *= -1.0f;
					   ball->positive = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX2[i][j];
							   Power->powerpos.y = brick->brickGridY2[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }

					   if (brick->bricknum2[i][j] == 1)
						   brick->brickGridX2[i][j] = -300;
					   if (brick->bricknum2[i][j] == 2)
						   brick->bricknum2[i][j] = 1;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum2[i][j] = 4;
				   }
			   }
			   if (ball->position2.x >= brick->brickGridX2[i][j] && ball->position2.x <= brick->brickGridX2[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position2.y - ball->image2->HeightGet() / 2 >= brick->brickGridY2[i] && ball->position2.y - ball->image2->HeightGet() / 2 <= brick->brickGridY2[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity2.x *= 1.0f;
					   ball->velocity2.y *= -1.0f;
					   ball->positive2 = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX2[i][j];
							   Power->powerpos.y = brick->brickGridY2[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }

					   if (brick->bricknum2[i][j] == 1)
						   brick->brickGridX2[i][j] = -300;
					   if (brick->bricknum2[i][j] == 2)
						   brick->bricknum2[i][j] = 1;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum2[i][j] = 4;
				   }
			   }
			   if (ball->position3.x >= brick->brickGridX2[i][j] && ball->position3.x <= brick->brickGridX2[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position3.y - ball->image3->HeightGet() / 2 >= brick->brickGridY2[i] && ball->position3.y - ball->image3->HeightGet() / 2 <= brick->brickGridY2[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity3.x *= 1.0f;
					   ball->velocity3.y *= -1.0f;
					   ball->positive3 = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX2[i][j];
							   Power->powerpos.y = brick->brickGridY2[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }

					   if (brick->bricknum2[i][j] == 1)
						   brick->brickGridX2[i][j] = -300;
					   if (brick->bricknum2[i][j] == 2)
						   brick->bricknum2[i][j] = 1;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum2[i][j] = 4;
				   }
			   }
			   if (Power->bulletpos.x >= brick->brickGridX2[i][j] && Power->bulletpos.x <= brick->brickGridX2[i][j] + brick->brickImage->WidthGet())
			   {
				   if (Power->bulletpos.y <= brick->brickGridY2[i])
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   Power->bulletpos.x = -100.0f;
					   Power->bulletImg = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX2[i][j];
							   Power->powerpos.y = brick->brickGridY2[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum2[i][j] == 1)
						   brick->brickGridX2[i][j] = -300;
					   if (brick->bricknum2[i][j] == 2)
						   brick->bricknum2[i][j] = 1;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum2[i][j] = 4;
				   }
			   }

			   //===================================================================================================================================================================================
			   if (ball->position.x >= brick->brickGridX2[i][j] && ball->position.x <= brick->brickGridX2[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position.y + ball->image->HeightGet() / 2) >= (brick->brickGridY2[i] - brick->brickImage->HeightGet() / 2) && (ball->position.y) <= brick->brickGridY2[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity.x *= 1.0f;
					   ball->velocity.y *= -1.0f;
					   ball->positive = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX2[i][j];
							   Power->powerpos.y = brick->brickGridY2[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum2[i][j] == 1)
						   brick->brickGridX2[i][j] = -300;
					   if (brick->bricknum2[i][j] == 2)
						   brick->bricknum2[i][j] = 1;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum2[i][j] = 4;
				   }
			   }
			   if (ball->position2.x >= brick->brickGridX2[i][j] && ball->position2.x <= brick->brickGridX2[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position2.y + ball->image2->HeightGet() / 2) >= (brick->brickGridY2[i] - brick->brickImage->HeightGet() / 2) && (ball->position2.y) <= brick->brickGridY2[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity2.x *= 1.0f;
					   ball->velocity2.y *= -1.0f;
					   ball->positive2 = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX2[i][j];
							   Power->powerpos.y = brick->brickGridY2[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum2[i][j] == 1)
						   brick->brickGridX2[i][j] = -300;
					   if (brick->bricknum2[i][j] == 2)
						   brick->bricknum2[i][j] = 1;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum2[i][j] = 4;
				   }
			   }
			   if (ball->position3.x >= brick->brickGridX2[i][j] && ball->position3.x <= brick->brickGridX2[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position3.y + ball->image3->HeightGet() / 2) >= (brick->brickGridY2[i] - brick->brickImage->HeightGet() / 2) && (ball->position3.y) <= brick->brickGridY2[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity3.x *= 1.0f;
					   ball->velocity3.y *= -1.0f;
					   ball->positive3 = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX2[i][j];
							   Power->powerpos.y = brick->brickGridY2[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum2[i][j] == 1)
						   brick->brickGridX2[i][j] = -300;
					   if (brick->bricknum2[i][j] == 2)
						   brick->bricknum2[i][j] = 1;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 3)
						   brick->bricknum2[i][j] = 2;
					   if (brick->bricknum2[i][j] == 4)
						   brick->bricknum2[i][j] = 4;
				   }
			   }

		   }
	   }
   }

   //=============================================================================================================================================================================================================================================================================================================
   //=======================================================================      __       __             __  ====================================================================================================================================================================================================
   //======================================================================= |	 |__ \	/ |__ \  / |      __| ====================================================================================================================================================================================================
   //======================================================================= |__ |__  \/  |__  \/  |__    __| ====================================================================================================================================================================================================
   //=======================================================================                                  ====================================================================================================================================================================================================
   //=============================================================================================================================================================================================================================================================================================================
   //Collision detection for the bricks
   if (brick->level3)
   {
	   for (int i = 0; i < 20; i++)
	   {
		   for (int j = 0; j < 20; j++)
		   {
			   if (ball->position.x >= brick->brickGridX3[i][j] && ball->position.x <= brick->brickGridX3[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position.y - ball->image->HeightGet() / 2 >= brick->brickGridY3[i] && ball->position.y - ball->image->HeightGet() / 2 <= brick->brickGridY3[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity.x *= 1.0f;
					   ball->velocity.y *= -1.0f;
					   ball->positive = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX3[i][j];
							   Power->powerpos.y = brick->brickGridY3[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }

					   if (brick->bricknum3[i][j] == 1)
						   brick->brickGridX3[i][j] = -300;
					   if (brick->bricknum3[i][j] == 2)
						   brick->bricknum3[i][j] = 1;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 4)
						   brick->bricknum3[i][j] = 4;
				   }
			   }
			   if (ball->position2.x >= brick->brickGridX3[i][j] && ball->position2.x <= brick->brickGridX3[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position2.y - ball->image2->HeightGet() / 2 >= brick->brickGridY3[i] && ball->position2.y - ball->image2->HeightGet() / 2 <= brick->brickGridY3[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity2.x *= 1.0f;
					   ball->velocity2.y *= -1.0f;
					   ball->positive2 = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX3[i][j];
							   Power->powerpos.y = brick->brickGridY3[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }

					   if (brick->bricknum3[i][j] == 1)
						   brick->brickGridX3[i][j] = -300;
					   if (brick->bricknum3[i][j] == 2)
						   brick->bricknum3[i][j] = 1;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 4)
						   brick->bricknum3[i][j] = 4;
				   }
			   }
			   if (ball->position3.x >= brick->brickGridX3[i][j] && ball->position3.x <= brick->brickGridX3[i][j] + brick->brickImage->WidthGet())
			   {
				   if (ball->position3.y - ball->image3->HeightGet() / 2 >= brick->brickGridY3[i] && ball->position3.y - ball->image3->HeightGet() / 2 <= brick->brickGridY3[i] + brick->brickImage->HeightGet())
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity3.x *= 1.0f;
					   ball->velocity3.y *= -1.0f;
					   ball->positive3 = true;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX3[i][j];
							   Power->powerpos.y = brick->brickGridY3[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }

					   if (brick->bricknum3[i][j] == 1)
						   brick->brickGridX3[i][j] = -300;
					   if (brick->bricknum3[i][j] == 2)
						   brick->bricknum3[i][j] = 1;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 4)
						   brick->bricknum3[i][j] = 4;
				   }
			   }
			   if (Power->bulletpos.x >= brick->brickGridX3[i][j] && Power->bulletpos.x <= brick->brickGridX3[i][j] + brick->brickImage->WidthGet())
			   {
				   if (Power->bulletpos.y <= brick->brickGridY3[i])
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   Power->bulletpos.x = -100.0f;
					   Power->bulletImg = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX3[i][j];
							   Power->powerpos.y = brick->brickGridY3[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum3[i][j] == 1)
						   brick->brickGridX3[i][j] = -300;
					   if (brick->bricknum3[i][j] == 2)
						   brick->bricknum3[i][j] = 1;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 4)
						   brick->bricknum3[i][j] = 4;
				   }
			   }

			   //===================================================================================================================================================================================
			   if (ball->position.x >= brick->brickGridX3[i][j] && ball->position.x <= brick->brickGridX3[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position.y + ball->image->HeightGet() / 2) >= (brick->brickGridY3[i] - brick->brickImage->HeightGet() / 2) && (ball->position.y) <= brick->brickGridY3[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity.x *= 1.0f;
					   ball->velocity.y *= -1.0f;
					   ball->positive = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX3[i][j];
							   Power->powerpos.y = brick->brickGridY3[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum3[i][j] == 1)
						   brick->brickGridX3[i][j] = -300;
					   if (brick->bricknum3[i][j] == 2)
						   brick->bricknum3[i][j] = 1;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 4)
						   brick->bricknum3[i][j] = 4;
				   }
			   }
			   if (ball->position2.x >= brick->brickGridX3[i][j] && ball->position2.x <= brick->brickGridX3[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position2.y + ball->image2->HeightGet() / 2) >= (brick->brickGridY3[i] - brick->brickImage->HeightGet() / 2) && (ball->position2.y) <= brick->brickGridY3[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity2.x *= 1.0f;
					   ball->velocity2.y *= -1.0f;
					   ball->positive2 = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX3[i][j];
							   Power->powerpos.y = brick->brickGridY3[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum3[i][j] == 1)
						   brick->brickGridX3[i][j] = -300;
					   if (brick->bricknum3[i][j] == 2)
						   brick->bricknum3[i][j] = 1;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 4)
						   brick->bricknum3[i][j] = 4;
				   }
			   }
			   if (ball->position3.x >= brick->brickGridX3[i][j] && ball->position3.x <= brick->brickGridX3[i][j] + brick->brickImage->WidthGet())
			   {
				   if (((ball->position3.y + ball->image3->HeightGet() / 2) >= (brick->brickGridY3[i] - brick->brickImage->HeightGet() / 2) && (ball->position3.y) <= brick->brickGridY3[i] && ball->positive == true))
				   {
					   PlaySound("BrickHit", NULL, SND_ASYNC);
					   ball->velocity3.x *= 1.0f;
					   ball->velocity3.y *= -1.0f;
					   ball->positive3 = false;
					   Power->powerup = false;
					   if (!Power->powerup)
					   {
						   Power->powerRand = rand() % 1000 + 1;
						   if (Power->powerRand <= 200 && Power->powerRand >= 1)
						   {
							   Power->powerup = true;
							   Power->powerpos.x = brick->brickGridX3[i][j];
							   Power->powerpos.y = brick->brickGridY3[i];
							   Power->powerChoice = rand() % 4 + 1;
							   //Power->powerChoice = 4;
						   }
					   }
					   if (brick->bricknum3[i][j] == 1)
						   brick->brickGridX3[i][j] = -300;
					   if (brick->bricknum3[i][j] == 2)
						   brick->bricknum3[i][j] = 1;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 3)
						   brick->bricknum3[i][j] = 2;
					   if (brick->bricknum3[i][j] == 4)
						   brick->bricknum3[i][j] = 4;
				   }
			   }

		   }
	   }
   }
   Power->Update();
   ball->Update(dt);

   //Collision detection between the paddle and the powerup
   if (Power->powerpos2.y + Power->powerImage->HeightGet() / 2 >= Userpaddle->padpos.y - Userpaddle->paddleImage->HeightGet() / 2 
	   && Power->powerpos2.x + Power->powerImage->WidthGet() <= Userpaddle->padpos.x + Userpaddle->paddleImage->WidthGet() && Power->powerpos2.x + Power->powerImage->WidthGet() >= Userpaddle->padpos.x)
   {
	   Power->powImg1 = false;
	   Power->powerup = false;
	   Power->powerRand = 0;
	   Power->powerCollision = true;
	   Power->powerpos2.x = -200.0f;
   }
   if (Power->powerpos2.y >= theScreen->HeightGet())
   {
	   Power->powImg1 = false;
	   Power->powerup = false;
	   Power->powerRand = 0;
	   Power->powerpos2.x = -200.0f;
   }

   //Determine which power up to activate
   if (Power->powerCollision)
   {
	   if (Power->powerImage == theImages->Load("PowerUp1"))
	   {
		   score += 1000;
		   Power->powerCollision = false;
	   }
	   if (Power->powerImage == theImages->Load("PowerUp2"))
	   {
		   ball->position2.x = Userpaddle->padpos.x;
		   ball->position2.y = Userpaddle->padpos.y - 20.0f;
		   ball->position3.x = Userpaddle->padpos.x;
		   ball->position3.y = Userpaddle->padpos.y - 20.0f;
		   ball->velocity2.Set(300.0f, -200.0f);
		   ball->velocity3.Set(200.0f, -300.0f);
		   ball->ballAlive = true;
		   ball->ballAlive2 = true;
		   ball->moveball = true;
		   Power->powerCollision = false;
	   }
	   if (Power->powerImage == theImages->Load("PowerUp3"))
	   {
		   Userpaddle->paddleImage = theImages->Load("PaddleLarge");
		   Power->powerCollision = false;
		   Power->allowShoot = false;
		   powerTime = theClock->LoopTickCountGet();
	   }
	   if (Power->powerImage == theImages->Load("PowerUp4"))
	   {
		   Userpaddle->paddleImage = theImages->Load("PaddleShooter");
		   Power->powerCollision = false;
		   Power->allowShoot = true;
		   powerTime = theClock->LoopTickCountGet();
	   }
   }
   
   //Shoot bullet
   if (theKeyboard->KeyJustPressed(KEY_UP) && Power->allowShoot && !Power->bulletImg)
   {
	   PlaySound("Laser", NULL, SND_ASYNC);
	   Power->bulletpos.x = Userpaddle->padpos.x + Userpaddle->paddleImage->WidthGet() / 2;
	   Power->bulletpos.y = Userpaddle->padpos.y;
	   Power->bulletImg = true;
   }

   //Reset paddle if power timer is up
   if (powerTime + 20000 <= dd && Userpaddle->paddleImage == theImages->Load("PaddleLarge"))
   {
	   Userpaddle->paddleImage = theImages->Load("UserPaddle");
   }
   if (powerTime + 40000 <= dd && Userpaddle->paddleImage == theImages->Load("PaddleShooter"))
   {
	   Userpaddle->paddleImage = theImages->Load("UserPaddle");
	   Power->allowShoot = false;
   }
   
   // See if ball1 is too far right.
   if ((ball->position.x > ballArea.MaxXGet()) && (ball->velocity.x > 0.0f))
   {
	   ball->velocity.x *= -1.0f;
   }
   // See if ball2 is too far right.
   if ((ball->position2.x > ball2Area.MaxXGet()) && (ball->velocity2.x > 0.0f))
   {
	   ball->velocity2.x *= -1.0f;
   }
   // See if ball3 is too far right.
   if ((ball->position3.x > ball3Area.MaxXGet()) && (ball->velocity3.x > 0.0f))
   {
	   ball->velocity3.x *= -1.0f;
   }

   // See if ball1 too far left
   if ((ball->position.x < ballArea.x) && (ball->velocity.x < 0.0f))
   {
	   ball->velocity.x *= -1.0f;
   }
   if ((ball->position2.x < ball2Area.x) && (ball->velocity2.x < 0.0f))
   {
	   ball->velocity2.x *= -1.0f;
   }
   if ((ball->position3.x < ball3Area.x) && (ball->velocity3.x < 0.0f))
   {
	   ball->velocity3.x *= -1.0f;
   }
   
   // See if it's too far down.
   if ((ball->position.y > ballArea.MaxYGet()) && (ball->velocity.y > 0.0f))
   {
	   //Power->powerup = false;
	   if (!ball->ballAlive && !ball->ballAlive2)
	   {
		   ball->position = Point2F::Create(theScreen->SizeGet() / 2);
		   ball->velocity.Set(0, 0);
		   gamePaused = true;
		   ball->moveball = false;
		   
	   }
   }
   if ((ball->position2.y > ball2Area.MaxYGet()) && (ball->velocity2.y > 0.0f))
   {
	   ball->position2.Set(-2000.0f, 100.0f);
	   ball->velocity2.Set(0, 0);
	   ball->ballAlive = false;
	   Power->powerup = false;
   }
   if ((ball->position3.y > ball3Area.MaxYGet()) && (ball->velocity3.y > 0.0f))
   {
	   ball->position3.Set(-2000.0f, 100.0f);
	   ball->velocity3.Set(0, 0);
	   ball->ballAlive2 = false;
	   Power->powerup = false;

   }
   // See if it's too far up.
   if ((ball->position.y < ballArea.y) && (ball->velocity.y < 0.0f))
   {
	   ball->velocity.y *= -1.0f;
	   ball->positive = true;
   }
   if ((ball->position2.y < ball2Area.y) && (ball->velocity2.y < 0.0f))
   {
	   ball->velocity2.y *= -1.0f;
	   ball->positive2 = true;
   }
   if ((ball->position3.y < ball3Area.y) && (ball->velocity3.y < 0.0f))
   {
	   ball->velocity3.y *= -1.0f;
	   ball->positive3 = true;
   }
   // See if bullet is too far up
   if (Power->bulletpos.y + Power->bulletImage->HeightGet() < theScreen->HeightGet() - theScreen->HeightGet())
   {
	   Power->bulletImg = false;
   }
   // Return to the previous menu if the escape key is pressed.
   if(!theStates->StateChangeCheck() && theKeyboard->KeyJustPressed(KEY_ESCAPE))
   {
      theMainGame->StateChangeTransitionBegin(true);
      theStates->Pop();
   }
   if ((theKeyboard->KeyJustPressed(KEY_SPACE)) && (gamePaused)){
	   ball->velocity.Set(BALL_AXIS_SPEED, BALL_AXIS_SPEED);
	   gamePaused = false;
	   if (newGame)
	   {
		   //brick->Init();
	   }
   }
}

bool MainGame::intersects(UserPaddle * pd, Ball * b)
{
	double circleDistancex = std::abs(((pd->padpos.x + 60)) - b->position.x);
	double circleDistancey = std::abs((pd->padpos.y + 11) - b->position.y);
	
	double pdwidth = pd->paddleImage->WidthGet();
	double pdheight = pd->paddleImage->HeightGet();

	if (circleDistancex > (pdwidth / 1.5)) { return false; }
	if (circleDistancey > (pdheight / 2 + b->image->HeightGet() / 2)) { return false; }

	if (circleDistancex = ((pdwidth))) { return true; }
	if (circleDistancey = (pdheight)) { return true; }
	
	double radius = pd->paddleImage->WidthGet() / 2;
	double cornerDistance_sq = std::pow(circleDistancex - pdwidth, 2) + std::pow(circleDistancey - pdheight, 2);

	return (cornerDistance_sq <= (std::pow(radius, 2)));

}
bool MainGame::intersects2(UserPaddle * pd, Ball * b)
{
	double circleDistancex2 = std::abs(((pd->padpos.x + 60)) - b->position2.x);
	double circleDistancey2 = std::abs((pd->padpos.y) - b->position2.y);

	double pdwidth = pd->paddleImage->WidthGet();
	double pdheight = pd->paddleImage->HeightGet();

	if (circleDistancex2 > (pdwidth / 1.5)) { return false; }
	if (circleDistancey2 > (pdheight / 2 + b->image->HeightGet() / 2)) { return false; }

	if (circleDistancex2 = ((pdwidth))) { return true; }
	if (circleDistancey2 = (pdheight)) { return true; }

	double radius = pd->paddleImage->WidthGet() / 2;
	double cornerDistance_sq = std::pow(circleDistancex2 - pdwidth, 2) + std::pow(circleDistancey2 - pdheight, 2);

	return (cornerDistance_sq <= (std::pow(radius, 2)));
}
bool MainGame::intersects3(UserPaddle * pd, Ball * b)
{
	double circleDistancex3 = std::abs(((pd->padpos.x + 60)) - b->position3.x);
	double circleDistancey3 = std::abs((pd->padpos.y) - b->position3.y);

	double pdwidth = pd->paddleImage->WidthGet();
	double pdheight = pd->paddleImage->HeightGet();

	if (circleDistancex3 > (pdwidth / 1.5)) { return false; }
	if (circleDistancey3 > (pdheight / 2 + b->image->HeightGet() / 2)) { return false; }

	if (circleDistancex3 = ((pdwidth))) { return true; }
	if (circleDistancey3 = (pdheight)) { return true; }

	double radius = pd->paddleImage->WidthGet() / 2;
	double cornerDistance_sq = std::pow(circleDistancex3 - pdwidth, 2) + std::pow(circleDistancey3 - pdheight, 2);

	return (cornerDistance_sq <= (std::pow(radius, 2)));
}

//-----------------------------------------------------------------------------

void MainGame::Draw()
{
	brick->Draw();
	Power->Draw();
	Userpaddle->Draw();
	ball->Draw();
	
}

//==============================================================================

UserPaddle::UserPaddle()
{
	paddleImage = NULL;
}

PowerUp::PowerUp()
{
	powerImage = NULL;
	powerImage2 = NULL;
	powerImage3 = NULL;
	powerImage4 = NULL;
	bulletImage = NULL;
}

Brick::Brick()
{
	brickImage = NULL;
	fileReader = NULL;
}

Ball::Ball()
{
   image = NULL;
   image2 = NULL;
   image3 = NULL;
}

void UserPaddle::Init()
{
	padpos.x = theScreen->WidthGet() / 2;
	padpos.y = theScreen->HeightGet() - 50.0f;
	paddleImage = theImages->Load("UserPaddle");
}

void PowerUp::Init()
{
	powerup = false;
	powImg1 = false;
	powerCollision = false;
	allowShoot = false;
	bulletImg = false;
	bulletpos.Set(0.0f, 0.0f);
	powerImage = theImages->Load("PowerUp1");
	powerImage2 = theImages->Load("PowerUp1");
	powerImage3 = theImages->Load("PowerUp1");
	powerImage4 = theImages->Load("PowerUp1");
	bulletImage = theImages->Load("Bullet");
}

void Brick::Init()
{
	brickImage = theImages->Load("brick1");
	brickpos.x = theScreen->WidthGet() / 2;
	brickpos.y = theScreen->HeightGet() / 4;
	fileReader = frog_new FileReader();
	fileReader->Level2Init();
	fileReader->Level3Init();
	level1 = true;
	level2 = true;
	level3 = true;
	level4 = true;

	//Level 1 brick position initialization
	for (int i = 0; i < 7; i++)
	{
		brickGridY[i] = 20 + (i * 30);
		for (int j = 0; j < 15; j++)
		{
			if (i >(i - 1))
			{
				brickGridX[i][j] = 0;
			}
			brickGridX[i][j] = 30 + (j * 60);
			if (i == 0 || i == 6)
				bricknum[i][j] = 1;
			if (i == 1 || i == 5)
				bricknum[i][j] = 2;
			if (i == 2 || i == 4)
				bricknum[i][j] = 3;
			if (i == 3)
				bricknum[i][j] = 4;
		}
	}
	//Level 2 brick position initialization
	for (int i = 0; i < 8; i++)
	{
		brickGridY2[i] = 20 + (i * 50);
		for (int j = 0; j < 20; j++)
		{
			brickGridX2[i][j] = fileReader->Level2_x[i][j];
			bricknum2[i][j] = fileReader->bricknum[i][j];
		}
	}
	//Level 3 brick position initialization
	for (int i = 0; i < 8; i++)
	{
		brickGridY3[i] = 20 + (i * 50);
		for (int j = 0; j < 20; j++)
		{
			brickGridX3[i][j] = fileReader->Level3_x[i][j];
			bricknum3[i][j] = fileReader->bricknum2[i][j];
		}
	}
}

void FileReader::Level2Init()
{
	A = false;
	B = false;
	C = false;
	D = false;
	J = true;
	ifstream Level2File("Level2 X Coords.DAT");
	if (Level2File.is_open())
	{
		row = 0;
		col = 0;
		while (getline(Level2File, line))
		{
			stringstream ss(line);
			if (line == "A")
				A = true;
			if (line == "B")
				B = true;
			if (line == "C")
				C = true;
			if (line == "D")
				D = true;
			while (J)
			{
				if (A)
				{
					getline(Level2File, line);
					stringstream ss(line);
					while (ss >> Level2_x[row][col])
					{
						bricknum[row][col] = 1;
						col++;
					}
					getline(Level2File, line);
					if (line == "J")
						J = false;
					if (line == "B")
						B = true;
					if (line == "C")
						C = true;
					if (line == "D")
						D = true;
					A = false;
				}
				if (B)
				{
					getline(Level2File, line);
					stringstream ss(line);
					while (ss >> Level2_x[row][col])
					{
						bricknum[row][col] = 2;
						col++;
					}
					getline(Level2File, line);
					if (line == "J")
						J = false;
					if (line == "A")
						A = true;
					if (line == "C")
						C = true;
					if (line == "D")
						D = true;
					B = false;
				}
				if (C)
				{
					getline(Level2File, line);
					stringstream ss(line);
					while (ss >> Level2_x[row][col])
					{
						bricknum[row][col] = 3;
						col++;
					}
					getline(Level2File, line);
					if (line == "J")
						J = false;
					if (line == "A")
						A = true;
					if (line == "B")
						B = true;
					if (line == "D")
						D = true;
					C = false;
				}
				if (D)
				{
					getline(Level2File, line);
					stringstream ss(line);
					while (ss >> Level2_x[row][col])
					{
						bricknum[row][col] = 4;
						col++;
					}
					getline(Level2File, line);
					if (line == "J")
						J = false;
					if (line == "A")
						A = true;
					if (line == "B")
						B = true;
					if (line == "C")
						C = true;
					D = false;
				}
				if (!J)
				{
					row++;
					col = 0;
					A = false;
					B = false;
					C = false;
					D = false;
				}
			}	
			J = true;
		}
		Level2File.close();
	}
}

void FileReader::Level3Init()
{
	A = false;
	B = false;
	C = false;
	D = false;
	J = true;
	ifstream Level3File("Level3 X Coords.DAT");
	if (Level3File.is_open())
	{
		row = 0;
		col = 0;
		while (getline(Level3File, line))
		{
			stringstream ss(line);
			if (line == "A")
				A = true;
			if (line == "B")
				B = true;
			if (line == "C")
				C = true;
			if (line == "D")
				D = true;
			while (J)
			{
				if (A)
				{
					getline(Level3File, line);
					stringstream ss(line);
					while (ss >> Level3_x[row][col])
					{
						bricknum2[row][col] = 1;
						col++;
					}
					getline(Level3File, line);
					if (line == "J")
						J = false;
					if (line == "B")
						B = true;
					if (line == "C")
						C = true;
					if (line == "D")
						D = true;
					A = false;
				}
				if (B)
				{
					getline(Level3File, line);
					stringstream ss(line);
					while (ss >> Level3_x[row][col])
					{
						bricknum2[row][col] = 2;
						col++;
					}
					getline(Level3File, line);
					if (line == "J")
						J = false;
					if (line == "A")
						A = true;
					if (line == "C")
						C = true;
					if (line == "D")
						D = true;
					B = false;
				}
				if (C)
				{
					getline(Level3File, line);
					stringstream ss(line);
					while (ss >> Level3_x[row][col])
					{
						bricknum2[row][col] = 3;
						col++;
					}
					getline(Level3File, line);
					if (line == "J")
						J = false;
					if (line == "A")
						A = true;
					if (line == "B")
						B = true;
					if (line == "D")
						D = true;
					C = false;
				}
				if (D)
				{
					getline(Level3File, line);
					stringstream ss(line);
					while (ss >> Level3_x[row][col])
					{
						bricknum2[row][col] = 4;
						col++;
					}
					getline(Level3File, line);
					if (line == "J")
						J = false;
					if (line == "A")
						A = true;
					if (line == "B")
						B = true;
					if (line == "C")
						C = true;
					D = false;
				}
				if (!J)
				{
					row++;
					col = 0;
					A = false;
					B = false;
					C = false;
					D = false;
				}
			}
			J = true;
		}
		Level3File.close();
	}
}

//------------------------------------------------------------------------------

void Ball::Init()
{
   // Load the image of the ball.
   image = theImages->Load("Ball");
   image2 = theImages->Load("Ball2");
   image3 = theImages->Load("Ball3");

   moveball = false;
   ballAlive = false;
   ballAlive = false;

   // Start the ball in the middle of the screen.
   position.x = 512.0f;
   position.y = 500.0f;

   // The ball will initially be moving down and to the right.
	velocity.Set(0, 0);
}

//------------------------------------------------------------------------------
void UserPaddle::Deinit()
{
	// Unload the image of the user's paddle.
	if (paddleImage)
	{
		theImages->Unload(paddleImage);
		paddleImage = NULL;
	}
}

void PowerUp::DeInit()
{
	if (powerImage)
	{
		theImages->Unload(powerImage);
		powerImage = NULL;
	}
	if (powerImage2)
	{
		theImages->Unload(powerImage2);
		powerImage2 = NULL;
	}
	if (powerImage3)
	{
		theImages->Unload(powerImage3);
		powerImage3 = NULL;
	}
	if (powerImage4)
	{
		theImages->Unload(powerImage4);
		powerImage4 = NULL;
	}
	if (bulletImage)
	{
		theImages->Unload(bulletImage);
		bulletImage = NULL;
	}
}

void Brick::Deinit()
{
	if (brickImage)
	{
		theImages->Unload(brickImage);
		brickImage = NULL;
	}
}

void FileReader::Level2Deinit()
{
	delete this;
}

void FileReader::Level3Deinit()
{
	delete this;
}

void Ball::Deinit()
{
   // Unload the image of the ball.
   if(image)
   {
      theImages->Unload(image);
      image = NULL;
   }
   if (image2)
   {
	   theImages->Unload(image2);
	   image2 = NULL;
   }
   if (image3)
   {
	   theImages->Unload(image3);
	   image3 = NULL;
   }
}

//------------------------------------------------------------------------------
void UserPaddle::Update(unsigned int dt)
{
	if (theKeyboard->KeyPressed(KEY_LEFT))
	{
		
		if (padpos.x <= 5)
		{
			
		}
		else
		{
			padpos.x -= 10.0f;
		}
	}
	if (theKeyboard->KeyPressed(KEY_RIGHT))
	{
		if (padpos.x > theScreen->WidthGet() - paddleImage->WidthGet())
		{
			
		}
		else
		{
			padpos.x += 10.0f;
		}
	}
}

void PowerUp::Update()
{
	if (powerup && !powImg1)
	{
		powerpos2.x = powerpos.x;
		powerpos2.y = powerpos.y;
		if (powerChoice == 1)
			powerImage = theImages->Load("PowerUp1");
		if (powerChoice == 2)
			powerImage = theImages->Load("PowerUp2");
		if (powerChoice == 3)
			powerImage = theImages->Load("PowerUp3");
		if (powerChoice == 4)
			powerImage = theImages->Load("PowerUp4");
		powImg1 = true;
	}
	if (powImg1)
		powerpos2.y += 3.0f;
	else
		powerpos2.x = -200.0;

	if (bulletImg)
	{
		bulletpos.y -= 20.0f;
	}

}

void Brick::Update()
{

}

void Ball::Update(unsigned int dt)
{
	// Get a floating point number for the duration of the last frame in seconds.
	float dtSeconds = (float)dt / 1000.0f;

	// Update the position of the ball.
	position += velocity * dtSeconds;
	position2 += velocity2 * dtSeconds;
	position3 += velocity3 * dtSeconds;
   
}

//------------------------------------------------------------------------------

void PowerUp::Draw()
{
	powerImage->Draw(powerpos2);
	if (bulletImg)
		bulletImage->Draw(bulletpos);
}

void Ball::Draw()
{
   // The center of the ball is in the center of the image, so use an offset.
   image->Draw(position - (Point2F::Create(image->SizeGet()) / 2.0f));
   if (moveball)
   {
	   image2->Draw(position2);
	   image3->Draw(position3);
   }
}

void Brick::Draw()
{
	if (level1)
	{
		for (int i = 0; i < 7; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				brickpos.x = brickGridX[i][j];
				brickpos.y = brickGridY[i];

				if (brickpos.x != -100)
				{
					if (bricknum[i][j] == 1)
					{
						brickImage = theImages->Load("Brick1");
						brickImage->Draw(brickpos);
					}
					if (bricknum[i][j] == 2)
					{
						brickImage = theImages->Load("Brick2");
						brickImage->Draw(brickpos);
					}
					if (bricknum[i][j] == 3)
					{
						brickImage = theImages->Load("Brick3");
						brickImage->Draw(brickpos);
					}
					if (bricknum[i][j] == 4)
					{
						brickImage = theImages->Load("Brick4");
						brickImage->Draw(brickpos);
					}
				}
			}
		}
	}
	
	if (level2)
	{
		level1 = false;
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				
				brickpos.x = brickGridX2[i][j];
				brickpos.y = brickGridY2[i];
				if (bricknum2[i][j] == 1)
				{
					brickImage = theImages->Load("Brick1");
					brickImage->Draw(brickpos);
				}
				if (bricknum2[i][j] == 2)
				{
					brickImage = theImages->Load("Brick2");
					brickImage->Draw(brickpos);
				}
				if (bricknum2[i][j] == 3)
				{
					brickImage = theImages->Load("Brick3");
					brickImage->Draw(brickpos);
				}
				if (bricknum2[i][j] == 4)
				{
					brickImage = theImages->Load("Brick4");
					brickImage->Draw(brickpos);
				}
			}
		}
	}
	if (level3)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				brickpos.x = brickGridX3[i][j];
				brickpos.y = brickGridY3[i];
				if (bricknum3[i][j] == 1)
				{
					brickImage = theImages->Load("Brick1");
					brickImage->Draw(brickpos);
				}
				if (bricknum3[i][j] == 2)
				{
					brickImage = theImages->Load("Brick2");
					brickImage->Draw(brickpos);
				}
				if (bricknum3[i][j] == 3)
				{
					brickImage = theImages->Load("Brick3");
					brickImage->Draw(brickpos);
				}
				if (bricknum3[i][j] == 4)
				{
					brickImage = theImages->Load("Brick4");
					brickImage->Draw(brickpos);
				}
			}
		}
	}
}

void UserPaddle::Draw()
{
	paddleImage->Draw(padpos);
}

//------------------------------------------------------------------------------