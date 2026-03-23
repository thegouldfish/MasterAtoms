#ifndef SIMPLE_STATE
#define SIMPLE_STATE


typedef void _voidCallBack();

typedef struct
{
	_voidCallBack *Start;

	_voidCallBack *Update;

	_voidCallBack *End;

} SimpleState;



typedef struct
{
	const SimpleState* CurrentState;
	const SimpleState* ChangeTo;

} StateMachine;



void StateMachineStart(StateMachine* machine, const SimpleState* state);
void StateMachineChange(StateMachine* machine, const SimpleState* state);
void StateMachineUpdate(StateMachine* machine) __z88dk_fastcall;
#endif