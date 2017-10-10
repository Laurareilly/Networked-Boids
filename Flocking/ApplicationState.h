#pragma once

#include <string>
#include "NetworkManager.h"

class ApplicationState abstract
{
public:
	friend class HomeScreen;
	friend class ActiveGameState;

	virtual void UpdateInput() = 0;
	virtual void UpdateNetworking() = 0;
	virtual void UpdateState() = 0;
	virtual void Display() = 0;

	virtual void ForcePlayerToLobby() = 0;
	virtual void ReceiveBoidData() = 0;

	virtual void SyncBoidList() = 0;
	virtual void SendBoidData() = 0;

	virtual int getIsLocal() = 0;
	virtual void AcceptedToServer() = 0;

	ApplicationState() { mpNetworkManager = new NetworkManager(); };
	~ApplicationState() { delete mpNetworkManager; mpNetworkManager = nullptr; };

protected:
	ApplicationState *next, *previous;

	struct ApplicationStateData
	{
		//SystemAddress peerSystemAddress;
		unsigned int isLocal = 1; //0 = networked
		unsigned int clientID = 1;
		std::string headerMessage;
		char doesUpdateInput;
		char doesUpdateNetworking;
		char doesUpdateState;
		char doesDisplay;
		char doesSendData;
		int portNumber = 5546;
		char *ipAddress = "127.0.0.1";

		char enterServer = 0;
	}data[1];

	virtual void GoToNextState(ApplicationState *passData) = 0;
	virtual void OnArriveFromPrevious(ApplicationState *passData) = 0;

	NetworkManager *mpNetworkManager;
};