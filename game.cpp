#include <windows.h>
#include "game.h"


GAME_UPDATE_STATE_MACRO(UpdateGameState)
{

}

GAME_GATHER_INPUT_MACRO(GatherGameInput)
{

}

game *
GameInit()
{
	game *Game = VirtualAlloc(0, sizeof(game), MEM_RESERVE | MEM_COMMIT, 
							  PAGE_READWRITE);
	Game->UpdateGameState = &UpdateGameState;
	Game->GatherGameInput = &GatherGameInput;
	return Game;
}