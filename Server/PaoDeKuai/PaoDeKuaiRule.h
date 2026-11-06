// PaoDeKuaiRule.h
// 跑得快规则：牌型判定（单/对/三张/三带一/三带二/顺子/连对/炸弹），48 张牌（无王）

#ifndef _NIU_MA_PAO_DE_KUAI_RULE_H_
#define _NIU_MA_PAO_DE_KUAI_RULE_H_

#include "DouDiZhuRule.h"

namespace NiuMa
{
	// 跑得快牌型
	enum class PaoDeKuaiGenre : int
	{
		Invalid = 0,	// 无效牌型
		Single,			// 单张
		Pair,			// 对子
		Triple,			// 三张
		Triple1,		// 三带一
		Triple2,		// 三带二
		Straight,		// 顺子(5张以上连续单牌)
		StraightPair,	// 连对(2对以上连续对子)
		Bomb,			// 炸弹(四张同点)
		Total
	};

	/**
	 * 跑得快规则
	 */
	class PaoDeKuaiRule : public DouDiZhuRule
	{
	public:
		PaoDeKuaiRule();
		virtual ~PaoDeKuaiRule();

	public:
		// 排除的牌：跑得快没有大小王
		virtual bool isDisapprovedCard(const PokerCard& c) const override;

		// 炸弹顺序（只有四张炸弹）
		virtual int getBombOrder(int genre) const override;

		// 获得顺序表某一位置对应的炸弹牌型
		virtual int getBombByOrder(int order) const override;

		// 判定牌型
		virtual int predicateCardGenre(PokerGenre& pcg) const override;
	};
}

#endif // !_NIU_MA_PAO_DE_KUAI_RULE_H_
