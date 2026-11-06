// NiuNiuRoom.cpp

#include "NiuNiuRoom.h"
#include "NiuNiuAvatar.h"
#include "NiuNiuMessages.h"
#include "../GameDefines.h"
#include "../Common/SimpleRoundRecordTask.h"
#include "../Common/SettleOutboxTask.h"
#include "MySql/MysqlPool.h"

#include "Base/BaseUtils.h"
#include "Base/Log.h"
#include "Game/GameMessages.h"
#include "Player/PlayerManager.h"

#include <algorithm>
#include <sstream>

namespace NiuMa
{
	namespace {
		const int DEAL_DELAY = 1500;		// 发牌动画
		const int COMPARE_DELAY = 3000;		// 亮牌/比牌
		const int SETTLE_DELAY = 5000;		// 结算后等待下一局
		const int MAX_SEATS = 5;
	}

	NiuNiuRoom::NiuNiuRoom(const std::shared_ptr<NiuNiuRule>& rule,
		const std::string& venueId, const std::string& number, int diZhu)
		: GameRoom(venueId, static_cast<int>(GameType::NiuNiu), MAX_SEATS)
		, _rule(rule)
		, _dealer(rule)
		, _number(number)
		, _diZhu(diZhu)
		, _gameState(GameState::Waiting)
		, _stateTick(0LL)
		, _robotTick(0LL)
		, _banker(-1)
	{
		for (int i = 0; i < 5; i++) {
			_scores[i] = 0;
			_genres[i] = 0;
		}
		setSilenceDeadline(1800);
	}

	NiuNiuRoom::~NiuNiuRoom() {}

	void NiuNiuRoom::initialize() {
		GameRoom::initialize();
		_stateTick = BaseUtils::getCurrentMillisecond();
	}

	GameAvatar::Ptr NiuNiuRoom::createAvatar(const std::string& playerId, int seat, bool robot) const {
		return std::make_shared<NiuNiuAvatar>(_rule, playerId, seat, robot);
	}

	bool NiuNiuRoom::checkEnter(const std::string& playerId, std::string& errMsg, bool robot) const {
		(void)playerId;
		(void)robot;
		if (isFull()) {
			errMsg = "游戏已满员";
			return false;
		}
		if (_gameState != GameState::Waiting && _gameState != GameState::Settled) {
			errMsg = "游戏正在进行中，请等待本局结束";
			return false;
		}
		return true;
	}

	int NiuNiuRoom::checkLeave(const std::string& playerId, std::string& errMsg) const {
		(void)playerId;
		if (_gameState == GameState::Dealing || _gameState == GameState::Playing) {
			errMsg = "本局尚未结束，不能离开房间";
			return 1;
		}
		return 0;
	}

	void NiuNiuRoom::clean() {
		for (int i = 0; i < MAX_SEATS; i++)
			_genres[i] = 0;
		_banker = -1;
	}

	bool NiuNiuRoom::beforeDestroy(bool silence) {
		(void)silence;
		return true;
	}

	bool NiuNiuRoom::canReplaceRobot() const {
		return (_gameState == GameState::Waiting);
	}

	void NiuNiuRoom::refreshSessions() {
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			GameAvatar::Ptr avatar = getAvatar(i);
			if (!avatar)
				continue;
			Player::Ptr player = PlayerManager::getSingleton().getPlayer(avatar->getPlayerId());
			if (player)
				avatar->setSession(player->getSession());
		}
	}

	void NiuNiuRoom::onTimer() {
		GameRoom::onTimer();

		time_t nowTick = BaseUtils::getCurrentMillisecond();
		int deltaTicks = static_cast<int>(nowTick - _stateTick);

		switch (_gameState) {
			case GameState::Waiting: {
				addRobot(nowTick);
				if (isFull() && isAllReady())
					beginDeal();
				break;
			}
			case GameState::Dealing: {
				if (deltaTicks >= DEAL_DELAY)
					beginCompare();
				break;
			}
			case GameState::Playing: {
				if (deltaTicks >= COMPARE_DELAY)
					settle(-1);
				break;
			}
			case GameState::Settled: {
				if (deltaTicks >= SETTLE_DELAY) {
					_gameState = GameState::Waiting;
					_stateTick = nowTick;
					clean();
					if (isFull() && isAllReady())
						beginDeal();
				}
				break;
			}
			default:
				break;
		}
	}

	bool NiuNiuRoom::onMessage(const NetMessage::Ptr& netMsg) {
		if (GameRoom::onMessage(netMsg))
			return true;

		const std::string& msgType = netMsg->getType();
		if (msgType == MsgNiuNiuSync::TYPE) {
			onSync(netMsg);
			return true;
		}
		if (msgType == MsgPlayerReady::TYPE) {
			MsgPlayerReady* inst = dynamic_cast<MsgPlayerReady*>(netMsg->getMessage().get());
			if (inst == nullptr)
				return true;
			GameAvatar::Ptr avatar = getAvatar(inst->getPlayerId());
			if (!avatar)
				return true;
			avatar->setReady(true);
			MsgPlayerReadyResp resp;
			resp.playerId = inst->getPlayerId();
			resp.seat = avatar->getSeat();
			sendMessageToAll(resp);
			if (_gameState == GameState::Waiting && isFull() && isAllReady())
				beginDeal();
			return true;
		}
		return false;
	}

	void NiuNiuRoom::beginDeal() {
		if (_gameState != GameState::Waiting)
			return;
		clean();
		_gameState = GameState::Dealing;
		_stateTick = BaseUtils::getCurrentMillisecond();

		std::ostringstream os;
		os << "牛牛牌桌(ID:" << getId() << ")发牌，玩家:";
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			GameAvatar::Ptr avatar = getAvatar(i);
			if (i > 0) os << "、";
			os << (avatar ? avatar->getPlayerId() : std::string("-"));
		}
		InfoS << os.str();

		_dealer.shuffle();
		_banker = static_cast<int>(BaseUtils::randInt(0, getMaxPlayerNums()));

		CardComparator comp(_rule);
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			NiuNiuAvatar* avatar = dynamic_cast<NiuNiuAvatar*>(getAvatar(i).get());
			if (avatar == nullptr)
				continue;
			CardArray cards;
			_dealer.handOutCards(cards, 5, DealFilter::Ptr());
			std::sort(cards.begin(), cards.end(), comp);
			avatar->setCards(cards);
		}

		MsgNiuNiuDealCard msg;
		msg.banker = _banker;
		sendMessageToAll(msg);
	}

	void NiuNiuRoom::beginCompare() {
		_gameState = GameState::Playing;
		_stateTick = BaseUtils::getCurrentMillisecond();
		refreshSessions();

		int bestSeat = -1;
		PokerGenre bestGenre;
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			NiuNiuAvatar* avatar = dynamic_cast<NiuNiuAvatar*>(getAvatar(i).get());
			if (avatar == nullptr || avatar->getCardNums() != 5)
				continue;
			PokerGenre genre;
			genre.getCards() = avatar->getCards();
			InfoS << "牛牛诊断 座位" << i << " avatarCards=" << avatar->getCardNums()
				<< " genreNums=" << genre.getCardNums();
			int ret = _rule->predicateCardGenre(genre);
			// predicateCardGenre 直接返回牌型值（不一定会写回 PokerGenre::genre），统一用返回值
			int genreValue = (ret > 0) ? ret : 0;
			_genres[i] = genreValue;
			InfoS << "牛牛牌桌(ID:" << getId() << ")座位" << i << " 牌型=" << genreValue;

			MsgNiuNiuHandCard hand;
			hand.cards = avatar->getCards();
			hand.genre = genreValue;
			hand.times = 1;
			hand.send(avatar->getSession());

			MsgNiuNiuCompare cmp;
			cmp.seat = i;
			cmp.genre = genreValue;
			cmp.times = 1;
			cmp.cards = avatar->getCards();
			sendMessageToAll(cmp);

			if (ret > 0) {
				if (bestSeat < 0 || _rule->compareGenre(genre, bestGenre) == 1) {
					bestSeat = i;
					bestGenre = genre;
				}
			}
		}
		if (bestSeat < 0) {
			InfoS << "牛牛牌桌(ID:" << getId() << ")无有效牌型，本局流局";
			settle(-1);
			return;
		}
		settle(bestSeat);
	}

	void NiuNiuRoom::settle(int winner) {
		_gameState = GameState::Settled;
		_stateTick = BaseUtils::getCurrentMillisecond();

		MsgNiuNiuGameOver msg;
		msg.winner = winner;
		msg.banker = _banker;
		if (winner >= 0) {
			for (int i = 0; i < getMaxPlayerNums(); i++) {
				if (i == winner) {
					msg.deltas[i] = _diZhu * (getMaxPlayerNums() - 1);
					_scores[i] += msg.deltas[i];
				}
				else {
					msg.deltas[i] = -_diZhu;
					_scores[i] += msg.deltas[i];
				}
			}
		}
		sendMessageToAll(msg);
		for (int i = 0; i < getMaxPlayerNums() && i < 5; i++)
			_lastDeltas[i] = msg.deltas[i];
		// 战绩落库
		{
			std::shared_ptr<SimpleRoundRecordTask> record = std::make_shared<SimpleRoundRecordTask>();
			record->_table = "game_niu_niu_record";
			record->_venueId = getId();
			record->_roundNo = ++_roundNo;
			record->_winner = winner;
			record->_desc = (winner < 0) ? "流局" : "牛牛";
			for (int i = 0; i < getMaxPlayerNums() && i < 5; i++) {
				GameAvatar::Ptr avatar = getAvatar(i);
				if (avatar)
					record->_playerIds[i] = avatar->getPlayerId();
				record->_deltas[i] = _lastDeltas[i];
			}
			MysqlPool::getSingleton().asyncQuery(record);
		}


		// 结算出站（供聚合平台入账）
		{
			std::shared_ptr<SettleOutboxTask> outbox = std::make_shared<SettleOutboxTask>();
			outbox->_venueId = getId();
			outbox->_roundNo = _roundNo;
			outbox->_gameType = static_cast<int>(GameType::NiuNiu);
			for (int i = 0; i < getMaxPlayerNums() && i < 5; i++) {
				GameAvatar::Ptr avatar = getAvatar(i);
				if (!avatar || _lastDeltas[i] == 0)
					continue;
				SettleEntry entry;
				entry.playerId = avatar->getPlayerId();
				entry.delta = _lastDeltas[i];
				outbox->_entries.push_back(entry);
			}
			if (!outbox->_entries.empty())
				MysqlPool::getSingleton().asyncQuery(outbox);
		}
		InfoS << "牛牛牌桌(ID:" << getId() << ")结算，赢家座位:" << winner
			<< "，庄家座位:" << _banker;
	}

	void NiuNiuRoom::onSync(const NetMessage::Ptr& netMsg) {
		MsgNiuNiuSync* inst = dynamic_cast<MsgNiuNiuSync*>(netMsg->getMessage().get());
		if (inst == nullptr)
			return;
		NiuNiuAvatar* avatar = dynamic_cast<NiuNiuAvatar*>(getAvatar(inst->getPlayerId()).get());
		if (avatar == nullptr)
			return;

		MsgNiuNiuSyncResp msg;
		msg.number = _number;
		msg.diZhu = _diZhu;
		msg.seat = avatar->getSeat();
		msg.banker = _banker;
		msg.gameState = static_cast<int>(_gameState);
		msg.handCards = avatar->getCards();
		for (int i = 0; i < 5; i++)
			msg.scores[i] = _scores[i];
		msg.send(netMsg->getSession());
	}

	void NiuNiuRoom::addRobot(time_t nowTick) {
		if (isFull())
			return;
		if (getAvatarCount() <= 0)
			return;
		if (_robotTick == 0) {
			_robotTick = nowTick;
			return;
		}
		if (nowTick - _robotTick < 1000)
			return;
		_robotTick = nowTick;

		int count = PlayerManager::getSingleton().getRobotCount();
		if (count <= 0)
			return;
		for (int loop = 0; loop < 4; loop++) {
			int robotId = static_cast<int>(BaseUtils::randInt(0, count)) + 1;
			std::string robotPlayerId;
			if (!PlayerManager::getSingleton().loadRobot(robotId, robotPlayerId))
				return;
			if (joinRobot(robotPlayerId)) {
				GameAvatar::Ptr avatar = getAvatar(robotPlayerId);
				if (avatar)
					avatar->setReady(true);
				InfoS << "牛牛补位：机器人(Id:" << robotPlayerId << ")进入房间(Id:" << getId()
					<< ")，当前人数:" << getAvatarCount();
				return;
			}
		}
	}
}
