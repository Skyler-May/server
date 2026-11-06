// PaoDeKuaiAvatar.h
// 斗地主玩家替身

#ifndef _NIU_MA_PAO_DE_KUAI_AVATAR_H_
#define _NIU_MA_PAO_DE_KUAI_AVATAR_H_

#include "PokerGenre.h"
#include "PokerAvatar.h"

namespace NiuMa
{
	class PaoDeKuaiRule;
	class PaoDeKuaiAvatar : public PokerAvatar
	{
	public:
		PaoDeKuaiAvatar(const std::shared_ptr<PaoDeKuaiRule>& rule, const std::string& playerId, int seat, bool robot);
		virtual ~PaoDeKuaiAvatar();

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

#endif // !_NIU_MA_PAO_DE_KUAI_AVATAR_H_
