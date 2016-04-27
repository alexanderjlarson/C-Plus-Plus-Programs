#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "MenuState.h"
#include "FileReader.h"
#include <string> 
using namespace std;
//#include "Paddle.h"

namespace Webfoot {

class Ball;
class UserPaddle;
class Brick;
class PowerUp;
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
   bool intersects2(UserPaddle * pd, Ball * b);
   bool intersects3(UserPaddle * pd, Ball * b);

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
   PowerUp* Power;
   UserPaddle* Userpaddle;
   Brick* brick;
   FileReader* fileReader;
   bool gamePaused;
   bool allowCollision;
   bool newGame;
   bool allowVelChange;
   unsigned int collisionAtTick;
   unsigned int powerTime;
   int score;
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

class PowerUp
{
	friend class MainGame;
public:
	PowerUp();
	void Init();
	void DeInit();
	void Update();
	void Draw();
protected:
	Image* powerImage;
	Image* powerImage2;
	Image* powerImage3;
	Image* powerImage4;
	Image* bulletImage;
	Point2F powerpos;
	Point2F powerpos2;
	Point2F powerpos3;
	Point2F powerpos4;
	Point2F bulletpos;
	int powerRand;
	bool powerup;
	int powerChoice;
	bool allowShoot;
	bool powImg1;
	bool powImg2;
	bool powImg3;
	bool powImg4;
	bool powerCollision;
	bool bulletImg;
};

class Brick
{
	friend class MainGame;
	friend class FileReader;
public:
	Brick();
	void Init();
	void Deinit();
	void Update();
	void Draw();
protected:
	Image* brickImage;
	FileReader* fileReader;
	Point2F brickpos;
	int bricknum[30][30];
	int bricknum2[30][30];
	int bricknum3[30][30];
	int bricknum4[30][30];
	int brickGridX[30][30];
	int brickGridY[30];
	int brickGridX2[30][30];
	int brickGridY2[30];
	int brickGridX3[30][30];
	int brickGridY3[30];
	int brickGridX4[30][30];
	int brickGridY4[30];
	bool level1;
	bool level2;
	bool level3;
	bool level4;
	
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
   Image* image2;
   Image* image3;
   /// Current position of the ball.
   Point2F position;
   Point2F position2;
   Point2F position3;
   /// Current velocity of the ball.
   Point2F velocity;
   Point2F velocity2;
   Point2F velocity3;
   bool positive;
   bool positive2;
   bool positive3;
   bool moveball;
   bool ballAlive;
   bool ballAlive2;
};

//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MAINGAME_H__
