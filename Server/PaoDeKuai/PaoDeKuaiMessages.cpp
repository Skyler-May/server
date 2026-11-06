// PaoDeKuaiMessages.cpp

#include "PaoDeKuaiMessages.h"
#include "Message/MessageManager.h"

namespace NiuMa
{
	const std::string MsgPaoDeKuaiSync::TYPE("MsgPaoDeKuaiSync");

	const std::string MsgPaoDeKuaiSyncResp::TYPE("MsgPaoDeKuaiSyncResp");

	MsgPaoDeKuaiSyncResp::MsgPaoDeKuaiSyncResp()
		: diZhu(0)
		, seat(0)
		, landlord(0)
		, current(0)
		, gameState(0)
		, playedSeat(-1)
	{
		for (int i = 0; i < 3; i++) {
			handCardNums[i] = 0;
			scores[i] = 0;
		}
	}

	const std::string MsgPaoDeKuaiDealCard::TYPE("MsgPaoDeKuaiDealCard");

	MsgPaoDeKuaiDealCard::MsgPaoDeKuaiDealCard()
		: landlord(0)
	{}

	const std::string MsgPaoDeKuaiHandCard::TYPE("MsgPaoDeKuaiHandCard");

	MsgPaoDeKuaiHandCard::MsgPaoDeKuaiHandCard() {}

	const std::string MsgPaoDeKuaiTurn::TYPE("MsgPaoDeKuaiTurn");

	MsgPaoDeKuaiTurn::MsgPaoDeKuaiTurn()
		: seat(0)
		, mustPlay(false)
		, second(0)
	{}

	const std::string MsgPaoDeKuaiPlayCard::TYPE("MsgPaoDeKuaiPlayCard");

	MsgPaoDeKuaiPlayCard::MsgPaoDeKuaiPlayCard() {}

	const std::string MsgPaoDeKuaiPlayCardResp::TYPE("MsgPaoDeKuaiPlayCardResp");

	MsgPaoDeKuaiPlayCardResp::MsgPaoDeKuaiPlayCardResp()
		: seat(0)
		, success(false)
		, passed(false)
		, leftNums(0)
	{}

	const std::string MsgPaoDeKuaiGameOver::TYPE("MsgPaoDeKuaiGameOver");

	MsgPaoDeKuaiGameOver::MsgPaoDeKuaiGameOver()
		: winner(-1)
		, landlordWin(false)
	{
		for (int i = 0; i < 3; i++)
			deltas[i] = 0;
	}

	void PaoDeKuaiMessages::registMessages() {
		IMsgCreator::Ptr creator;
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiSync>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiSync::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiSyncResp>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiSyncResp::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiDealCard>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiDealCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiHandCard>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiHandCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiTurn>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiTurn::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiPlayCard>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiPlayCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiPlayCardResp>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiPlayCardResp::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgPaoDeKuaiGameOver>());
		MessageManager::getSingleton().registCreator(MsgPaoDeKuaiGameOver::TYPE, creator);
	}
}
