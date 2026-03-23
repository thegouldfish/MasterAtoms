#include "SimpleState.h"


void StateMachineStart(StateMachine* machine, const SimpleState* state)
{
	if (state != 0 && machine != 0)
	{
		machine->ChangeTo = 0;
		machine->CurrentState = state;
		machine->CurrentState->Start();
	}
}


void StateMachineChange(StateMachine* machine, const SimpleState* state)
{
	if (state != 0 && machine != 0 )
	{		
		machine->ChangeTo = state;
	}
}


void StateMachineUpdate(StateMachine* machine) __z88dk_fastcall
{
	if (machine != 0 )
	{
		if (machine->ChangeTo != 0 )
		{
			machine->CurrentState->End();
			
			
			machine->CurrentState = machine->ChangeTo;
			machine->CurrentState->Start();
			
			machine->ChangeTo = 0;
		}
		else
		{
			machine->CurrentState->Update();
		}
	}
}