// NiuNiuMessages.h
// 经典牛牛游戏网络消息

#ifndef _NIU_MA_NIU_NIU_MESSAGES_H_
#define _NIU_MA_NIU_NIU_MESSAGES_H_

#include "Game/GameMessages.h"
#include "PokerCard.h"

namespace NiuMa
{
	class NiuNiuMessages
	{
	private:
		NiuNiuMessages() {}

	public:
		virtual ~NiuNiuMessages() {}

		static void registMessages();
	};

	/** 请求同步牛牛数据（客户端->服务器） */
	class MsgNiuNiuSync : public MsgVenueInner
	{
	public:
		MsgNiuNiuSync() {}
		virtual ~MsgNiuNiuSync() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSGPACK_DEFINE_MAP(playerId, timestamp, nonce, signature, venueId);
	};

	/** 同步响应（服务器->客户端） */
	class MsgNiuNiuSyncResp : public MsgBase
	{
	public:
		MsgNiuNiuSyncResp();
		virtual ~MsgNiuNiuSyncResp() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(number, diZhu, seat, banker, gameState, handCards, scores);

	public:
		std::string number;
		int diZhu;
		int seat;
		int banker;
		int gameState;
		CardArray handCards;
		int scores[5];
	};

	/** 发牌通知 */
	class MsgNiuNiuDealCard : public MsgBase
	{
	public:
		MsgNiuNiuDealCard();
		virtual ~MsgNiuNiuDealCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(banker);

	public:
		int banker;
	};

	/** 自己的手牌 */
	class MsgNiuNiuHandCard : public MsgBase
	{
	public:
		MsgNiuNiuHandCard();
		virtual ~MsgNiuNiuHandCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(cards, genre, times);

	public:
		CardArray cards;
		int genre;		// 牌型(NiuNiuGenre)
		int times;		// 倍数
	};

	/** 比牌结果（逐座位） */
	class MsgNiuNiuCompare : public MsgBase
	{
	public:
		MsgNiuNiuCompare();
		virtual ~MsgNiuNiuCompare() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(seat, genre, times, cards);

	public:
		int seat;
		int genre;
		int times;
		CardArray cards;
	};

	/** 本局结算 */
	class MsgNiuNiuGameOver : public MsgBase
	{
	public:
		MsgNiuNiuGameOver();
		virtual ~MsgNiuNiuGameOver() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(winner, banker, deltas);

	public:
		int winner;			// 赢家座位
		int banker;			// 庄家座位
		int deltas[5];		// 各座位分数变化
	};
}

#endif // !_NIU_MA_NIU_NIU_MESSAGES_H_
