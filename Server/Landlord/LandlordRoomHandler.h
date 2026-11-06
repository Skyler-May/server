// LandlordRoomHandler.h

#ifndef _NIU_MA_LANDLORD_ROOM_HANDLER_H_
#define _NIU_MA_LANDLORD_ROOM_HANDLER_H_

#include "Game/GameRoomHandler.h"

namespace NiuMa
{
	class LandlordRoomHandler : public GameRoomHandler
	{
	public:
		LandlordRoomHandler(const MessageQueue::Ptr& queue = nullptr);
		virtual ~LandlordRoomHandler();
	};
}

#endif // !_NIU_MA_LANDLORD_ROOM_HANDLER_H_
