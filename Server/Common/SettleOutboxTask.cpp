// SettleOutboxTask.cpp

#include "SettleOutboxTask.h"

#include <sstream>

namespace NiuMa
{
	SettleOutboxTask::SettleOutboxTask() {}
	SettleOutboxTask::~SettleOutboxTask() {}

	MysqlQueryTask::QueryType SettleOutboxTask::buildQuery(std::string& sql) {
		if (_entries.empty()) {
			sql.clear();
			return MysqlQueryTask::QueryType::Insert;
		}
		std::stringstream ss;
		ss << "insert into `game_settle_outbox`"
			" (`venue_id`, `round_no`, `game_type`, `player_id`, `delta`, `status`, `created_at`) values ";
		for (size_t i = 0; i < _entries.size(); i++) {
			if (i > 0)
				ss << ", ";
			ss << "(\"" << _venueId << "\", " << _roundNo << ", " << _gameType << ", \""
				<< _entries[i].playerId << "\", " << _entries[i].delta << ", 'pending', now())";
		}
		sql = ss.str();
		return MysqlQueryTask::QueryType::Insert;
	}
}
