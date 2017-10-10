#pragma once
#include "ApplicationState.h"

class HomeScreen : public ApplicationState
{
public:
	HomeScreen();
	~HomeScreen() {};

	virtual void UpdateState();
	virtual void UpdateInput();
	virtual void UpdateNetworking();
	virtual void Display();

	virtual void ForcePlayerToLobby() {};
	virtual int getIsLocal() { return data->isLocal; }
	virtual void SetClientID(int cID) {};

	virtual void ReceiveBoidData() {};

	virtual void SyncBoidList() {};
	virtual void SendBoidData() {};
	virtual void AcceptedToServer() { data->enterServer = 1; };

	bool isServer;

	virtual void OnArriveFromPrevious(ApplicationState *passData)
	{
		data->headerMessage = "Welcome 2 boidsz\nChoose Option\n1: Local Boids\n2: Join Networked Boids\n3: Create Networked Boids (Data Push)\n4: Create Networked Boids (Data Share)\n5: Create Networked Boids (Data Couple)\n6: Quit\n";
		data->clientID = 0;
		data->ipAddress = passData->data->ipAddress;
		data->isLocal = passData->data->isLocal;
		data->portNumber = passData->data->portNumber;
		data->doesDisplay = 1;
		data->doesUpdateInput = 1;
		data->doesUpdateState = 1;
		data->doesUpdateNetworking = 1;
		data->doesSendData = 0;
	}

	virtual void GoToNextState(ApplicationState *passData);

private:

	bool wantsToBeSever = false;
	bool successfullyConnectedToServer = false;
	int waitFrames = 30;
	int selectedOptionIndex = -1;
	bool tryingToConnect;
};