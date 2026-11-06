// LandlordRule.h
// 斗地主（欢乐斗地主类玩法）规则：牌型判定 + 炸弹顺序
// 移植自网狐斗地主 GameLogic 的牌型语义，按 NiuMa 的 PokerRule 体系重写

#ifndef _NIU_MA_LANDLORD_RULE_H_
#define _NIU_MA_LANDLORD_RULE_H_

#include "DouDiZhuRule.h"

namespace NiuMa
{
	// 斗地主牌型
	enum class LandlordGenre : int
	{
		Invalid = 0,	// 无效牌型
		Single,			// 单张
		Pair,			// 对子
		Triple,			// 三条
		Triple1,		// 三带一
		Triple2,		// 三带二
		Straight,		// 顺子(5张以上连续单牌)
		StraightPair,	// 连对(3对以上连续对子)
		Bomb,			// 炸弹
		Rocket,			// 火箭(双王)
		Total
	};

	/**
	 * 斗地主规则
	 */
	class LandlordRule : public DouDiZhuRule
	{
	public:
		LandlordRule();
		virtual ~LandlordRule();

	public:
		// 判断牌型是否为炸弹牌型
		virtual int getBombOrder(int genre) const override;

		// 获得顺序表某一位置对应的炸弹牌型
		virtual int getBombByOrder(int order) const override;

		// 判定牌型（把牌数组识别为具体牌型，并设置主牌）
		virtual int predicateCardGenre(PokerGenre& pcg) const override;
	};
}

#endif // !_NIU_MA_LANDLORD_RULE_H_
