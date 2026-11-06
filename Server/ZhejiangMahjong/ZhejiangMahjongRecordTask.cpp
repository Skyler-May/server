// ZhejiangMahjongRecordTask.cpp

#include "ZhejiangMahjongRecordTask.h"

#include <sstream>

namespace NiuMa
{
	ZhejiangMahjongRecordTask::ZhejiangMahjongRecordTask()
		: _roundNo(0)
		, _banker(0)
		, _winner(-1)
		, _zimo(0)
		, _fromSeat(-1)
		, _fan(0)
	{
		for (int i = 0; i < 4; i++)
			_deltas[i] = 0;
	}

	ZhejiangMahjongRecordTask::~ZhejiangMahjongRecordTask() {}

	MysqlQueryTask::QueryType ZhejiangMahjongRecordTask::buildQuery(std::string& sql) {
		std::stringstream ss;
		ss << "insert into `game_zhejiang_mahjong_record`"
			" (`venue_id`, `round_no`, `banker`, `winner`, `zimo`, `from_seat`, `fan`, `fan_desc`,"
			" `player_id0`, `player_id1`, `player_id2`, `player_id3`,"
			" `delta0`, `delta1`, `delta2`, `delta3`, `time`) values (\""
			<< _venueId << "\", " << _roundNo << ", " << _banker << ", " << _winner << ", "
			<< _zimo << ", " << _fromSeat << ", " << _fan << ", \"" << _desc << "\"";
		for (int i = 0; i < 4; i++)
			ss << ", \"" << _playerIds[i] << "\"";
		for (int i = 0; i < 4; i++)
			ss << ", " << _deltas[i];
		ss << ", now())";
		sql = ss.str();
		return MysqlQueryTask::QueryType::Insert;
	}
}
