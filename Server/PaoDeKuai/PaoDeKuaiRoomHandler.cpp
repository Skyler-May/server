// PaoDeKuaiRoomHandler.cpp

#include "PaoDeKuaiRoomHandler.h"
#include "PaoDeKuaiMessages.h"

#include "../GameDefines.h"

namespace NiuMa
{
	PaoDeKuaiRoomHandler::PaoDeKuaiRoomHandler(const MessageQueue::Ptr& queue)
		: GameRoomHandler(queue)
	{
		// 添加接收的消息类型
		addMessage(MsgPaoDeKuaiSync::TYPE);
		addMessage(MsgPaoDeKuaiPlayCard::TYPE);

		// 添加游戏类型
		addGameType(static_cast<int>(GameType::PaoDeKuai));
	}

	PaoDeKuaiRoomHandler::~PaoDeKuaiRoomHandler() {}
}
