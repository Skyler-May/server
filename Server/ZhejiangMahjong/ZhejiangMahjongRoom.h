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
		bool discardTile(int seat, int tile, std::string& errMsg);
		// 检查某座位是否可胡（hand 已含刚摸到/打出的牌）
		bool checkHuFor(int seat, bool zimo, int fromSeat, int& fan, int& score, std::string& desc);
		// 出牌后检查其他家点炮
		bool checkRobHu(int fromSeat);
		void settle(int winner, bool zimo, int fromSeat, int fan, int score, const std::string& desc);
		void onSync(const NetMessage::Ptr& netMsg);
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
		bool _waitingDiscard;			// 是否在等待某人出牌（用于超时托管）
		std::shared_ptr<ZhejiangHuChecker> _checker;
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_ROOM_H_
