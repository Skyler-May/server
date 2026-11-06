// LandlordAvatar.cpp

#include "LandlordAvatar.h"
#include "LandlordRule.h"

namespace NiuMa
{
	LandlordAvatar::LandlordAvatar(const std::shared_ptr<LandlordRule>& rule, const std::string& playerId, int seat, bool robot)
		: PokerAvatar(rule, playerId, seat, robot)
	{}

	LandlordAvatar::~LandlordAvatar() {}

	void LandlordAvatar::clear() {
		PokerAvatar::clear();
	}

	void LandlordAvatar::combineAllGenres() {
		// 暂不提供 AI 组合，后续接入网狐 AndroidUserItemSink 的算法时在此实现
	}

	void LandlordAvatar::candidateCombinationsImpl(int situation) {
		(void)situation;
	}

	void LandlordAvatar::candidateCombinationsImpl(const PokerGenre& pg, int situation) {
		(void)pg;
		(void)situation;
	}
}
