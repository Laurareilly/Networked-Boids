#include "NetworkManager.h"
#include "ApplicationState.h"

ApplicationState::ApplicationState()
{
	 mpNetworkManager = new NetworkManager();
}

ApplicationState::~ApplicationState()
{
	delete mpNetworkManager; mpNetworkManager = nullptr;
}
