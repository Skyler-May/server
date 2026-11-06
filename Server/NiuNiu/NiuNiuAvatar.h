// NiuNiuAvatar.h
// 经典牛牛玩家替身

#ifndef _NIU_MA_NIU_NIU_AVATAR_H_
#define _NIU_MA_NIU_NIU_AVATAR_H_

#include "PokerGenre.h"
#include "PokerAvatar.h"

namespace NiuMa
{
	class NiuNiuAvatar : public PokerAvatar
	{
	public:
		NiuNiuAvatar(const PokerRule::Ptr& rule, const std::string& playerId, int seat, bool robot);
		virtual ~NiuNiuAvatar();

	public:
		virtual void clear() override;

	protected:
		virtual void combineAllGenres() override;
		virtual void candidateCombinationsImpl(int situation = 0) override;
		virtual void candidateCombinationsImpl(const PokerGenre& pg, int situation = 0) override;
	};
}

#endif // !_NIU_MA_NIU_NIU_AVATAR_H_
