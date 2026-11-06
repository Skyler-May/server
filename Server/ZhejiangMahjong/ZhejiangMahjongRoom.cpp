// ZhejiangMahjongRoom.cpp

#include "ZhejiangMahjongRoom.h"
#include "ZhejiangMahjongAvatar.h"
#include "ZhejiangMahjongMessages.h"
#include "ZhejiangHuChecker.h"
#include "../GameDefines.h"

#include "Base/BaseUtils.h"
#include "Base/Log.h"
#include "Game/GameMessages.h"
#include "Player/PlayerManager.h"

#include <algorithm>
#include <sstream>

namespace NiuMa
{
	namespace {
		const int DEAL_DELAY = 2000;		// 发牌
		const int DISCARD_TIMEOUT = 20000;	// 出牌超时
		const int SETTLE_DELAY = 6000;		// 结算后等待
		const int MAX_SEATS = 4;
		const int HAND_NUM = 13;

		// 生成 136 张牌墙（万/条/筒/字 各 4 张）
		void buildWall(std::vector<int>& wall) {
			wall.clear();
			static const int colors[] = { 0x00, 0x10, 0x20, 0x30 };
			for (int c = 0; c < 4; c++) {
				int maxValue = (colors[c] == 0x30) ? 7 : 9;
				for (int v = 1; v <= maxValue; v++) {
					for (int k = 0; k < 4; k++)
						wall.push_back(colors[c] | v);
				}
			}
		}
	}

	ZhejiangMahjongRoom::ZhejiangMahjongRoom(const std::string& venueId, const std::string& number, int baseScore)
		: GameRoom(venueId, static_cast<int>(GameType::ZhejiangMahjong), MAX_SEATS)
		, _number(number)
		, _baseScore(baseScore > 0 ? baseScore : 1)
		, _gameState(GameState::Waiting)
		, _stateTick(0LL)
		, _robotTick(0LL)
		, _dealer(0)
		, _current(0)
		, _kingCard(0)
		, _waitingDiscard(false)
		, _lastDiscardTile(0)
		, _lastDiscardSeat(-1)
	{
		_scores.assign(MAX_SEATS, 0);
		_checker = std::make_shared<ZhejiangHuChecker>();
		setSilenceDeadline(1800);
	}

	ZhejiangMahjongRoom::~ZhejiangMahjongRoom() {}

	void ZhejiangMahjongRoom::initialize() {
		GameRoom::initialize();
		_stateTick = BaseUtils::getCurrentMillisecond();
	}

	GameAvatar::Ptr ZhejiangMahjongRoom::createAvatar(const std::string& playerId, int seat, bool robot) const {
		return std::make_shared<ZhejiangMahjongAvatar>(playerId, seat, robot);
	}

	bool ZhejiangMahjongRoom::checkEnter(const std::string& playerId, std::string& errMsg, bool robot) const {
		(void)playerId;
		(void)robot;
		if (isFull()) {
			errMsg = "游戏已满员";
			return false;
		}
		if (_gameState != GameState::Waiting && _gameState != GameState::Settled) {
			errMsg = "游戏正在进行中";
			return false;
		}
		return true;
	}

	int ZhejiangMahjongRoom::checkLeave(const std::string& playerId, std::string& errMsg) const {
		(void)playerId;
		if (_gameState == GameState::Dealing || _gameState == GameState::Playing) {
			errMsg = "本局尚未结束，不能离开房间";
			return 1;
		}
		return 0;
	}

	void ZhejiangMahjongRoom::clean() {
		_wall.clear();
		_lastDiscardTile = 0;
		_lastDiscardSeat = -1;
		_current = 0;
		_waitingDiscard = false;
	}

	bool ZhejiangMahjongRoom::beforeDestroy(bool silence) {
		(void)silence;
		return true;
	}

	bool ZhejiangMahjongRoom::canReplaceRobot() const {
		return (_gameState == GameState::Waiting);
	}

	void ZhejiangMahjongRoom::refreshSessions() {
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			GameAvatar::Ptr avatar = getAvatar(i);
			if (!avatar)
				continue;
			Player::Ptr player = PlayerManager::getSingleton().getPlayer(avatar->getPlayerId());
			if (player)
				avatar->setSession(player->getSession());
		}
	}

	int ZhejiangMahjongRoom::nextSeat(int seat) const {
		return (seat + 1) % getMaxPlayerNums();
	}

	void ZhejiangMahjongRoom::onTimer() {
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
					beginPlay();
				break;
			}
			case GameState::Playing: {
				if (_waitingDiscard && deltaTicks >= DISCARD_TIMEOUT) {
					if (_current >= 0)
						autoDiscard(_current);
				}
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

	bool ZhejiangMahjongRoom::onMessage(const NetMessage::Ptr& netMsg) {
		if (GameRoom::onMessage(netMsg))
			return true;

		const std::string& msgType = netMsg->getType();
		if (msgType == MsgZhejiangMahjongSync::TYPE) {
			onSync(netMsg);
			return true;
		}
		if (msgType == MsgZhejiangMahjongDiscard::TYPE) {
			onDiscard(netMsg);
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

	void ZhejiangMahjongRoom::beginDeal() {
		if (_gameState != GameState::Waiting)
			return;
		clean();
		_gameState = GameState::Dealing;
		_stateTick = BaseUtils::getCurrentMillisecond();

		// 洗牌
		buildWall(_wall);
		for (int i = static_cast<int>(_wall.size()) - 1; i > 0; i--) {
			int j = static_cast<int>(BaseUtils::randInt(0, i + 1));
			std::swap(_wall[i], _wall[j]);
		}

		// 财神：本局随机一张牌（简化：取牌墙最后一张，保证只在万/条/筒里）
		_kingCard = _wall.back();
		_checker->setKingCard(static_cast<BYTE>(_kingCard));

		_dealer = static_cast<int>(BaseUtils::randInt(0, getMaxPlayerNums()));

		std::ostringstream os;
		os << "浙江麻将牌桌(ID:" << getId() << ")发牌，庄家座位:" << _dealer
			<< "，财神:0x" << std::hex << _kingCard << std::dec;
		InfoS << os.str();

		// 发牌：每人 13 张
		for (int round = 0; round < HAND_NUM; round++) {
			for (int i = 0; i < getMaxPlayerNums(); i++) {
				int seat = (_dealer + i) % getMaxPlayerNums();
				ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
				if (!avatar || _wall.empty())
					continue;
				avatar->addTile(_wall.back());
				_wall.pop_back();
			}
		}

		MsgZhejiangMahjongDealCard msg;
		msg.dealer = _dealer;
		msg.kingCard = _kingCard;
		sendMessageToAll(msg);

		for (int i = 0; i < getMaxPlayerNums(); i++)
			sendHand(i);
	}

	void ZhejiangMahjongRoom::beginPlay() {
		_gameState = GameState::Playing;
		_stateTick = BaseUtils::getCurrentMillisecond();
		_current = _dealer;
		// 庄家摸第 14 张
		drawTile(_dealer);
	}

	void ZhejiangMahjongRoom::drawTile(int seat) {
		// 递归调用保护：本局若已结束（自摸/点炮/流局），不再继续摸牌
		if (_gameState != GameState::Playing)
			return;
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr)
			return;
		if (_wall.empty()) {
			InfoS << "浙江麻将(ID:" << getId() << ")牌墙已空，流局";
			settle(-1, false, -1, 0, 0, "流局");
			return;
		}
		int tile = _wall.back();
		_wall.pop_back();
		avatar->addTile(tile);

		// 暗杠检查（杠完补摸，最多连续 4 次）
		int kongLoop = 0;
		while (kongLoop < 4 && tryConcealedKong(seat)) {
			kongLoop++;
			if (_wall.empty()) {
				InfoS << "浙江麻将(ID:" << getId() << ")牌墙已空，流局";
				settle(-1, false, -1, 0, 0, "流局");
				return;
			}
			tile = _wall.back();
			_wall.pop_back();
			avatar->addTile(tile);
		}

		refreshSessions();
		MsgZhejiangMahjongDraw msg;
		msg.seat = seat;
		msg.tile = tile;
		msg.leftNum = static_cast<int>(_wall.size());
		msg.send(avatar->getSession());

		// 自摸检查
		int fan = 0, score = 0;
		std::string desc;
		bool canHu = checkHuFor(seat, true, -1, 0, fan, score, desc);
		if (canHu) {
			settle(seat, true, -1, fan, score, desc);
			return;
		}
		waitDiscard(seat);
	}

	void ZhejiangMahjongRoom::waitDiscard(int seat) {
		if (_gameState != GameState::Playing)
			return;
		_current = seat;
		_waitingDiscard = true;
		_stateTick = BaseUtils::getCurrentMillisecond();

		MsgZhejiangMahjongTurn turn;
		turn.seat = seat;
		turn.second = DISCARD_TIMEOUT / 1000;
		sendMessageToAll(turn);

		GameAvatar::Ptr avatar = getAvatar(seat);
		if (avatar && avatar->isRobot()) {
			// 机器人立刻出牌
			autoDiscard(seat);
		}
	}

	void ZhejiangMahjongRoom::onDiscard(const NetMessage::Ptr& netMsg) {
		MsgZhejiangMahjongDiscard* inst = dynamic_cast<MsgZhejiangMahjongDiscard*>(netMsg->getMessage().get());
		if (inst == nullptr)
			return;
		GameAvatar::Ptr avatar = getAvatar(inst->getPlayerId());
		if (!avatar)
			return;

		MsgZhejiangMahjongDiscardResp resp;
		resp.seat = avatar->getSeat();
		resp.tile = inst->tile;

		if (_gameState != GameState::Playing || !_waitingDiscard || avatar->getSeat() != _current) {
			resp.errMsg = "还没轮到你出牌";
			resp.send(netMsg->getSession());
			return;
		}
		std::string errMsg;
		if (!discardTile(avatar->getSeat(), inst->tile, errMsg)) {
			resp.errMsg = errMsg;
			resp.send(netMsg->getSession());
			return;
		}
		resp.success = true;
		sendMessageToAll(resp);
	}

	bool ZhejiangMahjongRoom::discardTile(int seat, int tile, std::string& errMsg) {
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr) {
			errMsg = "玩家不存在";
			return false;
		}
		if (!avatar->removeTile(tile)) {
			errMsg = "手上没有这张牌";
			return false;
		}
		_waitingDiscard = false;
		_stateTick = BaseUtils::getCurrentMillisecond();
		_lastDiscardTile = tile;
		_lastDiscardSeat = seat;
		InfoS << "浙江麻将(ID:" << getId() << ")座位" << seat << "打出 0x"
			<< std::hex << tile << std::dec;

		sendHand(seat);

		// 点炮检查（优先于碰）
		if (checkRobHu(seat))
			return true;
		// 杠优先于碰
		if (checkGang(seat))
			return true;
		// 碰牌检查
		if (checkPong(seat))
			return true;
		// 若已结算（点炮胡），不再继续
		if (_gameState != GameState::Playing)
			return true;

		// 下一家摸牌
		int next = nextSeat(seat);
		drawTile(next);
		return true;
	}

	bool ZhejiangMahjongRoom::checkHuFor(int seat, bool zimo, int fromSeat, int winTile, int& fan, int& score, std::string& desc) {
		(void)fromSeat;
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr)
			return false;

		std::vector<BYTE> cards;
		const std::vector<int>& hand = avatar->getHand();
		for (size_t i = 0; i < hand.size(); i++)
			cards.push_back(static_cast<BYTE>(hand[i]));

		int meldCount = avatar->getMeldCount();
		int expectHand = HAND_NUM - meldCount * 3;		// 未摸牌时的手牌数

		ZhejiangHuResult result;
		bool hu = false;
		if (zimo) {
			// 自摸：手牌为 expectHand+1 张，最后一张为刚摸到的牌
			if (static_cast<int>(cards.size()) != expectHand + 1)
				return false;
			std::vector<BYTE> hand13(cards.begin(), cards.end() - 1);
			hu = _checker->checkHu(hand13, cards.back(), true, result, meldCount);
			if (!hu)
				hu = _checker->checkHu(cards, cards.back(), true, result, meldCount);
		}
		else {
			// 点炮：手牌 expectHand 张 + 别人打出的牌
			if (static_cast<int>(cards.size()) != expectHand || winTile == 0)
				return false;
			cards.push_back(static_cast<BYTE>(winTile));
			std::vector<BYTE> hand13(cards.begin(), cards.end() - 1);
			hu = _checker->checkHu(hand13, static_cast<BYTE>(winTile), false, result, meldCount);
			if (!hu)
				hu = _checker->checkHu(cards, static_cast<BYTE>(winTile), false, result, meldCount);
		}
		if (!hu)
			return false;
		fan = result.fan;
		score = result.score * _baseScore;
		desc = result.desc;
		return true;
	}

	// 明杠：其他家有 3 张同牌
	bool ZhejiangMahjongRoom::checkGang(int fromSeat) {
		if (_lastDiscardTile == 0)
			return false;
		for (int i = 1; i < getMaxPlayerNums(); i++) {
			int seat = (fromSeat + i) % getMaxPlayerNums();
			ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
			if (avatar == nullptr)
				continue;
			int same = 0;
			const std::vector<int>& hand = avatar->getHand();
			for (size_t k = 0; k < hand.size(); k++) {
				if (hand[k] == _lastDiscardTile)
					same++;
			}
			if (same < 3)
				continue;
			for (int n = 0; n < 3; n++)
				avatar->removeTile(_lastDiscardTile);
			avatar->addMeld(_lastDiscardTile);

			MsgZhejiangMahjongGang msg;
			msg.seat = seat;
			msg.tile = _lastDiscardTile;
			msg.fromSeat = fromSeat;
			msg.concealed = false;
			sendMessageToAll(msg);
			sendHand(seat);
			InfoS << "浙江麻将(ID:" << getId() << ")座位" << seat << " 明杠 0x"
				<< std::hex << _lastDiscardTile << std::dec << "（来自座位" << fromSeat << "）";

			// 杠后补摸一张，再出牌
			drawTile(seat);
			return true;
		}
		return false;
	}

	// 暗杠：自己摸牌后手上有 4 张同牌
	bool ZhejiangMahjongRoom::tryConcealedKong(int seat) {
		if (_gameState != GameState::Playing)
			return false;
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr)
			return false;
		const std::vector<int>& hand = avatar->getHand();
		int target = 0;
		for (size_t i = 0; i < hand.size(); i++) {
			int c = 0;
			for (size_t k = 0; k < hand.size(); k++) {
				if (hand[k] == hand[i])
					c++;
			}
			if (c >= 4 && hand[i] != _kingCard) {
				target = hand[i];
				break;
			}
		}
		if (target == 0)
			return false;
		for (int n = 0; n < 4; n++)
			avatar->removeTile(target);
		avatar->addMeld(target);

		MsgZhejiangMahjongGang msg;
		msg.seat = seat;
		msg.tile = target;
		msg.fromSeat = -1;
		msg.concealed = true;
		sendMessageToAll(msg);
		sendHand(seat);
		InfoS << "浙江麻将(ID:" << getId() << ")座位" << seat << " 暗杠 0x"
			<< std::hex << target << std::dec;
		return true;
	}
	bool ZhejiangMahjongRoom::checkPong(int fromSeat) {
		if (_lastDiscardTile == 0)
			return false;
		for (int i = 1; i < getMaxPlayerNums(); i++) {
			int seat = (fromSeat + i) % getMaxPlayerNums();
			ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
			if (avatar == nullptr)
				continue;
			// 手上有两张同样的牌才能碰
			int same = 0;
			const std::vector<int>& hand = avatar->getHand();
			for (size_t k = 0; k < hand.size(); k++) {
				if (hand[k] == _lastDiscardTile)
					same++;
			}
			if (same < 2)
				continue;
			// 策略：只碰字牌和幺九牌（1/9），避免机器人乱碰把手牌拆散
			int color = (_lastDiscardTile & 0xF0) >> 4;
			int value = _lastDiscardTile & 0x0F;
			bool worth = (color == 3) || (value == 1) || (value == 9);
			if (!worth)
				continue;
			avatar->removeTile(_lastDiscardTile);
			avatar->removeTile(_lastDiscardTile);
			avatar->addMeld(_lastDiscardTile);

			MsgZhejiangMahjongPong msg;
			msg.seat = seat;
			msg.tile = _lastDiscardTile;
			msg.fromSeat = fromSeat;
			sendMessageToAll(msg);
			sendHand(seat);
			InfoS << "浙江麻将(ID:" << getId() << ")座位" << seat << " 碰 0x"
				<< std::hex << _lastDiscardTile << std::dec << "（来自座位" << fromSeat << "）";

			// 碰后由该玩家出牌
			waitDiscard(seat);
			return true;
		}
		return false;
	}
	bool ZhejiangMahjongRoom::checkRobHu(int fromSeat) {
		if (_lastDiscardTile == 0)
			return false;
		// 按出牌顺序逆时针遍历其他三家
		for (int i = 1; i < getMaxPlayerNums(); i++) {
			int seat = (fromSeat + i) % getMaxPlayerNums();
			ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
			if (avatar == nullptr)
				continue;
			if (avatar->getHandNum() != HAND_NUM - avatar->getMeldCount() * 3)
				continue;
			int fan = 0, score = 0;
			std::string desc;
			if (checkHuFor(seat, false, fromSeat, _lastDiscardTile, fan, score, desc)) {
				InfoS << "浙江麻将(ID:" << getId() << ")座位" << seat << " 点炮胡！来自座位"
					<< fromSeat << " 的 0x" << std::hex << _lastDiscardTile << std::dec;
				settle(seat, false, fromSeat, fan, score, desc);
				return true;
			}
		}
		return false;
	}

	// 出牌决策：给每张牌打"有用度"分，打掉最没用的（孤张、幺九优先），财神永远不打
	int ZhejiangMahjongRoom::pickDiscardTile(int seat) const {
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr || avatar->getHandNum() == 0)
			return 0;
		const std::vector<int>& hand = avatar->getHand();

		int bestTile = 0;
		int bestScore = 1 << 20;
		for (size_t i = 0; i < hand.size(); i++) {
			int tile = hand[i];
			if (tile == _kingCard)
				continue;					// 财神不打
			int color = (tile & 0xF0) >> 4;
			int value = tile & 0x0F;

			int score = 0;
			// 同张数
			int same = 0;
			for (size_t k = 0; k < hand.size(); k++) {
				if (hand[k] == tile)
					same++;
			}
			if (same >= 3) score += 12;
			else if (same == 2) score += 7;

			// 邻牌（同花色内 ±1 / ±2）
			if (color != 3) {
				for (size_t k = 0; k < hand.size(); k++) {
					int other = hand[k];
					if (((other & 0xF0) >> 4) != color)
						continue;
					int diff = (other & 0x0F) - value;
					if (diff == 1 || diff == -1) score += 3;
					else if (diff == 2 || diff == -2) score += 1;
				}
			}
			// 幺九、字牌单张价值低
			if (value == 1 || value == 9) score -= 1;
			if (color == 3 && same == 1) score -= 2;

			if (score < bestScore) {
				bestScore = score;
				bestTile = tile;
			}
		}
		if (bestTile == 0 && !hand.empty())
			bestTile = hand.back();
		return bestTile;
	}

	void ZhejiangMahjongRoom::autoDiscard(int seat) {
		if (_gameState != GameState::Playing || !_waitingDiscard)
			return;
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr || avatar->getHandNum() == 0)
			return;
		int tile = pickDiscardTile(seat);
		if (tile == 0)
			tile = avatar->getHand().back();
		std::string errMsg;
		discardTile(seat, tile, errMsg);
	}

	void ZhejiangMahjongRoom::settle(int winner, bool zimo, int fromSeat, int fan, int score, const std::string& desc) {
		_gameState = GameState::Settled;
		_stateTick = BaseUtils::getCurrentMillisecond();
		_waitingDiscard = false;

		MsgZhejiangMahjongGameOver msg;
		msg.winner = winner;
		msg.zimo = zimo;
		msg.fromSeat = fromSeat;
		msg.fan = fan;
		msg.score = score;
		msg.desc = desc;
		msg.deltas.assign(getMaxPlayerNums(), 0);

		if (winner >= 0) {
			if (zimo) {
				// 自摸：三家各付
				for (int i = 0; i < getMaxPlayerNums(); i++) {
					if (i == winner)
						msg.deltas[i] = score * (getMaxPlayerNums() - 1);
					else
						msg.deltas[i] = -score;
					_scores[i] += msg.deltas[i];
				}
			}
			else if (fromSeat >= 0) {
				// 点炮：点炮者付双倍
				for (int i = 0; i < getMaxPlayerNums(); i++) {
					if (i == winner)
						msg.deltas[i] = score;
					else if (i == fromSeat)
						msg.deltas[i] = -score;
					_scores[i] += msg.deltas[i];
				}
			}
		}

		sendMessageToAll(msg);
		InfoS << "浙江麻将(ID:" << getId() << ")结算，赢家座位:" << winner
			<< (zimo ? "(自摸)" : "(点炮)") << "，番:" << fan << "，得分:" << score << "，牌型:" << desc;
	}

	void ZhejiangMahjongRoom::onSync(const NetMessage::Ptr& netMsg) {
		MsgZhejiangMahjongSync* inst = dynamic_cast<MsgZhejiangMahjongSync*>(netMsg->getMessage().get());
		if (inst == nullptr)
			return;
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(inst->getPlayerId()).get());
		if (avatar == nullptr)
			return;

		MsgZhejiangMahjongSyncResp msg;
		msg.number = _number;
		msg.baseScore = _baseScore;
		msg.seat = avatar->getSeat();
		msg.dealer = _dealer;
		msg.current = _current;
		msg.gameState = static_cast<int>(_gameState);
		msg.kingCard = _kingCard;
		msg.hand = avatar->getHand();
		msg.scores = _scores;
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			GameAvatar::Ptr tmp = getAvatar(i);
			msg.handNums.push_back(tmp ? 0 : 0);
		}
		for (int i = 0; i < getMaxPlayerNums(); i++) {
			ZhejiangMahjongAvatar* tmp = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(i).get());
			msg.handNums[i] = tmp ? tmp->getHandNum() : 0;
		}
		msg.send(netMsg->getSession());
	}

	void ZhejiangMahjongRoom::sendHand(int seat) {
		ZhejiangMahjongAvatar* avatar = dynamic_cast<ZhejiangMahjongAvatar*>(getAvatar(seat).get());
		if (avatar == nullptr)
			return;
		MsgZhejiangMahjongHandCard msg;
		msg.hand = avatar->getHand();
		msg.send(avatar->getSession());
	}

	void ZhejiangMahjongRoom::addRobot(time_t nowTick) {
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
				InfoS << "浙江麻将补位：机器人(Id:" << robotPlayerId << ")进入房间(Id:" << getId()
					<< ")，当前人数:" << getAvatarCount();
				return;
			}
		}
	}
}
