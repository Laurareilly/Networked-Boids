#include "ActiveGameState.h"
#include "UnitManager.h"
#include "Game.h"
#include "HomeScreen.h"

ActiveGameState::ActiveGameState()
{
	data->headerMessage = "";
	data->doesSendData = 0;
	data->doesDisplay = 1;
	data->doesUpdateInput = 1;
	data->doesUpdateState = 1;
	data->doesUpdateNetworking = 1;
}

void ActiveGameState::GoToNextState(ApplicationState * passData)
{
	gpGame->theState = gpGame->theHomeScreen;
	next = gpGame->theState;
	next->OnArriveFromPrevious(passData);
}
