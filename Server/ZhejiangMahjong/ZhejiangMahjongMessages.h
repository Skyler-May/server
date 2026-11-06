// ZhejiangMahjongMessages.h
// 浙江麻将网络消息

#ifndef _NIU_MA_ZHEJIANG_MAHJONG_MESSAGES_H_
#define _NIU_MA_ZHEJIANG_MAHJONG_MESSAGES_H_

#include "Game/GameMessages.h"

#include <vector>

namespace NiuMa
{
	class ZhejiangMahjongMessages
	{
	private:
		ZhejiangMahjongMessages() {}

	public:
		virtual ~ZhejiangMahjongMessages() {}

		static void registMessages();
	};

	/** 请求同步（客户端->服务器） */
	class MsgZhejiangMahjongSync : public MsgVenueInner
	{
	public:
		MsgZhejiangMahjongSync() {}
		virtual ~MsgZhejiangMahjongSync() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSGPACK_DEFINE_MAP(playerId, timestamp, nonce, signature, venueId);
	};

	/** 同步响应（服务器->客户端） */
	class MsgZhejiangMahjongSyncResp : public MsgBase
	{
	public:
		MsgZhejiangMahjongSyncResp();
		virtual ~MsgZhejiangMahjongSyncResp() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(number, baseScore, seat, dealer, current, gameState, hand, handNums, kingCard, scores);

	public:
		std::string number;
		int baseScore;
		int seat;
		int dealer;
		int current;
		int gameState;
		std::vector<int> hand;			// 自己的手牌（网狐牌值）
		std::vector<int> handNums;		// 各家手牌数
		int kingCard;					// 财神牌
		std::vector<int> scores;
	};

	/** 发牌 */
	class MsgZhejiangMahjongDealCard : public MsgBase
	{
	public:
		MsgZhejiangMahjongDealCard();
		virtual ~MsgZhejiangMahjongDealCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(dealer, kingCard);

	public:
		int dealer;
		int kingCard;
	};

	/** 自己的手牌 */
	class MsgZhejiangMahjongHandCard : public MsgBase
	{
	public:
		MsgZhejiangMahjongHandCard();
		virtual ~MsgZhejiangMahjongHandCard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(hand);

	public:
		std::vector<int> hand;
	};

	/** 摸牌通知（只发给摸牌的人） */
	class MsgZhejiangMahjongDraw : public MsgBase
	{
	public:
		MsgZhejiangMahjongDraw();
		virtual ~MsgZhejiangMahjongDraw() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(seat, tile, leftNum);

	public:
		int seat;
		int tile;
		int leftNum;
	};

	/** 轮到某人出牌 */
	class MsgZhejiangMahjongTurn : public MsgBase
	{
	public:
		MsgZhejiangMahjongTurn();
		virtual ~MsgZhejiangMahjongTurn() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(seat, second);

	public:
		int seat;
		int second;
	};

	/** 出牌请求（客户端->服务器） */
	class MsgZhejiangMahjongDiscard : public MsgVenueInner
	{
	public:
		MsgZhejiangMahjongDiscard();
		virtual ~MsgZhejiangMahjongDiscard() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSGPACK_DEFINE_MAP(playerId, timestamp, nonce, signature, venueId, tile);

	public:
		int tile;
	};

	/** 出牌广播 */
	class MsgZhejiangMahjongDiscardResp : public MsgBase
	{
	public:
		MsgZhejiangMahjongDiscardResp();
		virtual ~MsgZhejiangMahjongDiscardResp() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(seat, tile, success, errMsg);

	public:
		int seat;
		int tile;
		bool success;
		std::string errMsg;
	};

	/** 结算 */
	class MsgZhejiangMahjongGameOver : public MsgBase
	{
	public:
		MsgZhejiangMahjongGameOver();
		virtual ~MsgZhejiangMahjongGameOver() {}

		static const std::string TYPE;

		virtual const std::string& getType() const {
			return TYPE;
		}

		MSG_PACK_IMPL

		MSGPACK_DEFINE_MAP(winner, zimo, fromSeat, fan, score, desc, deltas);

	public:
		int winner;			// 赢家座位
		bool zimo;			// 是否自摸
		int fromSeat;		// 点炮者座位（自摸为 -1）
		int fan;			// 番数
		int score;			// 得分
		std::string desc;	// 番型描述
		std::vector<int> deltas;
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_MESSAGES_H_
