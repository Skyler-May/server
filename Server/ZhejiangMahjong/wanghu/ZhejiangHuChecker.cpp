// ZhejiangHuChecker.cpp

#include "ZhejiangHuChecker.h"

#include <cstdio>

namespace NiuMa
{
	ZhejiangHuChecker::ZhejiangHuChecker()
		: _kingCard(0)
	{}

	ZhejiangHuChecker::~ZhejiangHuChecker() {}

	void ZhejiangHuChecker::setKingCard(BYTE card) {
		_kingCard = card;
		_logic.SetKingCardData(card);
	}

	BYTE ZhejiangHuChecker::getKingCard() const {
		return _kingCard;
	}

	void ZhejiangHuChecker::buildCardIndex(const std::vector<BYTE>& cards, BYTE cbCardIndex[MAX_INDEX]) {
		for (int i = 0; i < MAX_INDEX; i++)
			cbCardIndex[i] = 0;
		for (size_t i = 0; i < cards.size(); i++) {
			BYTE c = cards[i];
			// 网狐牌索引：按其自身的换算规则（0x01-0x09 万、0x11-0x19 条、0x21-0x29 筒、0x31-0x37 字）
			int color = (c & 0xF0) >> 4;
			int value = c & 0x0F;
			if (value < 1 || value > 9)
				continue;
			int index = -1;
			if (color == 0)			// 万
				index = value - 1;
			else if (color == 1)	// 条
				index = 9 + (value - 1);
			else if (color == 2)	// 筒
				index = 18 + (value - 1);
			else if (color == 3)	// 字
				index = 27 + (value - 1);
			if (index < 0 || index >= MAX_INDEX)
				continue;
			cbCardIndex[index]++;
		}
	}

	bool ZhejiangHuChecker::checkHu(const std::vector<BYTE>& cards, BYTE currentCard, bool zimo, ZhejiangHuResult& result) {
		result.reset();
		result.zimo = zimo;
		if (cards.empty())
			return false;

		BYTE cbCardIndex[MAX_INDEX];
		buildCardIndex(cards, cbCardIndex);
		// 13 张手牌 + 摸到/打出的当前牌：把当前牌计入索引
		if ((cards.size() % 3) == 1 && currentCard != 0) {
			int color = (currentCard & 0xF0) >> 4;
			int value = currentCard & 0x0F;
			int idx = -1;
			if (color == 0) idx = value - 1;
			else if (color == 1) idx = 9 + (value - 1);
			else if (color == 2) idx = 18 + (value - 1);
			else if (color == 3) idx = 27 + (value - 1);
			if (idx >= 0 && idx < CARD_KING_INDEX)
				cbCardIndex[idx]++;
		}

		tagWeaveItem weaveItem[4];
		for (int i = 0; i < 4; i++) {
			weaveItem[i].cbWeaveKind = 0;
			weaveItem[i].cbCenterCard = 0;
			weaveItem[i].wProvideUser = INVALID_CHAIR;
		}

		tagHuInput input;
		input.pCardIndexSeq = cbCardIndex;
		input.pWeaveItemSeq = weaveItem;
		input.cbWeaveItemCount = 0;
		input.cbCurrentCard = currentCard;
		// 手牌 14 张时当前牌已包含在内；13 张时由 bInsertCurrentCard 插入
		input.bInsertCurrentCard = (cards.size() % 3 == 1) ? true : false;
		input.bJudgeHuOnly = true;
		input.bJudgeKingWait = false;

		tagHuCourt court;
		court.Reset();
		bool vendorHu = _logic.GetHuInfo(&input, court);

		// 网狐的 IsNeat3（平胡）组合枚举有缺陷，这里用自己实现的判定补齐
		// 财神（_kingCard）作为万能牌：统计手上的财神张数后从索引中剔除
		int jokers = 0;
		if (_kingCard != 0) {
			int color = (_kingCard & 0xF0) >> 4;
			int value = _kingCard & 0x0F;
			int idx = -1;
			if (color == 0) idx = value - 1;
			else if (color == 1) idx = 9 + (value - 1);
			else if (color == 2) idx = 18 + (value - 1);
			else if (color == 3) idx = 27 + (value - 1);
			if (idx >= 0 && idx < CARD_KING_INDEX) {
				jokers = cbCardIndex[idx];
				cbCardIndex[idx] = 0;
			}
		}
		bool neat3 = isNeat3WithJokers(cbCardIndex, jokers);
		bool neat2 = (cards.size() == 14) && _logic.IsNeat2(cbCardIndex);

		bool hu = vendorHu || neat3 || neat2;
		result.hu = hu;
		if (!hu)
			return false;
		if (neat3 && !vendorHu) {
			result.desc = "平胡";
			result.baseScore = 1;
			return true;
		}
		if (neat2 && !vendorHu) {
			result.desc = "七小对";
			result.baseScore = 2;
			return true;
		}

		result.zimo = court.IsZimo();
		// 简化番数：有财神按财神个数，无财神额外加分（具体分值由房间层再算）
		result.fan = static_cast<int>(court.cbKingCount);
		result.baseScore = court.IsHu() ? 1 : 0;
		result.desc = court.IsZimo() ? "自摸" : "点炮";
		if (court.cbKingCount > 0) {
			char buf[32];
			snprintf(buf, sizeof(buf), "+%d财神", court.cbKingCount);
			result.desc += buf;
		}
		return true;
	}


	// ---- 自己实现的平胡判定（3n+2，支持财神当万能牌）----
	namespace {
		// 剩余牌能否全部拆成刻子/顺子（jokers 为可任意替代的财神张数）
		bool canFormSets(BYTE idx[MAX_INDEX], int jokers, int from) {
			int i = from;
			while (i < CARD_KING_INDEX && idx[i] == 0)
				i++;
			if (i >= CARD_KING_INDEX)
				return (jokers % 3) == 0;

			// 刻子：用 k 张真牌 + (3-k) 张财神
			int useReal = idx[i] >= 3 ? 3 : idx[i];
			for (int k = useReal; k >= 0; k--) {
				int need = 3 - k;
				if (need > jokers)
					continue;
				idx[i] -= k;
				bool ok = canFormSets(idx, jokers - need, i);
				idx[i] += k;
				if (ok)
					return true;
			}

			// 顺子：i, i+1, i+2，缺的用财神补（字牌不能成顺）
			bool isWind = (i >= 27);
			if (!isWind && (i % 9) <= 6) {
				int need = 0;
				int use[3];
				for (int d = 0; d < 3; d++) {
					use[d] = idx[i + d] > 0 ? 1 : 0;
					need += (use[d] ? 0 : 1);
				}
				if (need <= jokers) {
					for (int d = 0; d < 3; d++)
						idx[i + d] -= use[d];
					bool ok = canFormSets(idx, jokers - need, i);
					for (int d = 0; d < 3; d++)
						idx[i + d] += use[d];
					if (ok)
						return true;
				}
			}
			return false;
		}
	}

	bool ZhejiangHuChecker::isNeat3(const BYTE cbCardIndex[MAX_INDEX]) {
		return isNeat3WithJokers(cbCardIndex, 0);
	}

	bool ZhejiangHuChecker::isNeat3WithJokers(const BYTE cbCardIndex[MAX_INDEX], int jokers) {
		int total = jokers;
		for (int i = 0; i < CARD_KING_INDEX; i++)
			total += cbCardIndex[i];
		if (total == 0 || (total % 3) != 2)
			return false;

		// 依次尝试将牌（对子），剩余拆组
		for (int i = 0; i < CARD_KING_INDEX; i++) {
			for (int useReal = (cbCardIndex[i] >= 2 ? 2 : cbCardIndex[i]); useReal >= 0; useReal--) {
				int need = 2 - useReal;
				if (need > jokers)
					continue;
				BYTE work[MAX_INDEX];
				for (int k = 0; k < MAX_INDEX; k++)
					work[k] = cbCardIndex[k];
				work[i] -= useReal;
				if (canFormSets(work, jokers - need, 0))
					return true;
			}
		}
		return false;
	}
	std::string ZhejiangHuChecker::cardToText(BYTE card) {
		const char* names[] = { "万", "条", "筒", "字" };
		int color = (card & 0xF0) >> 4;
		int value = card & 0x0F;
		if (color < 0 || color > 3 || value < 1 || value > 9)
			return "??";
		char buf[24];
		if (color == 3) {
			const char* winds[] = { "东", "南", "西", "北", "中", "发", "白" };
			if (value >= 1 && value <= 7)
				snprintf(buf, sizeof(buf), "%s", winds[value - 1]);
			else
				snprintf(buf, sizeof(buf), "字%d", value);
		}
		else {
			snprintf(buf, sizeof(buf), "%d%s", value, names[color]);
		}
		return std::string(buf);
	}
}
