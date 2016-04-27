#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "MenuState.h"
#include "FileReader.h"

namespace Webfoot {
//==============================================================================

class MainGame : public MenuState
{
public:
   typedef MenuState Inherited;

   MainGame();
   virtual ~MainGame() {};
   
   virtual void Init();
   virtual void Deinit();

   /// Call this on every frame to update the positions.
   virtual void Update();
   /// Call this on every frame to draw the images.
   virtual void Draw();

   static MainGame instance;

protected:
	FileReader* fileReader;
   /// Returns the name of the GUI layer
   virtual const char* GUILayerNameGet();
};

MainGame* const theMainGame = &MainGame::instance;

//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MAINGAME_H__
