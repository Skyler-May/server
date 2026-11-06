// ZhejiangMahjongRecordTask.h
// 浙江麻将一局战绩落库任务

#ifndef _NIU_MA_ZHEJIANG_MAHJONG_RECORD_TASK_H_
#define _NIU_MA_ZHEJIANG_MAHJONG_RECORD_TASK_H_

#include "MySql/MysqlQueryTask.h"

namespace NiuMa
{
	class ZhejiangMahjongRecordTask : public MysqlQueryTask
	{
	public:
		ZhejiangMahjongRecordTask();
		virtual ~ZhejiangMahjongRecordTask();

	public:
		virtual QueryType buildQuery(std::string& sql) override;

	public:
		// 局号
		int _roundNo;

		// 场地id
		std::string _venueId;

		// 庄家座位
		int _banker;

		// 赢家座位（-1 流局）
		int _winner;

		// 是否自摸
		int _zimo;

		// 点炮座位（-1 无）
		int _fromSeat;

		// 番数
		int _fan;

		// 牌型描述
		std::string _desc;

		// 各座位玩家id
		std::string _playerIds[4];

		// 各座位得分变化
		int _deltas[4];
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_RECORD_TASK_H_
