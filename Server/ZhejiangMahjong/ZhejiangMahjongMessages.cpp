// ZhejiangMahjongMessages.cpp

#include "ZhejiangMahjongMessages.h"
#include "Message/MessageManager.h"

namespace NiuMa
{
	const std::string MsgZhejiangMahjongSync::TYPE("MsgZhejiangMahjongSync");

	const std::string MsgZhejiangMahjongSyncResp::TYPE("MsgZhejiangMahjongSyncResp");

	MsgZhejiangMahjongSyncResp::MsgZhejiangMahjongSyncResp()
		: baseScore(0)
		, seat(0)
		, dealer(0)
		, current(0)
		, gameState(0)
		, kingCard(0)
	{}

	const std::string MsgZhejiangMahjongDealCard::TYPE("MsgZhejiangMahjongDealCard");

	MsgZhejiangMahjongDealCard::MsgZhejiangMahjongDealCard()
		: dealer(0)
		, kingCard(0)
	{}

	const std::string MsgZhejiangMahjongHandCard::TYPE("MsgZhejiangMahjongHandCard");

	MsgZhejiangMahjongHandCard::MsgZhejiangMahjongHandCard() {}

	const std::string MsgZhejiangMahjongDraw::TYPE("MsgZhejiangMahjongDraw");

	MsgZhejiangMahjongDraw::MsgZhejiangMahjongDraw()
		: seat(0)
		, tile(0)
		, leftNum(0)
	{}

	const std::string MsgZhejiangMahjongTurn::TYPE("MsgZhejiangMahjongTurn");

	MsgZhejiangMahjongTurn::MsgZhejiangMahjongTurn()
		: seat(0)
		, second(0)
	{}

	const std::string MsgZhejiangMahjongDiscard::TYPE("MsgZhejiangMahjongDiscard");

	MsgZhejiangMahjongDiscard::MsgZhejiangMahjongDiscard()
		: tile(0)
	{}

	const std::string MsgZhejiangMahjongDiscardResp::TYPE("MsgZhejiangMahjongDiscardResp");

	MsgZhejiangMahjongDiscardResp::MsgZhejiangMahjongDiscardResp()
		: seat(0)
		, tile(0)
		, success(false)
	{}

	const std::string MsgZhejiangMahjongGameOver::TYPE("MsgZhejiangMahjongGameOver");

	MsgZhejiangMahjongGameOver::MsgZhejiangMahjongGameOver()
		: winner(-1)
		, zimo(false)
		, fromSeat(-1)
		, fan(0)
		, score(0)
	{}

	void ZhejiangMahjongMessages::registMessages() {
		IMsgCreator::Ptr creator;
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongSync>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongSync::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongSyncResp>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongSyncResp::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongDealCard>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongDealCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongHandCard>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongHandCard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongDraw>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongDraw::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongTurn>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongTurn::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongDiscard>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongDiscard::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongDiscardResp>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongDiscardResp::TYPE, creator);
		creator = IMsgCreator::Ptr(new MsgCreator<MsgZhejiangMahjongGameOver>());
		MessageManager::getSingleton().registCreator(MsgZhejiangMahjongGameOver::TYPE, creator);
	}
}
