#ifndef MISC_INPUTCOMMAND_H
#define MISC_INPUTCOMMAND_H

// command pattern struct that represents a single input action
// produced this frame. Vehicle can consume these instead of
// talking to the raw PAD API.

enum class InputCommandType
{
	Accelerate,
	Brake,
	Steer,
	StartPressed,
	ResetRequested,
	ControllerDisconnected
};

struct InputCommand
{
	InputCommandType type;
	int   playerIndex; // which player this command belongs to
	float value;       // amount for analog stuff like accel,steer
};

#endif


