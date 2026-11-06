// ZhejiangMahjongRoom.h
// 浙江麻将房间（4 人，136 张，财神制）

#ifndef _NIU_MA_ZHEJIANG_MAHJONG_ROOM_H_
#define _NIU_MA_ZHEJIANG_MAHJONG_ROOM_H_

#include "Game/GameRoom.h"

#include <deque>
#include <vector>

namespace NiuMa
{
	class ZhejiangMahjongAvatar;
	class ZhejiangHuChecker;

	class ZhejiangMahjongRoom : public GameRoom
	{
	public:
		ZhejiangMahjongRoom(const std::string& venueId, const std::string& number, int baseScore);
		virtual ~ZhejiangMahjongRoom();

	public:
		enum class GameState : int
		{
			Waiting,
			Dealing,
			Playing,
			Settled
		};

	public:
		virtual void initialize() override;
		virtual void onTimer() override;
		virtual bool onMessage(const NetMessage::Ptr& netMsg) override;
		virtual bool canReplaceRobot() const override;

	protected:
		virtual GameAvatar::Ptr createAvatar(const std::string& playerId, int seat, bool robot) const override;
		virtual bool checkEnter(const std::string& playerId, std::string& errMsg, bool robot = false) const override;
		virtual int checkLeave(const std::string& playerId, std::string& errMsg) const override;
		virtual void clean() override;
		virtual bool beforeDestroy(bool silence) override;

	private:
		void beginDeal();
		void beginPlay();
		void drawTile(int seat);
		void waitDiscard(int seat);
		void onDiscard(const NetMessage::Ptr& netMsg);
		void autoDiscard(int seat);

		// 选择要打出的牌（孤张优先、留搭子，财神不打）
		int pickDiscardTile(int seat) const;
		bool discardTile(int seat, int tile, std::string& errMsg);
		// 检查某座位是否可胡（zimo=true 用摸到的牌，否则用 winTile 点炮）
		bool checkHuFor(int seat, bool zimo, int fromSeat, int winTile, int& fan, int& score, std::string& desc);
		// 出牌后检查其他家点炮
		bool checkRobHu(int fromSeat);

		// 出牌后询问其他家是否碰/杠（带超时与优先级裁决）
		bool beginActionCheck(int fromSeat);

		// 执行碰 / 明杠
		void applyPong(int seat, int tile, int fromSeat);
		void applyGang(int seat, int tile, int fromSeat);

		// 裁决所有应答
		void resolveActions();

		// 处理玩家动作应答
		void onActionResp(const NetMessage::Ptr& netMsg);

		// 出牌后检查其他家明杠
		bool checkGang(int fromSeat);

		// 摸牌后检查自己是否可以暗杠（返回 true 表示杠了，需要补摸）
		bool tryConcealedKong(int seat);

		// 补杠：碰过的牌又摸到第 4 张
		bool tryUpgradeKong(int seat);

		// 一炮多响结算
		void settleMulti(const std::vector<int>& winners, int fromSeat);
		void settle(int winner, bool zimo, int fromSeat, int fan, int score, const std::string& desc);
		void onSync(const NetMessage::Ptr& netMsg);

		// 战绩落库
		void saveRoundRecord(int winner, bool zimo, int fromSeat, int fan, const std::string& desc);
		void sendHand(int seat);
		void addRobot(time_t nowTick);
		void refreshSessions();
		int nextSeat(int seat) const;

	private:
		std::string _number;
		int _baseScore;
		GameState _gameState;
		time_t _stateTick;
		time_t _robotTick;
		int _dealer;
		int _current;					// 当前该出牌的人
		int _kingCard;					// 财神牌（0 表示本局无财神）
		std::vector<int> _wall;			// 牌墙（已洗）
		std::vector<int> _scores;		// 各座位累计分
		int _roundNo;					// 局号
		int _lastDeltas[4];				// 最近一局的分数变化
		bool _waitingDiscard;			// 是否在等待某人出牌（用于超时托管）
		int _lastDiscardTile;			// 最近打出的牌
		int _lastDiscardSeat;			// 最近打牌的座位
		std::shared_ptr<ZhejiangHuChecker> _checker;

		// 待裁决的碰杠询问
		int _actionTile = 0;
		int _actionFromSeat = -1;
		int _actionOptions[4] = { 0, 0, 0, 0 };		// 位掩码：1碰 2杠
		int _actionChosen[4] = { -1, -1, -1, -1 };	// -1 未答 0 过
		time_t _actionDeadline = 0;
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_ROOM_H_
