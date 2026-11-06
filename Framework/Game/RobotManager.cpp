// RobotManager.cpp

#include "RobotManager.h"

#include "Base/BaseUtils.h"
#include "Base/Log.h"
#include "Player/PlayerManager.h"

#include <chrono>
#include <vector>

namespace NiuMa
{
	template<> RobotManager* Singleton<RobotManager>::_inst = nullptr;

	namespace {
		long long currentSeconds() {
			return static_cast<long long>(std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now().time_since_epoch()).count());
		}
	}

	bool RobotManager::request(std::string& playerId) {
		playerId = BaseUtils::EMPTY_STRING;
		std::lock_guard<std::mutex> lock(_mutex);
		// 先清理超时占用（默认 30 分钟）
		long long now = currentSeconds();
		std::map<std::string, long long>::iterator it = _occupied.begin();
		while (it != _occupied.end()) {
			if (now - it->second > 1800) {
				InfoS << "释放超时占用的机器人: " << it->first;
				it = _occupied.erase(it);
			}
			else {
				++it;
			}
		}
		int count = PlayerManager::getSingleton().getRobotCount();
		if (count <= 0)
			return false;
		// 随机尝试若干次，避免总是撞到已占用的机器人
		const int maxTry = 2 * count > 60 ? 60 : 2 * count;
		for (int i = 0; i < maxTry; i++) {
			int robotId = BaseUtils::randInt(0, count) + 1;
			std::string tmp;
			if (!PlayerManager::getSingleton().loadRobot(robotId, tmp))
				continue;
			if (tmp.empty())
				continue;
			if (_occupied.find(tmp) != _occupied.end())
				continue;
			_occupied.insert(std::make_pair(tmp, now));
			playerId = tmp;
			InfoS << "分配机器人: " << playerId << " (id=" << robotId << ")，当前占用: " << _occupied.size();
			return true;
		}
		return false;
	}

	void RobotManager::free(const std::string& playerId) {
		if (playerId.empty())
			return;
		std::lock_guard<std::mutex> lock(_mutex);
		std::map<std::string, long long>::iterator it = _occupied.find(playerId);
		if (it != _occupied.end()) {
			_occupied.erase(it);
			InfoS << "释放机器人: " << playerId << "，当前占用: " << _occupied.size();
		}
	}

	void RobotManager::cleanTimeout(int timeoutSeconds) {
		if (timeoutSeconds <= 0)
			return;
		std::lock_guard<std::mutex> lock(_mutex);
		long long now = currentSeconds();
		std::map<std::string, long long>::iterator it = _occupied.begin();
		while (it != _occupied.end()) {
			if (now - it->second > timeoutSeconds)
				it = _occupied.erase(it);
			else
				++it;
		}
	}
}
