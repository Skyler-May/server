// PaoDeKuaiMessages.h
// 跑得快游戏网络消息

#ifndef _NIU_MA_PAO_DE_KUAI_MESSAGES_H_
#define _NIU_MA_PAO_DE_KUAI_MESSAGES_H_

#include "Game/GameMessages.h"
#include "PokerCard.h"

namespace NiuMa
{
	class PaoDeKuaiMessages
	{
	private:
		PaoDeKuaiMessages() {}

	public:
		virtual ~PaoDeKuaiMessages() {}

		static void registMessages();
	};

	/**
	 * 请求同步斗地主数据消息
	 * 客户端->服务器
	 */
	class MsgPaoDeKuaiSync : public MsgVenueInner {
	public:
		MsgPaoDeKuaiSync() {}
		virtual ~MsgPaoDeKuaiSync() {}

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
	class MsgPaoDeKuaiSyncResp : public MsgBase
	{
	public:
		MsgPaoDeKuaiSyncResp();
		virtual ~MsgPaoDeKuaiSyncResp() {}

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
	class MsgPaoDeKuaiDealCard : public MsgBase
	{
	public:
		MsgPaoDeKuaiDealCard();
		virtual ~MsgPaoDeKuaiDealCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(landlord, bottomCards);

	public:
		// 地主座位号
		int landlord;

		// 底牌（跑得快无底牌，保留字段兼容）
		CardArray bottomCards;
	};

	/**
	 * 自己的手牌
	 * 服务器->客户端
	 */
	class MsgPaoDeKuaiHandCard : public MsgBase
	{
	public:
		MsgPaoDeKuaiHandCard();
		virtual ~MsgPaoDeKuaiHandCard() {}

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
	class MsgPaoDeKuaiTurn : public MsgBase
	{
	public:
		MsgPaoDeKuaiTurn();
		virtual ~MsgPaoDeKuaiTurn() {}

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
	class MsgPaoDeKuaiPlayCard : public MsgVenueInner
	{
	public:
		MsgPaoDeKuaiPlayCard();
		virtual ~MsgPaoDeKuaiPlayCard() {}

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
	class MsgPaoDeKuaiPlayCardResp : public MsgBase
	{
	public:
		MsgPaoDeKuaiPlayCardResp();
		virtual ~MsgPaoDeKuaiPlayCardResp() {}

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
	class MsgPaoDeKuaiGameOver : public MsgBase
	{
	public:
		MsgPaoDeKuaiGameOver();
		virtual ~MsgPaoDeKuaiGameOver() {}

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

#endif // !_NIU_MA_PAO_DE_KUAI_MESSAGES_H_
