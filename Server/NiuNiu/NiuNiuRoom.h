// NiuNiuRoom.h
// 经典牛牛房间

#ifndef _NIU_MA_NIU_NIU_ROOM_H_
#define _NIU_MA_NIU_NIU_ROOM_H_

#include "Game/GameRoom.h"
#include "PokerDealer.h"
#include "NiuNiuRule.h"

namespace NiuMa
{
	class NiuNiuAvatar;

	class NiuNiuRoom : public GameRoom
	{
	public:
		NiuNiuRoom(const std::shared_ptr<NiuNiuRule>& rule,
			const std::string& venueId, const std::string& number, int diZhu);
		virtual ~NiuNiuRoom();

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

		// 最近一局的分数变化（用于战绩落库）
		int _lastDeltas[5];

		// 本房间局号
		int _roundNo = 0;
		void beginDeal();
		void beginCompare();
		void settle(int winner);
		void onSync(const NetMessage::Ptr& netMsg);
		void addRobot(time_t nowTick);
		void refreshSessions();

	private:
		std::shared_ptr<NiuNiuRule> _rule;
		PokerDealer _dealer;
		std::string _number;
		int _diZhu;
		GameState _gameState;
		time_t _stateTick;
		time_t _robotTick;
		int _banker;
		int _scores[5];
		int _genres[5];
	};
}

#endif // !_NIU_MA_NIU_NIU_ROOM_H_
