// LandlordRoomHandler.cpp

#include "LandlordRoomHandler.h"
#include "LandlordMessages.h"

#include "../GameDefines.h"

namespace NiuMa
{
	LandlordRoomHandler::LandlordRoomHandler(const MessageQueue::Ptr& queue)
		: GameRoomHandler(queue)
	{
		// 添加接收的消息类型
		addMessage(MsgLandlordSync::TYPE);
		addMessage(MsgLandlordPlayCard::TYPE);

		// 添加游戏类型
		addGameType(static_cast<int>(GameType::Landlord));
	}

	LandlordRoomHandler::~LandlordRoomHandler() {}
}
