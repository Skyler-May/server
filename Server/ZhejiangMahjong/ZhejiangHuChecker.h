// ZhejiangHuChecker.h
// 浙江麻将胡牌判定封装（基于网狐 GameLogic::GetHuInfo）
// 本类不依赖 NiuMa 框架，便于独立单测；牌值沿用网狐编码：
//   0x01-0x09 万子、0x11-0x19 条子、0x21-0x29 筒子、0x31-0x37 东南西北中发白

#ifndef _NIU_MA_ZHEJIANG_HU_CHECKER_H_
#define _NIU_MA_ZHEJIANG_HU_CHECKER_H_

#include "StdAfx.h"
#include "GameLogic.h"

#include <string>
#include <vector>

namespace NiuMa
{
	// 胡牌结果
	struct ZhejiangHuResult
	{
		bool hu = false;			// 是否可胡
		bool zimo = false;			// 是否自摸
		int fan = 0;				// 番数
		int baseScore = 0;			// 基础分（底分 1）
		int score = 0;				// 最终得分 = 底分 * 2^番
		int kingCount = 0;			// 手上财神张数
		std::string desc;			// 牌型描述（如 "平胡+自摸+无财神"）
		std::vector<std::string> fans;	// 番型明细

		void reset() {
			hu = false;
			zimo = false;
			fan = 0;
			baseScore = 0;
			score = 0;
			kingCount = 0;
			desc.clear();
			fans.clear();
		}
	};

	class ZhejiangHuChecker
	{
	public:
		ZhejiangHuChecker();
		virtual ~ZhejiangHuChecker();

	public:
		// 设置财神牌（0 表示无财神）
		void setKingCard(BYTE card);
		BYTE getKingCard() const;

		/**
		 * 判定是否可胡
		 * @param cards 手牌（网狐牌值，14 张含摸到的牌）
		 * @param currentCard 当前摸到/打出的牌
		 * @param zimo 是否自摸
		 * @param result 输出结果
		 * @return 是否可胡
		 */
		bool checkHu(const std::vector<BYTE>& cards, BYTE currentCard, bool zimo, ZhejiangHuResult& result, int meldCount = 0);

		// 计算番型与得分（checkHu 成功后调用）
		void calcFan(const std::vector<BYTE>& cards, BYTE cbCardIndex[MAX_INDEX],
			int jokers, bool zimo, ZhejiangHuResult& result);

		// 牌值转文字（便于日志）
		static std::string cardToText(BYTE card);

	private:
		// 把手牌转换为网狐的牌索引数组（每种牌的张数）
		static void buildCardIndex(const std::vector<BYTE>& cards, BYTE cbCardIndex[MAX_INDEX]);

		// 自己实现的"3n+2"平胡判定（网狐 IsNeat3 的组合枚举有缺陷，这里重写）
		static bool isNeat3(const BYTE cbCardIndex[MAX_INDEX]);

		// 支持财神（万能牌）的平胡判定；setsNeeded 为还需要拆出的面子数（4 - 已碰杠组数）
		static bool isNeat3WithJokers(const BYTE cbCardIndex[MAX_INDEX], int jokers, int setsNeeded = 4);

	private:
		mutable CGameLogic _logic;
		BYTE _kingCard;
	};
}

#endif // !_NIU_MA_ZHEJIANG_HU_CHECKER_H_
