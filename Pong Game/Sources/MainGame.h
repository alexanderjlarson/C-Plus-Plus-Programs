#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "MenuState.h"
#include <string> 
using namespace std;
//#include "Paddle.h"

namespace Webfoot {

class Ball;
class AIPaddle;
class UserPaddle;
class Score;
class FireWork;
//==============================================================================

class MainGame : public MenuState
{
public:
   typedef MenuState Inherited;

   MainGame();
   virtual ~MainGame() {};
   
   virtual void Init();
   virtual void Deinit();

   bool intersects(UserPaddle * pd, Ball * b);
   
   bool aiintersects(AIPaddle * aipd, Ball * b);

   /// Call this on every frame to update the positions.
   virtual void Update();
   /// Call this on every frame to draw the images.
   virtual void Draw();
   ///virtual void Process_Input();

   static MainGame instance;

protected:
   /// Returns the name of the GUI layer
   virtual const char* GUILayerNameGet();

   /// The ball that bounces around the screen.
   Ball* ball;
   AIPaddle* AIpaddle;
   UserPaddle* Userpaddle;
   Score* score;
   FireWork* firework;
   bool gamePaused;
   bool allowCollision;
   bool newGame;
   bool allowVelChange;
   unsigned int collisionAtTick;
};

MainGame* const theMainGame = &MainGame::instance;

//==============================================================================
class UserPaddle
{
	friend class MainGame;
public:
	UserPaddle();
	void Init();
	void Deinit();
	void Update(unsigned int dt);
	void Draw();

protected:
	Image* paddleImage;
	Point2F padpos;
};

class AIPaddle
{
	friend class MainGame;
public:
	AIPaddle();
	void Init();
	void Deinit();
	void Update(unsigned int dt);
	void Draw();

protected:
	Image* aiImage;
	Point2F aipadpos;
	Ball* ball;
	int paddleVelocity[5];
	int randVel;
};
/// A bouncing ball
class Ball
{
	friend class MainGame;
	friend class AIPaddle;
public:
   Ball();
   
   /// Initialize the ball
   void Init();
   /// Clean up the ball
   void Deinit();

   /// Make any changes for the given frame.  'dt' is the amount of time that
   /// has passed since the last frame, in milliseconds.
   void Update(unsigned int dt);
   /// Draw the ball.
   void Draw();

protected:
	//UserPaddle upad;
   /// Appearance of the ball.
   Image* image;
   /// Current position of the ball.
   Point2F position;
   /// Current velocity of the ball.
   Point2F velocity;
};

//==============================================================================

class Score
{
	friend class MainGame;
public:
	Score();
	void Init();
	void Deinit();
	void Update();
	void Draw();

protected:

	Image* userScoreImage;
	Image* aiScoreImage;
	Image* resultImage;
	Point2F userPosition;
	Point2F aiPosition;
	Point2F resultPosition;
	int userScore, aiScore;
};

class FireWork
{
	friend class MainGame;
public:
	FireWork();
	void Init();
	void Deinit();
	void Update();
	void Draw();

protected:
	Image* fireworkImage;
	Point2F fworkPosition;
	Point2F fworkPosition2;
	int imageCount;
};

//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MAINGAME_H__
