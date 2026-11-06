// LandlordRoom.cpp

#include "LandlordRoom.h"
#include "LandlordRule.h"
#include "LandlordAvatar.h"
#include "LandlordMessages.h"
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
		// 单局超时（毫秒）
		const int PLAY_TIMEOUT = 20000;
		// 发牌动画等待（毫秒）
		const int DEAL_DELAY = 1500;
		// 结算后等待下一局（毫秒）
		const int SETTLE_DELAY = 5000;
	}

	LandlordRoom::LandlordRoom(const std::shared_ptr<LandlordRule>& rule,
		const std::string& venueId, const std::string& number, int diZhu)
		: GameRoom(venueId, static_cast<int>(GameType::Landlord), 3)
		, _rule(rule)
		, _dealer(rule)
		, _number(number)
		, _diZhu(diZhu)
		, _gameState(GameState::Waiting)
		, _stateTick(0LL)
		, _robotTick(0LL)
		, _landlord(-1)
		, _current(-1)
		, _playedSeat(-1)
		, _mustPlay(true)
		, _passCount(0)
	{
		for (int i = 0; i < 3; i++) {
			_scores[i] = 0;
			_cardNums[i] = 0;
		}
		setSilenceDeadline(1800);
	}

	LandlordRoom::~LandlordRoom() {}

	void LandlordRoom::initialize() {
		GameRoom::initialize();
		_stateTick = BaseUtils::getCurrentMillisecond();
	}

	GameAvatar::Ptr LandlordRoom::createAvatar(const std::string& playerId, int seat, bool robot) const {
		return std::make_shared<LandlordAvatar>(_rule, playerId, seat, robot);
	}

	bool LandlordRoom::checkEnter(const std::string& playerId, std::string& errMsg, bool robot) const {
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

	int LandlordRoom::checkLeave(const std::string& playerId, std::string& errMsg) const {
		(void)playerId;
		if (_gameState == GameState::Playing) {
			errMsg = "本局尚未结束，不能离开房间";
			return 1;
		}
		return 0;
	}

	void LandlordRoom::clean() {
		_playedCards.clear();
		_playedSeat = -1;
		_mustPlay = true;
		_passCount = 0;
		_landlord = -1;
	}

	bool LandlordRoom::beforeDestroy(bool silence) {
		(void)silence;
		return true;
	}

	bool LandlordRoom::canReplaceRobot() const {
		return (_gameState == GameState::Waiting);
	}

	void LandlordRoom::onTimer() {
		GameRoom::onTimer();

		time_t nowTick = BaseUtils::getCurrentMillisecond();
		int deltaTicks = static_cast<int>(nowTick - _stateTick);

		switch (_gameState) {
			case GameState::Waiting: {
				// 真人不足时补位机器人
				addRobot(nowTick);
				if (isFull() && isAllReady()) {
					InfoS << "斗地主房间(Id:" << getId() << ")人满且全部准备，开始发牌";
					beginDeal();
				}
				break;
			}
			case GameState::Dealing: {
				if (deltaTicks >= DEAL_DELAY)
					beginPlay();
				break;
			}
			case GameState::Playing: {
				if (deltaTicks >= PLAY_TIMEOUT) {
					if (_current >= 0)
						autoPlay(_current);
				}
				break;
			}
			case GameState::Settled: {
				if (deltaTicks >= SETTLE_DELAY) {
					// 可以开始下一局
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

	bool LandlordRoom::onMessage(const NetMessage::Ptr& netMsg) {
		if (GameRoom::onMessage(netMsg))
			return true;

		const std::string& msgType = netMsg->getType();
		if (msgType == MsgLandlordSync::TYPE) {
			onSync(netMsg);
			return true;
		}
		if (msgType == MsgLandlordPlayCard::TYPE) {
			onPlayCard(netMsg);
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

	void LandlordRoom::beginDeal() {
		if (_gameState != GameState::Waiting)
			return;
		clean();
		_gameState = GameState::Dealing;
		_stateTick = BaseUtils::getCurrentMillisecond();

		std::ostringstream os;
		os << "斗地主牌桌(ID:" << getId() << ")发牌，玩家:";
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			GameAvatar::Ptr avatar = getAvatar(i);
			if (i > 0) os << "、";
			os << (avatar ? avatar->getPlayerId() : std::string("-"));
		}
		InfoS << os.str();

		// 洗牌
		_dealer.shuffle();

		// 随机地主
		_landlord = static_cast<int>(BaseUtils::randInt(0, getMaxPlayerNums()));

		// 发牌：地主 17+3，其他人 17
		CardComparator comp(_rule);
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			LandlordAvatar* avatar = dynamic_cast<LandlordAvatar*>(getAvatar(i).get());
			if (avatar == nullptr)
				continue;
			CardArray cards;
			_dealer.handOutCards(cards, 17, DealFilter::Ptr());
			std::sort(cards.begin(), cards.end(), comp);
			avatar->setCards(cards);
			_cardNums[i] = static_cast<int>(cards.size());
		}
		// 底牌给地主
		LandlordAvatar* landlord = dynamic_cast<LandlordAvatar*>(getAvatar(_landlord).get());
		if (landlord) {
			CardArray bottom;
			_dealer.handOutCards(bottom, 3, DealFilter::Ptr());
			for (int i = 0; i < static_cast<int>(bottom.size()); i++)
				landlord->addCard(bottom[i]);
			landlord->sortCards();
			_cardNums[_landlord] = landlord->getCardNums();

			// 通知发牌
			MsgLandlordDealCard msg;
			msg.landlord = _landlord;
			msg.bottomCards = bottom;
			sendMessageToAll(msg);
		}

		// 下发各自手牌
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			LandlordAvatar* avatar = dynamic_cast<LandlordAvatar*>(getAvatar(i).get());
			if (avatar)
				sendHandCard(avatar);
		}
		notifyCardNums();
	}

	void LandlordRoom::beginPlay() {
		_gameState = GameState::Playing;
		_stateTick = BaseUtils::getCurrentMillisecond();
		_current = _landlord;
		_mustPlay = true;
		_playedCards.clear();
		_playedSeat = -1;
		_passCount = 0;
		InfoS << "斗地主房间(Id:" << getId() << ")开始出牌，地主座位:" << _landlord;
		notifyTurn(_current);
	}

	void LandlordRoom::notifyTurn(int seat) {
		refreshSessions();
		MsgLandlordTurn msg;
		msg.seat = seat;
		msg.mustPlay = _mustPlay;
		msg.second = PLAY_TIMEOUT / 1000;
		sendMessageToAll(msg);
		_stateTick = BaseUtils::getCurrentMillisecond();
		InfoS << "斗地主房间(Id:" << getId() << ")轮到座位" << seat << "出牌，必须出牌:" << (_mustPlay ? 1 : 0);
		// 机器人不需要等待超时，稍后立即出牌
		GameAvatar::Ptr avatar = getAvatar(seat);
		if (avatar && avatar->isRobot())
			_stateTick = _stateTick - PLAY_TIMEOUT + 800;
	}

	void LandlordRoom::refreshSessions() {
		// 防御：发牌/轮转时以玩家当前 session 为准，避免使用旧连接的 session
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			GameAvatar::Ptr avatar = getAvatar(i);
			if (!avatar)
				continue;
			Player::Ptr player = PlayerManager::getSingleton().getPlayer(avatar->getPlayerId());
			if (player)
				avatar->setSession(player->getSession());
		}
	}

	void LandlordRoom::sendHandCard(LandlordAvatar* avatar) {
		if (avatar == nullptr)
			return;
		MsgLandlordHandCard msg;
		msg.cards = avatar->getCards();
		InfoS << "斗地主发手牌：座位" << avatar->getSeat() << " 玩家" << avatar->getPlayerId()
			<< " 张数" << avatar->getCardNums() << " session=" << (avatar->getSession() ? "ok" : "null");
		msg.send(avatar->getSession());
	}

	void LandlordRoom::notifyCardNums() {
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			LandlordAvatar* avatar = dynamic_cast<LandlordAvatar*>(getAvatar(i).get());
			if (avatar)
				_cardNums[i] = avatar->getCardNums();
		}
	}

	void LandlordRoom::onSync(const NetMessage::Ptr& netMsg) {
		MsgLandlordSync* inst = dynamic_cast<MsgLandlordSync*>(netMsg->getMessage().get());
		if (inst == nullptr)
			return;
		LandlordAvatar* avatar = dynamic_cast<LandlordAvatar*>(getAvatar(inst->getPlayerId()).get());
		if (avatar == nullptr)
			return;

		MsgLandlordSyncResp msg;
		msg.number = _number;
		msg.diZhu = _diZhu;
		msg.seat = avatar->getSeat();
		msg.landlord = _landlord;
		msg.current = _current;
		msg.gameState = static_cast<int>(_gameState);
		msg.handCards = avatar->getCards();
		for (int i = 0; i < 3; i++) {
			msg.handCardNums[i] = _cardNums[i];
			msg.scores[i] = _scores[i];
		}
		msg.playedCards = _playedCards;
		msg.playedSeat = _playedSeat;
		msg.send(netMsg->getSession());
	}

	void LandlordRoom::onPlayCard(const NetMessage::Ptr& netMsg) {
		MsgLandlordPlayCard* inst = dynamic_cast<MsgLandlordPlayCard*>(netMsg->getMessage().get());
		if (inst == nullptr)
			return;
		LandlordAvatar* avatar = dynamic_cast<LandlordAvatar*>(getAvatar(inst->getPlayerId()).get());
		if (avatar == nullptr)
			return;

		MsgLandlordPlayCardResp resp;
		resp.seat = avatar->getSeat();

		if (_gameState != GameState::Playing) {
			resp.errMsg = "当前不在出牌阶段";
			resp.send(netMsg->getSession());
			return;
		}
		if (avatar->getSeat() != _current) {
			resp.errMsg = "还没轮到你出牌";
			resp.send(netMsg->getSession());
			return;
		}

		std::string errMsg;
		bool passed = false;
		bool ok = doPlayCard(avatar->getSeat(), inst->cardIds, errMsg, passed);
		resp.success = ok;
		resp.passed = passed;
		resp.errMsg = errMsg;
		if (ok && !passed) {
			resp.cards = _playedCards;
			resp.leftNums = avatar->getCardNums();
		}
		resp.send(netMsg->getSession());

		if (!ok)
			return;

		notifyCardNums();
		if (ok && !passed && avatar->getCardNums() == 0) {
			settle(avatar->getSeat());
			return;
		}
		// 轮到下一家
		_current = (_current + 1) % getMaxPlayerNums();
		if (_passCount >= getMaxPlayerNums() - 1) {
			// 其余全部不出，重新自由出牌
			_passCount = 0;
			_mustPlay = true;
			_playedCards.clear();
			_playedSeat = -1;
		}
		notifyTurn(_current);
	}

	bool LandlordRoom::doPlayCard(int seat, const std::vector<int>& cardIds, std::string& errMsg, bool& passed) {
		passed = false;
		LandlordAvatar* avatar = dynamic_cast<LandlordAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr) {
			errMsg = "玩家不存在";
			return false;
		}
		if (cardIds.empty()) {
			// 不出
			if (_mustPlay) {
				errMsg = "本轮必须出牌";
				return false;
			}
			passed = true;
			_passCount++;
			return true;
		}
		// 取出牌
		CardArray cards;
		if (!avatar->getCardsByIds(cardIds, cards)) {
			errMsg = "手牌中没有这些牌";
			return false;
		}
		// 牌型判定
		PokerGenre genre;
		genre.getCards() = cards;
		if (_rule->predicateCardGenre(genre) <= 0) {
			errMsg = "牌型不合法";
			return false;
		}
		// 比大小
		if (!_mustPlay) {
			PokerGenre lastGenre;
			lastGenre.getCards() = _playedCards;
			if (_rule->predicateCardGenre(lastGenre) <= 0) {
				errMsg = "桌面牌型异常";
				return false;
			}
			if (_rule->compareGenre(genre, lastGenre) != 1) {
				errMsg = "出的牌不够大";
				return false;
			}
		}
		// 从手牌移除
		avatar->removeCardsByIds(cardIds);
		_playedCards = cards;
		_playedSeat = seat;
		_mustPlay = false;
		_passCount = 0;
		return true;
	}

	void LandlordRoom::autoPlay(int seat) {
		LandlordAvatar* avatar = dynamic_cast<LandlordAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr)
			return;

		std::vector<int> cardIds;
		const CardArray& cards = avatar->getCards();
		if (_mustPlay) {
			// 首出：出最小的一张
			if (!cards.empty())
				cardIds.push_back(cards[0].getId());
		}
		else {
			// 需要压牌：尝试用最小的一张单牌压
			PokerGenre lastGenre;
			lastGenre.getCards() = _playedCards;
			if (_rule->predicateCardGenre(lastGenre) > 0) {
				for (int i = 0; i < static_cast<int>(cards.size()); i++) {
					CardArray one;
					one.push_back(cards[i]);
					PokerGenre g;
					g.getCards() = one;
					if (_rule->predicateCardGenre(g) <= 0)
						continue;
					if (_rule->compareGenre(g, lastGenre) == 1) {
						cardIds.push_back(cards[i].getId());
						break;
					}
				}
			}
		}

		std::string errMsg;
		bool passed = false;
		if (!doPlayCard(seat, cardIds, errMsg, passed)) {
			// 兜底：不出
			doPlayCard(seat, std::vector<int>(), errMsg, passed);
		}
		notifyCardNums();
		if (!passed && avatar->getCardNums() == 0) {
			settle(seat);
			return;
		}
		_current = (_current + 1) % getMaxPlayerNums();
		if (_passCount >= getMaxPlayerNums() - 1) {
			_passCount = 0;
			_mustPlay = true;
			_playedCards.clear();
			_playedSeat = -1;
		}
		notifyTurn(_current);
	}

	void LandlordRoom::settle(int winner) {
		_gameState = GameState::Settled;
		_stateTick = BaseUtils::getCurrentMillisecond();

		bool landlordWin = (winner == _landlord);
		MsgLandlordGameOver msg;
		msg.winner = winner;
		msg.landlordWin = landlordWin;
		// 简化计分：地主赢则地主 +2*底分，农民各 -底分；反之相反
		for (int i = 0; i < 3; i++) {
			if (landlordWin)
				msg.deltas[i] = (i == _landlord) ? (_diZhu * 2) : (-_diZhu);
			else
				msg.deltas[i] = (i == _landlord) ? (-_diZhu * 2) : _diZhu;
			_scores[i] += msg.deltas[i];
		}
		sendMessageToAll(msg);
		for (int i = 0; i < getMaxPlayerNums() && i < 5; i++)
			_lastDeltas[i] = msg.deltas[i];
		// 战绩落库
		{
			std::shared_ptr<SimpleRoundRecordTask> record = std::make_shared<SimpleRoundRecordTask>();
			record->_table = "game_landlord_record";
			record->_venueId = getId();
			record->_roundNo = ++_roundNo;
			record->_winner = winner;
			record->_desc = (winner < 0) ? "流局" : "斗地主";
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
			outbox->_gameType = static_cast<int>(GameType::Landlord);
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
		InfoS << "斗地主房间(Id:" << getId() << ")结算，赢家座位:" << winner
			<< "，地主" << (landlordWin ? "胜" : "负");
	}

	void LandlordRoom::addRobot(time_t nowTick) {
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
				InfoS << "斗地主补位：机器人(Id:" << robotPlayerId << ")进入房间(Id:" << getId()
					<< ")，当前人数:" << getAvatarCount();
				return;
			}
		}
	}
}
