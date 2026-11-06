// RobotManager.h
// Author wujian
// Email 393817707@qq.com
// Date 2025.02.17

#ifndef _NIU_MA_ROBOT_MANAGER_H_
#define _NIU_MA_ROBOT_MANAGER_H_

#include "Base/Singleton.h"

#include <string>
#include <map>
#include <mutex>

namespace NiuMa
{
	class RobotManager : public Singleton<RobotManager> {
	private:
		RobotManager() = default;

	public:
		virtual ~RobotManager() = default;

		friend class Singleton<RobotManager>;

	public:
		/**
		 * 获取一个空闲的机器人id
		 * @param playerId 机器人玩家id
		 * @return 是否获取成功
		 */
		bool request(std::string& playerId);

		/**
		 * 释放一个机器人
		 * @param playerId 机器人玩家id
		 */
		void free(const std::string& playerId);

		/**
		 * 清理占用超时的机器人（防止异常情况下机器人被永久占用）
		 * @param timeoutSeconds 超时秒数
		 */
		void cleanTimeout(int timeoutSeconds = 1800);

	private:
		// 机器人占用表：playerId -> 占用时间戳（秒）
		std::map<std::string, long long> _occupied;
		std::mutex _mutex;
	};
}

#endif // !_NIU_MA_ROBOT_MANAGER_H_