// SimpleRoundRecordTask.cpp

#include "SimpleRoundRecordTask.h"

#include <sstream>

namespace NiuMa
{
	SimpleRoundRecordTask::SimpleRoundRecordTask()
		: _roundNo(0)
		, _winner(-1)
	{
		for (int i = 0; i < 5; i++)
			_deltas[i] = 0;
	}

	SimpleRoundRecordTask::~SimpleRoundRecordTask() {}

	MysqlQueryTask::QueryType SimpleRoundRecordTask::buildQuery(std::string& sql) {
		std::stringstream ss;
		ss << "insert into `" << _table << "`"
			" (`venue_id`, `round_no`, `winner`, `desc`,"
			" `player_id0`, `player_id1`, `player_id2`, `player_id3`, `player_id4`,"
			" `delta0`, `delta1`, `delta2`, `delta3`, `delta4`, `time`) values (\""
			<< _venueId << "\", " << _roundNo << ", " << _winner << ", \"" << _desc << "\"";
		for (int i = 0; i < 5; i++)
			ss << ", \"" << _playerIds[i] << "\"";
		for (int i = 0; i < 5; i++)
			ss << ", " << _deltas[i];
		ss << ", now())";
		sql = ss.str();
		return MysqlQueryTask::QueryType::Insert;
	}
}
