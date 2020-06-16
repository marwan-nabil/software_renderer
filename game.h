#ifndef GAME_H
#define GAME_H

#define GAME_UPDATE_STATE_MACRO(name) void name(void)
typedef GAME_UPDATE_STATE_MACRO(update_game_state_func_t);

#define GAME_GATHER_INPUT_MACRO(name) void name(void)
typedef GAME_GATHER_INPUT_MACRO(gather_game_input_func_t);

typedef struct
{
	update_game_state_func_t *UpdateGameState;
	gather_game_input_func_t *GatherGameInput;
} game;

typedef struct
{
	bool IsDown;
	bool WasDown;
} button_state;

typedef struct
{
	button_state Up;
	button_state Down;
	button_state Left;
	button_state Right;
} game_input;


GAME_UPDATE_STATE_MACRO(UpdateGameState);
GAME_GATHER_INPUT_MACRO(GatherGameInput);

#endif