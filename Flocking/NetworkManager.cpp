#include "NetworkManager.h"
#include "UnitManager.h"
#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "GraphicsSystem.h"
#include "ApplicationState.h"


void NetworkManager::SendBoidData(Unit *units[30]) //take in list of boids as param
{
	char sendBuff[2048];
	unsigned int bytesWritten = 0;
	sendBuff[0] = ID_BOID_DATA;
	++bytesWritten;

	//for loop go thorugh each boid 
	for (unsigned int i = 0; i < 30; ++i)
	{
		if (units[i] == nullptr)
		{
			break;
		}

		data[0].boidID = units[i]->getID();
		data[0].posX = units[i]->getPositionComponent()->getPosition().getX();
		data[0].posY = units[i]->getPositionComponent()->getPosition().getY();
		data[0].rotation = units[i]->getPositionComponent()->getFacing();
		data[0].velX = units[i]->getPhysicsComponent()->getVelocity().getX();
		data[0].velY = units[i]->getPhysicsComponent()->getVelocity().getY();

		bytesWritten += Write(sendBuff + bytesWritten);
	}


	//send it
	mpPeer->Send(sendBuff, sizeof(sendBuff), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true); //Sends to everyone except for unassigned system address (USA) so we'll only send it to the client
}

unsigned int NetworkManager::Write(char *buffer)
{
	const char *const start = buffer; //starting pos
	memcpy(buffer, data, sizeof(data));
	buffer += sizeof(data);
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
	mpPeer->SetTimeoutTime(1000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	mpPeer->SetOccasionalPing(true);
	mpPeer->SetUnreliableTimeout(1000);
	mpPeer->Startup(maxClients, sd, 1);
}

void NetworkManager::initClient(int cPort, char * cIP)
{
	mIsServer = false;
	sd = new SocketDescriptor;
	mpPeer->Startup(1, sd, 1);
	mpPeer->SetOccasionalPing(true);

	if (!mpPeer)
	{
		printf("Peer does not exist");
	}

	serverPort = cPort;
	mpPeer->Connect(cIP, serverPort, 0, 0);
}

void NetworkManager::Update()
{
	for (mpPacket = mpPeer->Receive(); mpPacket; mpPeer->DeallocatePacket(mpPacket), mpPacket = mpPeer->Receive())
	{
		switch (mpPacket->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			gpGame->theState->ForcePlayerToLobby();
			mpPeer->CloseConnection(mpPacket->systemAddress, true);
			break;
		case ID_REMOTE_CONNECTION_LOST:
			gpGame->theState->ForcePlayerToLobby();
			mpPeer->CloseConnection(mpPacket->systemAddress, true);
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			//printf("Another client has connected.\n");
			break;
		case ID_NEW_INCOMING_CONNECTION:
			//printf("A client has connected.\n");
			break;
		case ID_NEW_CLIENT_JOIN:
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED: //the client receives this
			printf("Our connection request has been accepted.\n");
			gpGame->theState->AcceptedToServer();
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
		}
	}
}
