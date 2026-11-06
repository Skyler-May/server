// PaoDeKuaiRoomHandler.h

#ifndef _NIU_MA_PAO_DE_KUAI_ROOM_HANDLER_H_
#define _NIU_MA_PAO_DE_KUAI_ROOM_HANDLER_H_

#include "Game/GameRoomHandler.h"

namespace NiuMa
{
	class PaoDeKuaiRoomHandler : public GameRoomHandler
	{
	public:
		PaoDeKuaiRoomHandler(const MessageQueue::Ptr& queue = nullptr);
		virtual ~PaoDeKuaiRoomHandler();
	};
}

#endif // !_NIU_MA_PAO_DE_KUAI_ROOM_HANDLER_H_
