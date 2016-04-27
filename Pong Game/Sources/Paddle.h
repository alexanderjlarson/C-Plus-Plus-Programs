#ifndef __PADDLE_H__
#define __PADDLE_H__

#include "Frog.h"
#include "MainGame.h"

namespace Webfoot {

	class Paddle
	{
	public:

		Paddle();
		void Init();
		void Deinit();
		void Update(unsigned int dt);
		void Draw();

		
	protected:
		
		Image* aipaddleImage;
		Image* paddleImage;
		Point2F padpos;
	};

}
#endif