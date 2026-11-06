// ZhejiangMahjongRoomHandler.h

#ifndef _NIU_MA_ZHEJIANG_MAHJONG_ROOM_HANDLER_H_
#define _NIU_MA_ZHEJIANG_MAHJONG_ROOM_HANDLER_H_

#include "Game/GameRoomHandler.h"

namespace NiuMa
{
	class ZhejiangMahjongRoomHandler : public GameRoomHandler
	{
	public:
		ZhejiangMahjongRoomHandler(const MessageQueue::Ptr& queue = nullptr);
		virtual ~ZhejiangMahjongRoomHandler();
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_ROOM_HANDLER_H_
