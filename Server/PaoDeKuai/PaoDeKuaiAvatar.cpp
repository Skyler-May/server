// PaoDeKuaiAvatar.cpp

#include "PaoDeKuaiAvatar.h"
#include "PaoDeKuaiRule.h"

namespace NiuMa
{
	PaoDeKuaiAvatar::PaoDeKuaiAvatar(const std::shared_ptr<PaoDeKuaiRule>& rule, const std::string& playerId, int seat, bool robot)
		: PokerAvatar(rule, playerId, seat, robot)
	{}

	PaoDeKuaiAvatar::~PaoDeKuaiAvatar() {}

	void PaoDeKuaiAvatar::clear() {
		PokerAvatar::clear();
	}

	void PaoDeKuaiAvatar::combineAllGenres() {
		// 暂不提供 AI 组合，后续接入网狐 AndroidUserItemSink 的算法时在此实现
	}

	void PaoDeKuaiAvatar::candidateCombinationsImpl(int situation) {
		(void)situation;
	}

	void PaoDeKuaiAvatar::candidateCombinationsImpl(const PokerGenre& pg, int situation) {
		(void)pg;
		(void)situation;
	}
}
