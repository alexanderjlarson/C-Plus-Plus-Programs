#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"
//#include "Paddle.h"
#include <cstdlib>
#include <iostream>

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
   AIpaddle = NULL;
   ball = NULL;
   score = NULL;
   //firework = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();

   // Create and initialize the ball.
   
   ball = frog_new Ball();
   ball->Init();
   Userpaddle = frog_new UserPaddle();
   Userpaddle->Init();
   AIpaddle = frog_new AIPaddle();
   AIpaddle->Init();
   score = frog_new Score();
   score->Init();
   //firework = frog_new FireWork();
   gamePaused = true;
   allowCollision = true;
   newGame = true;
   allowVelChange = false;
   //score->userScore = 0;
   //score->aiScore = 0;

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
   if (AIpaddle)
   {
	   AIpaddle->Deinit();
	   frog_delete AIpaddle;
	   AIpaddle = NULL;
   }
   if (score)
   {
	   score->Deinit();
	   frog_delete score;
	   score = NULL;
   }
   
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
   AIpaddle->Update(dt);
   score->Update();
   // The position of the ball corresponds to its center.  We want to keep the
   // whole ball on-screen, so figure out the area within which the center must
   // stay.
   Point2F ballSize = Point2F::Create(ball->image->SizeGet());
   Point2F halfBallSize = ballSize / 2.0f;
   Box2F ballArea = { halfBallSize.x, halfBallSize.y,
	   (float)theScreen->WidthGet() - ballSize.x,
	   (float)theScreen->HeightGet() - ballSize.y };

   // If the ball has gone too far in any direction, make sure its velocity
   // will bring it back.
   if (collisionAtTick + 1000 <= dd)
   {
	   allowCollision = true;
   }
   //Check for collision of the ball and the paddle
   bool intersect = intersects(Userpaddle, ball);
   if ((intersect) && (allowCollision))
   {
	   collisionAtTick = theClock->LoopTickCountGet();
	   ball->velocity.x *= -1.02f;
	   ball->velocity.y *= 1.02f;
	   //AIpaddle->randVel = FrogMath::Random(4);
	   allowCollision = false;
   }
   bool aiIntersect = aiintersects(AIpaddle, ball);
   if ((aiIntersect) && (allowCollision))
   {
	   collisionAtTick = theClock->LoopTickCountGet();
	   ball->velocity.x *= -1.02f;
	   ball->velocity.y *= 1.02f;
	   //AIpaddle->randVel = FrogMath::Random(4);
	   allowCollision = false;
	   allowVelChange = true;
   }

   ball->Update(dt);


   if (ball->position.y > AIpaddle->aipadpos.y)
   {
	   //AIpaddle->aipadpos.y += (10.0f * AIpaddle->paddleVelocity[AIpaddle->randVel]);
	   AIpaddle->aipadpos.y += 10.0f;
   }
   if (ball->position.y < AIpaddle->aipadpos.y)
   {
	   //AIpaddle->aipadpos.y -= (10.0f * AIpaddle->paddleVelocity[AIpaddle->randVel]);
	   AIpaddle->aipadpos.y -= 10.0f;
   }
   // See if it's too far right.
   if ((ball->position.x > ballArea.MaxXGet()) && (ball->velocity.x > 0.0f))
   {
	   if (score->userScore == 2)
	   {
		   newGame = true;
		   score->userScore = 3;
	   }
	   else
	   {
		   score->userScore++;
		   newGame = false;
	   }

	   ball->position = Point2F::Create(theScreen->SizeGet() / 2);
	   ball->velocity.Set(0, 0);
	   gamePaused = true;
   }

   // too far left
   if ((ball->position.x < ballArea.x) && (ball->velocity.x < 0.0f))
   {
	   if (score->aiScore == 2)
	   {
		   newGame = true;
		   score->aiScore = 3;
	   }
	   else
	   {
		   score->aiScore++;
		   newGame = false;
	   }
	   ball->position = Point2F::Create(theScreen->SizeGet() / 2);
	   ball->velocity.Set(0, 0);
	   gamePaused = true;
   }
   
   // See if it's too far down.
   if ((ball->position.y > ballArea.MaxYGet()) && (ball->velocity.y > 0.0f))
	   ball->velocity.y *= -1.0f;

   // See if it's too far up.
   if ((ball->position.y < ballArea.y) && (ball->velocity.y < 0.0f))
	   ball->velocity.y *= -1.0f;

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
		   score->resultImage = theImages->Load("blank");
		   score->userScore = 0;
		   score->aiScore = 0;
	   }
   }
}

bool MainGame::intersects(UserPaddle * pd, Ball * b)
{
	double circleDistancex = std::abs(pd->padpos.x - b->position.x);
	double circleDistancey = std::abs((pd->padpos.y + 50) - b->position.y);
	double pdwidth = pd->paddleImage->WidthGet();
	double pdheight = pd->paddleImage->HeightGet();
	if (circleDistancex > (pdwidth + b->image->WidthGet() / 2)) { return false; }
	if (circleDistancey > (pdheight / 2 + b->image->HeightGet() / 2)) { return false; }


	if (circleDistancex = (pdwidth)) { return true; }
	if (circleDistancey = (pdheight)) { return true; }


	double radius = pd->paddleImage->WidthGet() / 2;
	double cornerDistance_sq = std::pow(circleDistancex - pdwidth, 2) + std::pow(circleDistancey - pdheight, 2);

	return (cornerDistance_sq <= (std::pow(radius, 2)));

}

bool MainGame::aiintersects(AIPaddle * aipd, Ball * b)
{
	double circleDistancex = std::abs(b->position.x - aipd->aipadpos.x);
	double circleDistancey = std::abs(b->position.y - (aipd->aipadpos.y + 50));

	double aipdwidth = aipd->aiImage->WidthGet() / 4;
	double aipdheight = aipd->aiImage->HeightGet();

	if (circleDistancex > (aipdwidth + b->image->WidthGet() / 2)) { return false; }
	if (circleDistancey > (aipdheight / 2 + b->image->HeightGet() / 2)) { return false; }

	if (circleDistancex = (aipdwidth)) { return true; }
	if (circleDistancey = (aipdheight)) { return true; }

	double radius = aipd->aiImage->WidthGet() / 2;
	double cornerDistance_sq = std::pow(circleDistancex - aipdwidth, 2) + std::pow(circleDistancey - aipdheight, 2);

	return (cornerDistance_sq >= (std::pow(radius, 2)));
}

//-----------------------------------------------------------------------------

void MainGame::Draw()
{
	score->Draw();
	Userpaddle->Draw();
	AIpaddle->Draw();
	ball->Draw();
}

//==============================================================================

UserPaddle::UserPaddle()
{
	paddleImage = NULL;
}

AIPaddle::AIPaddle()
{
	aiImage = NULL;
	ball = NULL;
}

Ball::Ball()
{
   image = NULL;
}

Score::Score()
{
	userScoreImage = NULL;
	aiScoreImage = NULL;
	resultImage = NULL;
}

/*FireWork::FireWork()
{
	fireworkImage = NULL;
}*/

void UserPaddle::Init()
{
	padpos.x = theScreen->WidthGet() - theScreen->WidthGet();
	padpos.y = theScreen->HeightGet() / 2.0f;
	paddleImage = theImages->Load("UserPaddle");
}

void AIPaddle::Init()
{
	aiImage = theImages->Load("AIPaddle");
	aipadpos.x = theScreen->WidthGet() - aiImage->WidthGet();
	aipadpos.y = theScreen->HeightGet() / 2.0f;
	randVel = 1;
	paddleVelocity[0] = 0.5;
	paddleVelocity[1] = 0.75;
	paddleVelocity[2] = 1.1;
	paddleVelocity[3] = 1.25;
	paddleVelocity[4] = 1.5;
}

//------------------------------------------------------------------------------

void Ball::Init()
{
   // Load the image of the ball.
   image = theImages->Load("Ball");

   // Start the ball in the middle of the screen.
   position = Point2F::Create(theScreen->SizeGet() / 2);

   // The ball will initially be moving down and to the right.
	velocity.Set(0, 0);
}

void Score::Init()
{
	userScore = 0;
	aiScore = 0;

	userScoreImage = theImages->Load("pongScore0");
	aiScoreImage = theImages->Load("pongScore0");
	resultImage = theImages->Load("blank");

	userPosition = Point2F::Create(theScreen->WidthGet() / 4, theScreen->HeightGet() / 10);
	aiPosition = Point2F::Create(theScreen->WidthGet() - (theScreen->WidthGet() / 4), theScreen->HeightGet() / 10);
	resultPosition = Point2F::Create(theScreen->SizeGet() / 4);
}

/*void FireWork::Init()
{
	imageCount = 0;
	fireworkImage = NULL;
	fworkPosition = Point2F::Create(theScreen->WidthGet() / 5, theScreen->HeightGet() / 7);
	fworkPosition2 = Point2F::Create(theScreen->WidthGet() - 200.0f, theScreen->HeightGet() / 7);
}*/

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

void AIPaddle::Deinit()
{
	if (aiImage)
	{
		theImages->Unload(aiImage);
		aiImage = NULL;
	}
}

void Ball::Deinit()
{
   // Unload the image of the ball.
   if(image)
   {
      theImages->Unload(image);
      image = NULL;
   }
}

void Score::Deinit()
{
	// Unload the image of the score.
	if (userScoreImage)
	{
		theImages->Unload(userScoreImage);
		userScoreImage = NULL;
	}
	if (aiScoreImage)
	{
		theImages->Unload(aiScoreImage);
		aiScoreImage = NULL;
	}
	if (resultImage)
	{
		theImages->Unload(resultImage);
		resultImage = NULL;
	}
}
/*void FireWork::Deinit()
{
	if (fireworkImage)
	{
		theImages->Unload(fireworkImage);
		fireworkImage = NULL;
	}
}*/

//------------------------------------------------------------------------------
void UserPaddle::Update(unsigned int dt)
{
	if (theKeyboard->KeyPressed(KEY_UP))
	{
		
		if (padpos.y <= 5)
		{
			
		}
		else
		{
			padpos.y -= 10.0f;
		}
	}
	if (theKeyboard->KeyPressed(KEY_DOWN))
	{
		if (padpos.y > theScreen->HeightGet() - paddleImage->HeightGet())
		{
			
		}
		else
		{
			padpos.y += 10.0f;
		}
	}
	
}

void AIPaddle::Update(unsigned int dt)
{
	
}

void Ball::Update(unsigned int dt)
{
	// Get a floating point number for the duration of the last frame in seconds.
	float dtSeconds = (float)dt / 1000.0f;

	// Update the position of the ball.
	position += velocity * dtSeconds;
   
	
}

void Score::Update()
{
	if (userScore == 0)
	{
		userScoreImage = theImages->Load("pongScore0");
	}
	if (userScore == 1)
	{
		userScoreImage = theImages->Load("pongScore1");
	}
	if (userScore == 2)
	{
		userScoreImage = theImages->Load("pongScore2");
	}
	if (userScore == 3)
	{
		userScoreImage = theImages->Load("pongScore3");
		resultImage = theImages->Load("youWin");
	}
	if (aiScore == 0)
	{
		aiScoreImage = theImages->Load("pongScore0");
	}
	if (aiScore == 1)
	{
		aiScoreImage = theImages->Load("pongScore1");
	}
	if (aiScore == 2)
	{
		aiScoreImage = theImages->Load("pongScore2");
	}
	if (aiScore == 3)
	{
		aiScoreImage = theImages->Load("pongScore3");
		resultImage = theImages->Load("youLose");
	}
}

/*void FireWork::Update()
{

}*/
//------------------------------------------------------------------------------

void Ball::Draw()
{
   // The center of the ball is in the center of the image, so use an offset.
   image->Draw(position - (Point2F::Create(image->SizeGet()) / 2.0f));
}

void UserPaddle::Draw()
{
	paddleImage->Draw(padpos);
}

void AIPaddle::Draw()
{
	aiImage->Draw(aipadpos);
}

void Score::Draw()
{
	userScoreImage->Draw(userPosition);
	aiScoreImage->Draw(aiPosition);
	resultImage->Draw(resultPosition);
}
//------------------------------------------------------------------------------