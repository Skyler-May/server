// LandlordMessages.cpp

#include "LandlordMessages.h"
#include "Message/MessageManager.h"

namespace NiuMa
{
	const std::string MsgLandlordSync::TYPE("MsgLandlordSync");

	const std::string MsgLandlordSyncResp::TYPE("MsgLandlordSyncResp");

	MsgLandlordSyncResp::MsgLandlordSyncResp()
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

	const std::string MsgLandlordDealCard::TYPE("MsgLandlordDealCard");

	MsgLandlordDealCard::MsgLandlordDealCard()
		: landlord(0)
	{}

	const std::string MsgLandlordHandCard::TYPE("MsgLandlordHandCard");

	MsgLandlordHandCard::MsgLandlordHandCard() {}

	const std::string MsgLandlordTurn::TYPE("MsgLandlordTurn");

	MsgLandlordTurn::MsgLandlordTurn()
		: seat(0)
		, mustPlay(false)
		, second(0)
	{}

	const std::string MsgLandlordPlayCard::TYPE("MsgLandlordPlayCard");

	MsgLandlordPlayCard::MsgLandlordPlayCard() {}

	const std::string MsgLandlordPlayCardResp::TYPE("MsgLandlordPlayCardResp");

	MsgLandlordPlayCardResp::MsgLandlordPlayCardResp()
		: seat(0)
		, success(false)
		, passed(false)
		, leftNums(0)
	{}

	const std::string MsgLandlordGameOver::TYPE("MsgLandlordGameOver");

	MsgLandlordGameOver::MsgLandlordGameOver()
		: winner(-1)
		, landlordWin(false)
	{
		for (int i = 0; i < 3; i++)
			deltas[i] = 0;
	}

	void LandlordMessages::registMessages() {
		IMsgCreator::Ptr creator;
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordSync>());
		MessageManager::getSingleton().registCreator(MsgLandlordSync::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordSyncResp>());
		MessageManager::getSingleton().registCreator(MsgLandlordSyncResp::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordDealCard>());
		MessageManager::getSingleton().registCreator(MsgLandlordDealCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordHandCard>());
		MessageManager::getSingleton().registCreator(MsgLandlordHandCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordTurn>());
		MessageManager::getSingleton().registCreator(MsgLandlordTurn::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordPlayCard>());
		MessageManager::getSingleton().registCreator(MsgLandlordPlayCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordPlayCardResp>());
		MessageManager::getSingleton().registCreator(MsgLandlordPlayCardResp::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgLandlordGameOver>());
		MessageManager::getSingleton().registCreator(MsgLandlordGameOver::TYPE, creator);
	}
}
