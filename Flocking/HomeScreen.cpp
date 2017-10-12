#include <RakNet/WindowsIncludes.h>

#include "HomeScreen.h"
#include "UnitManager.h"
#include "Game.h"
#include "InputManager.h"
#include "ActiveGameState.h"
#include "NetworkManager.h"

HomeScreen::HomeScreen()
{
	data->headerMessage[0] = "Welcome 2 boids! Choose Option:";
	data->headerMessage[1] = " 1: Local Boids";
	data->headerMessage[2] = " 2: Join Networked Boids";
	data->headerMessage[3] = " 3: Create Networked Boids (Data Push)";
	data->headerMessage[4] = " 4: Create Networked Boids (Data Share)";
	data->headerMessage[5] = " 5: Create Networked Boids (Data Couple)";
	data->headerMessage[6] = " 6: Quit";
	data->doesSendData = 0;
	data->doesDisplay = 1;
	data->doesUpdateInput = 1;
	data->doesUpdateState = 1;
	data->doesUpdateNetworking = 0;
}

void HomeScreen::UpdateState()
{
	if (data->doesUpdateState == 0)
	{
		data->doesUpdateState = 1;
		return;
	}

	if (data->enterServer)
	{
		data->enterServer = false;
		GoToNextState(this);
	}

	if (tryingToConnect)
	{
		if (successfullyConnectedToServer)
		{
			waitFrames = 30;
			GoToNextState(this);
		}
		else
		{
			--waitFrames;
			if (waitFrames <= 0)
			{
				waitFrames = 30;
				tryingToConnect = false;

				//latewr pass an error mesage that fails to connect
			}
		}
	}
	else
	{
		switch (selectedOptionIndex)
		{
		case 1:
			data->isLocal = 1;
			GoToNextState(this);
			break;
		case 2:
			mpNetworkManager->initClient(data->portNumber, data->ipAddress);
			data->isLocal = 0;
			tryingToConnect = true;
			break;
		case 3:
			mpNetworkManager->initServer(data->portNumber);
			data->isLocal = 0;
			tryingToConnect = true;
			wantsToBeSever = true;
			break;
		case 4:
			mpNetworkManager->initServer(data->portNumber);
			data->isLocal = 0;
			tryingToConnect = true;
			wantsToBeSever = true;
			break;
		case 5:
			mpNetworkManager->initServer(data->portNumber);
			data->isLocal = 0;
			tryingToConnect = true;
			wantsToBeSever = true;
			break;
		case 6:
			gpGame->exitGame();
			break;
		default:
			break;
		}

		selectedOptionIndex = 0;
	}
}

void HomeScreen::UpdateInput()
{
	gpGame->getInputManager()->update();
	if (gpGame->getInputManager()->getPressed(InputManager::KeyCode::N1))
	{
		selectedOptionIndex = 1;
	}
	if (gpGame->getInputManager()->getPressed(InputManager::KeyCode::N2))
	{
		selectedOptionIndex = 2;
	}
	if (gpGame->getInputManager()->getPressed(InputManager::KeyCode::N3))
	{
		selectedOptionIndex = 3;
	}
	if (gpGame->getInputManager()->getPressed(InputManager::KeyCode::N4))
	{
		selectedOptionIndex = 4;
	}
	if (gpGame->getInputManager()->getPressed(InputManager::KeyCode::N5))
	{
		selectedOptionIndex = 5;
	}
	if (gpGame->getInputManager()->getPressed(InputManager::KeyCode::N6))
	{
		selectedOptionIndex = 6;
	}

	if (gpGame->getInputManager()->getPressed(InputManager::KeyCode::ESCAPE))
	{
		gpGame->exitGame();
	}
}

void HomeScreen::UpdateNetworking()
{
	if (!data->doesUpdateNetworking)
		return;

	mpNetworkManager->Update();
}

void HomeScreen::Display()
{
	gpGame->render();
}

void HomeScreen::GoToNextState(ApplicationState * passData)
{
	gpGame->theState = gpGame->theGameState;
	next = gpGame->theState;
	next->OnArriveFromPrevious(passData);
}
