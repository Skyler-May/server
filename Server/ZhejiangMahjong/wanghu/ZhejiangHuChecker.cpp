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
		bool hu = _logic.GetHuInfo(&input, court);
		result.hu = hu;
		if (!hu)
			return false;

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
