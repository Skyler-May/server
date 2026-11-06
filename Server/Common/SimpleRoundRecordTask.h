// SimpleRoundRecordTask.h
// 通用牌局战绩落库任务（斗地主/牛牛/跑得快等牌类共用）
//
// 统一的表结构（每款游戏一张表，字段名一致）：
//   venue_id, round_no, winner, desc,
//   player_id0..4, delta0..4, time

#ifndef _NIU_MA_SIMPLE_ROUND_RECORD_TASK_H_
#define _NIU_MA_SIMPLE_ROUND_RECORD_TASK_H_

#include "MySql/MysqlQueryTask.h"

#include <string>

namespace NiuMa
{
	class SimpleRoundRecordTask : public MysqlQueryTask
	{
	public:
		SimpleRoundRecordTask();
		virtual ~SimpleRoundRecordTask();

	public:
		virtual QueryType buildQuery(std::string& sql) override;

	public:
		// 目标表名（由调用方指定，必须是白名单内的固定表，禁止外部输入）
		std::string _table;

		// 场地id
		std::string _venueId;

		// 局号
		int _roundNo;

		// 赢家座位（-1 表示流局/平局）
		int _winner;

		// 结果描述
		std::string _desc;

		// 各座位玩家id（最多 5 人）
		std::string _playerIds[5];

		// 各座位分数变化
		int _deltas[5];
	};
}

#endif // !_NIU_MA_SIMPLE_ROUND_RECORD_TASK_H_
