// LandlordMessages.h
// 斗地主游戏网络消息

#ifndef _NIU_MA_LANDLORD_MESSAGES_H_
#define _NIU_MA_LANDLORD_MESSAGES_H_

#include "Game/GameMessages.h"
#include "PokerCard.h"

namespace NiuMa
{
	class LandlordMessages
	{
	private:
		LandlordMessages() {}

	public:
		virtual ~LandlordMessages() {}

		static void registMessages();
	};

	/**
	 * 请求同步斗地主数据消息
	 * 客户端->服务器
	 */
	class MsgLandlordSync : public MsgVenueInner {
	public:
		MsgLandlordSync() {}
		virtual ~MsgLandlordSync() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSGPACK_DEFINE_MAP(playerId, timestamp, nonce, signature, venueId);
	};

	/**
	 * 响应同步斗地主数据消息
	 * 服务器->客户端
	 */
	class MsgLandlordSyncResp : public MsgBase
	{
	public:
		MsgLandlordSyncResp();
		virtual ~MsgLandlordSyncResp() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(number, diZhu, seat, landlord, current, gameState, handCards, handCardNums, playedCards, playedSeat, scores);

	public:
		// 房号
		std::string number;

		// 底分
		int diZhu;

		// 自己的座位号
		int seat;

		// 地主座位号
		int landlord;

		// 当前出牌座位号
		int current;

		// 游戏状态
		int gameState;

		// 自己手牌
		CardArray handCards;

		// 所有玩家的手牌数量
		int handCardNums[3];

		// 当前桌面上的牌
		CardArray playedCards;

		// 桌面牌所属座位
		int playedSeat;

		// 各玩家积分
		int scores[3];
	};

	/**
	 * 发牌通知
	 * 服务器->客户端
	 */
	class MsgLandlordDealCard : public MsgBase
	{
	public:
		MsgLandlordDealCard();
		virtual ~MsgLandlordDealCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(landlord, bottomCards);

	public:
		// 地主座位号
		int landlord;

		// 底牌
		CardArray bottomCards;
	};

	/**
	 * 自己的手牌
	 * 服务器->客户端
	 */
	class MsgLandlordHandCard : public MsgBase
	{
	public:
		MsgLandlordHandCard();
		virtual ~MsgLandlordHandCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(cards);

	public:
		// 手牌
		CardArray cards;
	};

	/**
	 * 通知某座位轮到自己出牌
	 * 服务器->客户端
	 */
	class MsgLandlordTurn : public MsgBase
	{
	public:
		MsgLandlordTurn();
		virtual ~MsgLandlordTurn() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(seat, mustPlay, second);

	public:
		// 出牌座位号
		int seat;

		// 是否必须出牌（新一轮首出）
		bool mustPlay;

		// 剩余秒数
		int second;
	};

	/**
	 * 出牌请求
	 * 客户端->服务器
	 */
	class MsgLandlordPlayCard : public MsgVenueInner
	{
	public:
		MsgLandlordPlayCard();
		virtual ~MsgLandlordPlayCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

	public:
		// 要打出的牌id列表（空表示不出）
		std::vector<int> cardIds;

		MSGPACK_DEFINE_MAP(playerId, timestamp, nonce, signature, venueId, cardIds);
	};

	/**
	 * 出牌结果
	 * 服务器->客户端
	 */
	class MsgLandlordPlayCardResp : public MsgBase
	{
	public:
		MsgLandlordPlayCardResp();
		virtual ~MsgLandlordPlayCardResp() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(seat, success, passed, errMsg, cards, leftNums);

	public:
		// 座位号
		int seat;

		// 是否成功
		bool success;

		// 是否不出
		bool passed;

		// 错误消息
		std::string errMsg;

		// 打出的牌
		CardArray cards;

		// 剩余手牌数
		int leftNums;
	};

	/**
	 * 一局结束
	 * 服务器->客户端
	 */
	class MsgLandlordGameOver : public MsgBase
	{
	public:
		MsgLandlordGameOver();
		virtual ~MsgLandlordGameOver() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(winner, landlordWin, deltas);

	public:
		// 赢家座位号（-1 表示流局）
		int winner;

		// 地主是否获胜
		bool landlordWin;

		// 各家积分变化
		int deltas[3];
	};
}

#endif // !_NIU_MA_LANDLORD_MESSAGES_H_
