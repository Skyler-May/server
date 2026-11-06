// NiuNiuAvatar.cpp

#include "NiuNiuAvatar.h"

namespace NiuMa
{
	NiuNiuAvatar::NiuNiuAvatar(const PokerRule::Ptr& rule, const std::string& playerId, int seat, bool robot)
		: PokerAvatar(rule, playerId, seat, robot)
	{}

	NiuNiuAvatar::~NiuNiuAvatar() {}

	void NiuNiuAvatar::clear() {
		PokerAvatar::clear();
	}

	void NiuNiuAvatar::combineAllGenres() {
		// 牛牛不需要组合分析（比牌由服务器直接判定）
	}

	void NiuNiuAvatar::candidateCombinationsImpl(int situation) {
		(void)situation;
	}

	void NiuNiuAvatar::candidateCombinationsImpl(const PokerGenre& pg, int situation) {
		(void)pg;
		(void)situation;
	}
}
