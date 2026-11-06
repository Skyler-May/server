// SettleOutboxTask.h
// 结算出站表（outbox）：每局每个玩家的分数变化写一行，供聚合平台消费入账
//
// 表结构 game_settle_outbox：
//   id, venue_id, round_no, game_type, player_id, delta, status(pending/settled/failed),
//   created_at, settled_at, remark
// 幂等键：unique(venue_id, round_no, player_id)

#ifndef _NIU_MA_SETTLE_OUTBOX_TASK_H_
#define _NIU_MA_SETTLE_OUTBOX_TASK_H_

#include "MySql/MysqlQueryTask.h"

#include <string>
#include <vector>

namespace NiuMa
{
	struct SettleEntry
	{
		std::string playerId;
		int delta = 0;
	};

	class SettleOutboxTask : public MysqlQueryTask
	{
	public:
		SettleOutboxTask();
		virtual ~SettleOutboxTask();

	public:
		virtual QueryType buildQuery(std::string& sql) override;

	public:
		std::string _venueId;
		int _roundNo = 0;
		int _gameType = 0;
		std::vector<SettleEntry> _entries;
	};
}

#endif // !_NIU_MA_SETTLE_OUTBOX_TASK_H_
