// LandlordLoader.cpp

#include "Base/Log.h"
#include "LandlordLoader.h"
#include "LandlordRoom.h"
#include "../GameDefines.h"
#include "MySql/MysqlPool.h"

#include <mysql/jdbc.h>

namespace NiuMa
{
	LandlordLoader::LandlordLoader()
		: VenueLoader(static_cast<int>(GameType::Landlord))
	{
		_rule = std::make_shared<LandlordRule>();
	}

	LandlordLoader::~LandlordLoader() {}

	Venue::Ptr LandlordLoader::load(const std::string& id) {
		class LandlordTask : public MysqlQueryTask
		{
		public:
			LandlordTask(const std::string& id)
				: _venueId(id)
				, _diZhu(0)
			{}

			virtual ~LandlordTask() {}

		public:
			virtual QueryType buildQuery(std::string& sql) override {
				sql = "select `number`, `di_zhu` from `game_landlord` where `venue_id` = \"" + _venueId + "\"";
				return QueryType::Select;
			}

			virtual int fetchResult(sql::ResultSet* res) override {
				int rows = 0;
				while (res->next()) {
					_number = res->getString("number");
					_diZhu = res->getInt("di_zhu");
					rows++;
				}
				return rows;
			}

		public:
			// 场地id
			const std::string _venueId;

			// 房号
			std::string _number;

			// 底分
			int _diZhu;
		};

		std::shared_ptr<LandlordTask> task = std::make_shared<LandlordTask>(id);
		MysqlPool::getSingleton().syncQuery(task);
		if (!task->getSucceed()) {
			ErrorS << "加载斗地主房间(Id: " << id << ")失败";
			return nullptr;
		}
		std::shared_ptr<LandlordRoom> room = std::make_shared<LandlordRoom>(_rule, id, task->_number, task->_diZhu);
		return room;
	}
}
