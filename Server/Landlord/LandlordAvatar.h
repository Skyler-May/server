// LandlordAvatar.h
// 斗地主玩家替身

#ifndef _NIU_MA_LANDLORD_AVATAR_H_
#define _NIU_MA_LANDLORD_AVATAR_H_

#include "PokerGenre.h"
#include "PokerAvatar.h"

namespace NiuMa
{
	class LandlordRule;
	class LandlordAvatar : public PokerAvatar
	{
	public:
		LandlordAvatar(const std::shared_ptr<LandlordRule>& rule, const std::string& playerId, int seat, bool robot);
		virtual ~LandlordAvatar();

	public:
		// 清理
		virtual void clear() override;

	protected:
		// 检索所有牌型的全部组合（AI 使用，暂未实现）
		virtual void combineAllGenres() override;

		// 更新候选组合列表，首位出牌
		virtual void candidateCombinationsImpl(int situation = 0) override;

		// 更新候选组合列表，压牌出牌
		virtual void candidateCombinationsImpl(const PokerGenre& pg, int situation = 0) override;
	};
}

#endif // !_NIU_MA_LANDLORD_AVATAR_H_
