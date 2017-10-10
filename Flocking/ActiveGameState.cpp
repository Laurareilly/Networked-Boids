#include "ActiveGameState.h"
#include "UnitManager.h"
#include "Game.h"
#include "HomeScreen.h"
#include "ApplicationState.h"
#include "NetworkManager.h"

ActiveGameState::ActiveGameState()
{
	data->headerMessage = "";
	data->doesSendData = 0;
	data->doesDisplay = 1;
	data->doesUpdateInput = 1;
	data->doesUpdateState = 1;
	data->doesUpdateNetworking = 1;
}

void ActiveGameState::UpdateState()
{
}

void ActiveGameState::UpdateInput()
{
}

void ActiveGameState::UpdateNetworking()
{
}

void ActiveGameState::Display()
{
}

void ActiveGameState::GoToNextState(ApplicationState * passData)
{
	gpGame->theState = gpGame->theHomeScreen;
	next = gpGame->theState;
	next->OnArriveFromPrevious(passData);
}
