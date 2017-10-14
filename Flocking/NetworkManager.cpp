#include "NetworkManager.h"
#include "UnitManager.h"
#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "GraphicsSystem.h"
#include "ApplicationState.h"

void NetworkManager::SendBoidData(std::map<UnitID, Unit*> units) //(Unit *units[30]) //take in list of boids as param
{
	//printf("send boid data is being called");
	char sendBuff[2048];
	unsigned int bytesWritten = 0;
	sendBuff[0] = ID_BOID_DATA;
	++bytesWritten;

	//for loop go thorugh each boid 
	//for (unsigned int i = 0; i < units.size(); ++i)
	for(auto it = units.begin(); it != units.end(); ++it)
	{
		if (it->second == nullptr)
		{
			printf("the unit is null");
			break;
		}

		data[0].boidID = it->second->getID();
		data[0].posX = it->second->getPositionComponent()->getPosition().getX();
		data[0].posY = it->second->getPositionComponent()->getPosition().getY();
		data[0].rotation = it->second->getPositionComponent()->getFacing();
		data[0].velX = it->second->getPhysicsComponent()->getVelocity().getX();
		data[0].velY = it->second->getPhysicsComponent()->getVelocity().getY();

		bytesWritten += Write(sendBuff + bytesWritten);
		//printf("bytes written: %i\n", bytesWritten);
	}


	//send it
	//mpPeer->Send(sendBuff, bytesWritten, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true); //Sends to everyone except for unassigned system address (USA) so we'll only send it to the client

	//mpPeer->Send(sendBuff, bytesWritten, HIGH_PRIORITY, RELIABLE_ORDERED, 0, mpPeer->GetSystemAddressFromIndex(0), false); //Sends to everyone except for unassigned system address (USA) so we'll only send it to the client
}

unsigned int NetworkManager::Write(char *buffer)
{
	const char *const start = buffer; //starting pos
	/*memcpy(buffer, data, sizeof(data));
	buffer += sizeof(data);*/
	memcpy(buffer, &data[0].boidID, sizeof(char));
	buffer += sizeof(char);
	memcpy(buffer, &data[0].posX, sizeof(float));
	buffer += sizeof(float);
	memcpy(buffer, &data[0].posY, sizeof(float));
	buffer += sizeof(float);
	memcpy(buffer, &data[0].velX, sizeof(float));
	buffer += sizeof(float);
	memcpy(buffer, &data[0].velY, sizeof(float));
	buffer += sizeof(float);
	memcpy(buffer, &data[0].rotation, sizeof(float));
	buffer += sizeof(float);
	return buffer - start;
}

unsigned int NetworkManager::Read(char * buffer)
{
	unsigned int totalData = 0;
	sscanf(buffer + totalData, "%c", &data[0].boidID);
	totalData += sizeof(char);
	sscanf(buffer + totalData, "%f", &data[0].posX);
	totalData += sizeof(float);
	sscanf(buffer + totalData, "%f", &data[0].posY);
	totalData += sizeof(float);
	sscanf(buffer + totalData, "%f", &data[0].velX);
	totalData += sizeof(float);
	sscanf(buffer + totalData, "%f", &data[0].velY);
	totalData += sizeof(float);
	sscanf(buffer + totalData, "%f", &data[0].rotation);
	totalData += sizeof(float);

	return totalData;
}

NetworkManager::NetworkManager()
{
	mpPeer = RakPeerInterface::GetInstance();
}

NetworkManager::~NetworkManager()
{
	RakPeerInterface::DestroyInstance(mpPeer);
}

void NetworkManager::initServer(int cPort)
{
	maxClients = 1;
	serverPort = cPort;
	sd = new SocketDescriptor(serverPort, 0);
	mpPeer->SetMaximumIncomingConnections(maxClients);
	//mpPeer->SetTimeoutTime(1000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	//mpPeer->SetOccasionalPing(true);
	//mpPeer->SetUnreliableTimeout(1000);
	mpPeer->Startup(maxClients, sd, 1);
	mIsServer = true;
}

void NetworkManager::initClient(int cPort, char * cIP)
{
	mIsServer = false;
	sd = new SocketDescriptor();
	mpPeer->Startup(1, sd, 1);
	//mpPeer->SetOccasionalPing(true);

	if (!mpPeer)
	{
		printf("Peer does not exist");
	}

	serverPort = cPort;
    mpPeer->Connect(cIP, serverPort, 0, 0);
}

void NetworkManager::Update()
{
	mpPeer;
	//std::cout << "networkmanger update" << std::endl;
	for (mpPacket = mpPeer->Receive(); mpPacket; mpPeer->DeallocatePacket(mpPacket), mpPacket = mpPeer->Receive())
	{
		switch (mpPacket->data[0])
		{

		std::cout << mpPacket->data[0] << std::endl;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Client disconnects");
			gpGame->theState->ForcePlayerToLobby();
			mpPeer->CloseConnection(mpPacket->systemAddress, true);
			break;
		case ID_REMOTE_CONNECTION_LOST:
			gpGame->theState->ForcePlayerToLobby();
			mpPeer->CloseConnection(mpPacket->systemAddress, true);
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A client has connected.\n");
			break;
		case ID_NEW_CLIENT_JOIN:
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED: //the client receives this
		{
			printf("Our connection request has been accepted.\n");
			gpGame->theState->AcceptedToServer();

			/*char sendBuff[2048];
			unsigned int bytesWritten = 0;
			sendBuff[0] = TEST;
			bytesWritten++;

			mpPeer->Send(sendBuff, bytesWritten, HIGH_PRIORITY, RELIABLE_ORDERED, 0, mpPacket->systemAddress, false);*/
			//mpPeer->Send(sendBuff, bytesWritten, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

			UsernameMessage username[1] = { ID_USERNAME,  "", "hello" };
			username[0].messageID;
		
			mpPeer->Send((char*)username, sizeof(username), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mpPacket->systemAddress, false);
			mpPeer->Send((char*)username, sizeof(username), HIGH_PRIORITY, RELIABLE_ORDERED, 0, mpPacket->systemAddress, true);
		}
			break;
		case ID_CLIENT_NUMBER:
			break;
		case ID_CLIENT_TO_SERVER:
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			printf("Disconnection");
			gpGame->theState->ForcePlayerToLobby();
			mpPeer->CloseConnection(mpPacket->systemAddress, true);
			break;
		case ID_CONNECTION_LOST:
			gpGame->theState->ForcePlayerToLobby();
			break;
		case ID_PEER_LEAVE:
			gpGame->theState->ForcePlayerToLobby();
			mpPeer->CloseConnection(mpPacket->systemAddress, true);
			break;
		case ID_BOID_DATA:
		{
			std::cout << "id boids data" << std::endl;
			int position = 1;

			gpGame->getUnitManager()->updateAll(true); //hehe

			while (mpPacket->data[position] != NULL)
			{
				position += Read((char*)mpPacket->data + position);

				mTmpUnit = gpGame->getUnitManager()->getUnit(data[0].boidID);

				if (mTmpUnit)
				{
					mTmpUnit->setID(data[0].boidID);
					mTmpUnit->getPositionComponent()->setPosition(Vector2D(data[0].posX, data[0].posY));
					mTmpUnit->getPositionComponent()->setFacing(data[0].rotation);
					mTmpUnit->getPhysicsComponent()->setVelocity(Vector2D(data[0].velX, data[0].velY));
					mTmpUnit->setShouldBeDeleted(false);
				}
				else
				{
					//add boid
					UnitManager::mBoidsOnScreen++;
					Unit* pUnit = gpGame->getUnitManager()->createUnit(*gpGame->getSpriteManager()->getSprite(AI_ICON_SPRITE_ID), true, PositionData(Vector2D(rand() % gpGame->getGraphicsSystem()->getWidth(), rand() % gpGame->getGraphicsSystem()->getHeight()), 0.0f));
					pUnit->setID(data[0].boidID);
					pUnit->getPositionComponent()->setFacing(data[0].rotation);
					pUnit->getPositionComponent()->setPosition(Vector2D(data[0].posX, data[0].posY));
					pUnit->getPhysicsComponent()->setVelocity(Vector2D(data[0].velX, data[0].velY));
					pUnit->setShouldBeDeleted(false);
					pUnit->setSteering(Steering::FLOCKING, ZERO_VECTOR2D, PLAYER_UNIT_ID);
				}
			}

			gpGame->getUnitManager()->deleteIfShouldBeDeleted();
		}
		break;
		case ID_USERNAME:
		{
			std::cout << "test" << std::endl;
		}
		break;
		default:
		{
			std::cout << "receiving id: " << mpPacket->data[0] << std::endl;
		}
		break;
		}
	}
}
