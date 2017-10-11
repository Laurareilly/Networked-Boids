#pragma once

#include "ApplicationState.h"

class ActiveGameState : public ApplicationState
{
public:
	ActiveGameState();
	~ActiveGameState() {};

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
	virtual void AcceptedToServer() {};

	bool isServer;

	virtual void OnArriveFromPrevious(ApplicationState *passData)
	{
		data->headerMessage = "";
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

};