// NiuNiuRoomHandler.cpp

#include "NiuNiuRoomHandler.h"
#include "NiuNiuMessages.h"

#include "../GameDefines.h"

namespace NiuMa
{
	NiuNiuRoomHandler::NiuNiuRoomHandler(const MessageQueue::Ptr& queue)
		: GameRoomHandler(queue)
	{
		addMessage(MsgNiuNiuSync::TYPE);
		addGameType(static_cast<int>(GameType::NiuNiu));
	}

	NiuNiuRoomHandler::~NiuNiuRoomHandler() {}
}
