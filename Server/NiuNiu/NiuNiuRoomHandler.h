// NiuNiuRoomHandler.h

#ifndef _NIU_MA_NIU_NIU_ROOM_HANDLER_H_
#define _NIU_MA_NIU_NIU_ROOM_HANDLER_H_

#include "Game/GameRoomHandler.h"

namespace NiuMa
{
	class NiuNiuRoomHandler : public GameRoomHandler
	{
	public:
		NiuNiuRoomHandler(const MessageQueue::Ptr& queue = nullptr);
		virtual ~NiuNiuRoomHandler();
	};
}

#endif // !_NIU_MA_NIU_NIU_ROOM_HANDLER_H_
