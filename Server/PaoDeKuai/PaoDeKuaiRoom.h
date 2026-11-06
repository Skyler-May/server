// PaoDeKuaiRoom.h
// 跑得快房间（3 人）

#ifndef _NIU_MA_PAO_DE_KUAI_ROOM_H_
#define _NIU_MA_PAO_DE_KUAI_ROOM_H_

#include "Game/GameRoom.h"
#include "PokerDealer.h"

namespace NiuMa
{
	class PaoDeKuaiRule;
	class PaoDeKuaiAvatar;

	class PaoDeKuaiRoom : public GameRoom
	{
	public:
		PaoDeKuaiRoom(const std::shared_ptr<PaoDeKuaiRule>& rule,
			const std::string& venueId, const std::string& number, int diZhu);
		virtual ~PaoDeKuaiRoom();

	public:
		// 房间状态
		enum class GameState : int
		{
			Waiting,		// 等待玩家准备
			Dealing,		// 发牌中
			Playing,		// 出牌中
			Settled			// 本局已结算
		};

	public:
		virtual void initialize() override;
		virtual void onTimer() override;
		virtual bool onMessage(const NetMessage::Ptr& netMsg) override;
		// 真人顶替机器人只在两局之间允许
		virtual bool canReplaceRobot() const override;

	protected:
		virtual GameAvatar::Ptr createAvatar(const std::string& playerId, int seat, bool robot) const override;
		virtual bool checkEnter(const std::string& playerId, std::string& errMsg, bool robot = false) const override;
		virtual int checkLeave(const std::string& playerId, std::string& errMsg) const override;
		virtual void clean() override;
		virtual bool beforeDestroy(bool silence) override;

	private:

		// 最近一局的分数变化（用于战绩落库）
		int _lastDeltas[5];

		// 本房间局号
		int _roundNo = 0;
		// 开始一局：发牌
		void beginDeal();

		// 进入出牌阶段
		void beginPlay();

		// 通知某座位出牌
		void notifyTurn(int seat);

		// 处理出牌
		void onPlayCard(const NetMessage::Ptr& netMsg);

		// 处理同步请求
		void onSync(const NetMessage::Ptr& netMsg);

		// 玩家出牌（内部实现）
		bool doPlayCard(int seat, const std::vector<int>& cardIds, std::string& errMsg, bool& passed);

		// 超时/机器人自动出牌
		void autoPlay(int seat);

		// 一人出完牌，结算
		void settle(int winner);

		// 给指定座位补位机器人
		void addRobot(time_t nowTick);

		// 刷新各替身的 session（以玩家当前连接为准）
		void refreshSessions();

		// 给某个玩家发送自己的手牌
		void sendHandCard(PaoDeKuaiAvatar* avatar);

		// 广播剩余手牌数量
		void notifyCardNums();

	private:
		const std::shared_ptr<PaoDeKuaiRule> _rule;
		PokerDealer _dealer;
		std::string _number;
		int _diZhu;
		GameState _gameState;
		time_t _stateTick;
		time_t _robotTick;

		// 地主座位
		int _landlord;

		// 当前该谁出牌
		int _current;

		// 桌面上的牌
		CardArray _playedCards;

		// 桌面牌所属座位
		int _playedSeat;

		// 当前这一手必须出牌（新一轮首出）
		bool _mustPlay;

		// 本轮已连续不出的人数
		int _passCount;

		// 各家积分
		int _scores[3];

		// 各家手牌数
		int _cardNums[3];
	};
}

#endif // !_NIU_MA_PAO_DE_KUAI_ROOM_H_
