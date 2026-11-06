// NiuNiuMessages.cpp

#include "NiuNiuMessages.h"
#include "Message/MessageManager.h"

namespace NiuMa
{
	const std::string MsgNiuNiuSync::TYPE("MsgNiuNiuSync");

	const std::string MsgNiuNiuSyncResp::TYPE("MsgNiuNiuSyncResp");

	MsgNiuNiuSyncResp::MsgNiuNiuSyncResp()
		: diZhu(0)
		, seat(0)
		, banker(-1)
		, gameState(0)
	{
		for (int i = 0; i < 5; i++)
			scores[i] = 0;
	}

	const std::string MsgNiuNiuDealCard::TYPE("MsgNiuNiuDealCard");

	MsgNiuNiuDealCard::MsgNiuNiuDealCard()
		: banker(-1)
	{}

	const std::string MsgNiuNiuHandCard::TYPE("MsgNiuNiuHandCard");

	MsgNiuNiuHandCard::MsgNiuNiuHandCard()
		: genre(0)
		, times(1)
	{}

	const std::string MsgNiuNiuCompare::TYPE("MsgNiuNiuCompare");

	MsgNiuNiuCompare::MsgNiuNiuCompare()
		: seat(0)
		, genre(0)
		, times(1)
	{}

	const std::string MsgNiuNiuGameOver::TYPE("MsgNiuNiuGameOver");

	MsgNiuNiuGameOver::MsgNiuNiuGameOver()
		: winner(-1)
		, banker(-1)
	{
		for (int i = 0; i < 5; i++)
			deltas[i] = 0;
	}

	void NiuNiuMessages::registMessages() {
		IMsgCreator::Ptr creator;
		creator = IMsgCreator::Ptr(new MsgCreator<MsgNiuNiuSync>());
		MessageManager::getSingleton().registCreator(MsgNiuNiuSync::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgNiuNiuSyncResp>());
		MessageManager::getSingleton().registCreator(MsgNiuNiuSyncResp::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgNiuNiuDealCard>());
		MessageManager::getSingleton().registCreator(MsgNiuNiuDealCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgNiuNiuHandCard>());
		MessageManager::getSingleton().registCreator(MsgNiuNiuHandCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgNiuNiuCompare>());
		MessageManager::getSingleton().registCreator(MsgNiuNiuCompare::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgNiuNiuGameOver>());
		MessageManager::getSingleton().registCreator(MsgNiuNiuGameOver::TYPE, creator);
	}
}
