// ZhejiangMahjongLoader.cpp

#include "Base/Log.h"
#include "ZhejiangMahjongLoader.h"
#include "ZhejiangMahjongRoom.h"
#include "../GameDefines.h"
#include "MySql/MysqlPool.h"

#include <mysql/jdbc.h>

namespace NiuMa
{
	ZhejiangMahjongLoader::ZhejiangMahjongLoader()
		: VenueLoader(static_cast<int>(GameType::ZhejiangMahjong))
	{}

	ZhejiangMahjongLoader::~ZhejiangMahjongLoader() {}

	Venue::Ptr ZhejiangMahjongLoader::load(const std::string& id) {
		class Task : public MysqlQueryTask
		{
		public:
			Task(const std::string& id) : _venueId(id), _baseScore(1) {}
			virtual ~Task() {}

		public:
			virtual QueryType buildQuery(std::string& sql) override {
				sql = "select `number`, `base_score` from `game_zhejiang_mahjong` where `venue_id` = \"" + _venueId + "\"";
				return QueryType::Select;
			}

			virtual int fetchResult(sql::ResultSet* res) override {
				int rows = 0;
				while (res->next()) {
					_number = res->getString("number");
					_baseScore = res->getInt("base_score");
					rows++;
				}
				return rows;
			}

		public:
			const std::string _venueId;
			std::string _number;
			int _baseScore;
		};

		std::shared_ptr<Task> task = std::make_shared<Task>(id);
		MysqlPool::getSingleton().syncQuery(task);
		if (!task->getSucceed()) {
			ErrorS << "加载浙江麻将房间(Id: " << id << ")失败";
			return nullptr;
		}
		return std::make_shared<ZhejiangMahjongRoom>(id, task->_number, task->_baseScore);
	}
}
