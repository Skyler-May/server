// PaoDeKuaiLoader.cpp

#include "Base/Log.h"
#include "PaoDeKuaiLoader.h"
#include "PaoDeKuaiRoom.h"
#include "../GameDefines.h"
#include "MySql/MysqlPool.h"

#include <mysql/jdbc.h>

namespace NiuMa
{
	PaoDeKuaiLoader::PaoDeKuaiLoader()
		: VenueLoader(static_cast<int>(GameType::PaoDeKuai))
	{
		_rule = std::make_shared<PaoDeKuaiRule>();
	}

	PaoDeKuaiLoader::~PaoDeKuaiLoader() {}

	Venue::Ptr PaoDeKuaiLoader::load(const std::string& id) {
		class PaoDeKuaiTask : public MysqlQueryTask
		{
		public:
			PaoDeKuaiTask(const std::string& id)
				: _venueId(id)
				, _diZhu(0)
			{}

			virtual ~PaoDeKuaiTask() {}

		public:
			virtual QueryType buildQuery(std::string& sql) override {
				sql = "select `number`, `di_zhu` from `game_pao_de_kuai` where `venue_id` = \"" + _venueId + "\"";
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

		std::shared_ptr<PaoDeKuaiTask> task = std::make_shared<PaoDeKuaiTask>(id);
		MysqlPool::getSingleton().syncQuery(task);
		if (!task->getSucceed()) {
			ErrorS << "加载跑得快房间(Id: " << id << ")失败";
			return nullptr;
		}
		std::shared_ptr<PaoDeKuaiRoom> room = std::make_shared<PaoDeKuaiRoom>(_rule, id, task->_number, task->_diZhu);
		return room;
	}
}
