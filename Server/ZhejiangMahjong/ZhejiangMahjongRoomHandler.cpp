// ZhejiangMahjongRoomHandler.cpp

#include "ZhejiangMahjongRoomHandler.h"
#include "ZhejiangMahjongMessages.h"

#include "../GameDefines.h"

namespace NiuMa
{
	ZhejiangMahjongRoomHandler::ZhejiangMahjongRoomHandler(const MessageQueue::Ptr& queue)
		: GameRoomHandler(queue)
	{
		addMessage(MsgZhejiangMahjongSync::TYPE);
		addMessage(MsgZhejiangMahjongDiscard::TYPE);
		addMessage(MsgZhejiangMahjongActionResp::TYPE);
		addGameType(static_cast<int>(GameType::ZhejiangMahjong));
	}

	ZhejiangMahjongRoomHandler::~ZhejiangMahjongRoomHandler() {}
}
