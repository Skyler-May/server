// NiuNiuLoader.cpp

#include "Base/Log.h"
#include "NiuNiuLoader.h"
#include "NiuNiuRoom.h"
#include "../GameDefines.h"
#include "MySql/MysqlPool.h"

#include <mysql/jdbc.h>

namespace NiuMa
{
	NiuNiuLoader::NiuNiuLoader()
		: VenueLoader(static_cast<int>(GameType::NiuNiu))
	{
		// false = 经典牛牛（支持顺子牛/五花牛等）
		_rule = std::make_shared<NiuNiuRule>(false);
	}

	NiuNiuLoader::~NiuNiuLoader() {}

	Venue::Ptr NiuNiuLoader::load(const std::string& id) {
		class NiuNiuTask : public MysqlQueryTask
		{
		public:
			NiuNiuTask(const std::string& id)
				: _venueId(id)
				, _diZhu(0)
			{}

			virtual ~NiuNiuTask() {}

		public:
			virtual QueryType buildQuery(std::string& sql) override {
				sql = "select `number`, `di_zhu` from `game_niu_niu` where `venue_id` = \"" + _venueId + "\"";
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
			const std::string _venueId;
			std::string _number;
			int _diZhu;
		};

		std::shared_ptr<NiuNiuTask> task = std::make_shared<NiuNiuTask>(id);
		MysqlPool::getSingleton().syncQuery(task);
		if (!task->getSucceed()) {
			ErrorS << "加载牛牛房间(Id: " << id << ")失败";
			return nullptr;
		}
		std::shared_ptr<NiuNiuRoom> room = std::make_shared<NiuNiuRoom>(_rule, id, task->_number, task->_diZhu);
		return room;
	}
}
