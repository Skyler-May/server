// PaoDeKuaiRule.cpp

#include "PaoDeKuaiRule.h"
#include "PokerUtilities.h"

#include <algorithm>
#include <map>

namespace NiuMa
{
	PaoDeKuaiRule::PaoDeKuaiRule() {
		if (_orderTable == nullptr)
			_orderTable = new CardOrderTable();
		// 点数顺序：3 < 4 < 5 < 6 < 7 < 8 < 9 < 10 < J < Q < K < A < 2
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Three), 0);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Four), 1);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Five), 2);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Six), 3);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Seven), 4);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Eight), 5);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Nine), 6);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Ten), 7);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Jack), 8);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Queen), 9);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::King), 10);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Ace), 11);
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Two), 12);
		// 花色顺序（跑得快不比花色）
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Diamond), 0);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Club), 1);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Heart), 2);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Spade), 3);
	}

	PaoDeKuaiRule::~PaoDeKuaiRule() {}

	bool PaoDeKuaiRule::isDisapprovedCard(const PokerCard& c) const {
		// 跑得快没有大小王，牌池只保留 52 张中的 48 张（去掉大小王）
		int suit = c.getSuit();
		if (suit == static_cast<int>(PokerSuit::Little) || suit == static_cast<int>(PokerSuit::Big))
			return true;
		return false;
	}

	int PaoDeKuaiRule::getBombOrder(int genre) const {
		if (genre == static_cast<int>(PaoDeKuaiGenre::Bomb))
			return 1;
		return -1;
	}

	int PaoDeKuaiRule::getBombByOrder(int order) const {
		if (order == 1)
			return static_cast<int>(PaoDeKuaiGenre::Bomb);
		return static_cast<int>(PaoDeKuaiGenre::Invalid);
	}

	int PaoDeKuaiRule::predicateCardGenre(PokerGenre& pcg) const {
		const CardArray& cards = pcg.getCards();
		int nums = pcg.getCardNums();
		if (nums <= 0)
			return 0;

		// 统计每个点数的张数
		std::map<int, int> counts;
		for (int i = 0; i < nums; i++)
			counts[cards[i].getPoint()]++;

		// 炸弹：四张同点
		if (nums == 4 && counts.size() == 1) {
			pcg.setGenre(static_cast<int>(PaoDeKuaiGenre::Bomb));
			pcg.setOfficer(cards[0]);
			return static_cast<int>(PaoDeKuaiGenre::Bomb);
		}

		// 单张、对子、三张
		if (counts.size() == 1) {
			int genre = 0;
			if (nums == 1)
				genre = static_cast<int>(PaoDeKuaiGenre::Single);
			else if (nums == 2)
				genre = static_cast<int>(PaoDeKuaiGenre::Pair);
			else if (nums == 3)
				genre = static_cast<int>(PaoDeKuaiGenre::Triple);
			if (genre > 0) {
				pcg.setGenre(genre);
				pcg.setOfficer(cards[0]);
				return genre;
			}
			return 0;
		}

		// 三带一 / 三带二
		if (counts.size() == 2) {
			int triplePoint = -1;
			int otherNums = 0;
			for (std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
				if (it->second == 3)
					triplePoint = it->first;
				else
					otherNums = it->second;
			}
			int genre = 0;
			if (triplePoint >= 0 && nums == 4 && otherNums == 1)
				genre = static_cast<int>(PaoDeKuaiGenre::Triple1);
			else if (triplePoint >= 0 && nums == 5 && otherNums == 2)
				genre = static_cast<int>(PaoDeKuaiGenre::Triple2);
			if (genre > 0) {
				for (int i = 0; i < nums; i++) {
					if (cards[i].getPoint() == triplePoint) {
						pcg.setGenre(genre);
						pcg.setOfficer(cards[i]);
						return genre;
					}
				}
			}
			return 0;
		}

		// 顺子：5张以上连续单牌，且不含2
		if (counts.size() == static_cast<size_t>(nums) && nums >= 5) {
			bool ok = true;
			int prevOrder = -1;
			for (std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
				int point = it->first;
				if (point == static_cast<int>(PokerPoint::Two)) {
					ok = false;
					break;
				}
				int order = getPointOrder(point);
				if (order < 0 || (prevOrder >= 0 && order != prevOrder + 1)) {
					ok = false;
					break;
				}
				prevOrder = order;
			}
			if (ok) {
				pcg.setGenre(static_cast<int>(PaoDeKuaiGenre::Straight));
				pcg.setOfficer(cards[0]);
				return static_cast<int>(PaoDeKuaiGenre::Straight);
			}
		}

		// 连对：2对以上连续对子，且不含2
		if (counts.size() >= 2 && nums == static_cast<int>(counts.size()) * 2) {
			bool allPair = true;
			for (std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
				if (it->second != 2) {
					allPair = false;
					break;
				}
			}
			if (allPair) {
				bool ok = true;
				int prevOrder = -1;
				for (std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
					int point = it->first;
					if (point == static_cast<int>(PokerPoint::Two)) {
						ok = false;
						break;
					}
					int order = getPointOrder(point);
					if (order < 0 || (prevOrder >= 0 && order != prevOrder + 1)) {
						ok = false;
						break;
					}
					prevOrder = order;
				}
				if (ok) {
					pcg.setGenre(static_cast<int>(PaoDeKuaiGenre::StraightPair));
					pcg.setOfficer(cards[0]);
					return static_cast<int>(PaoDeKuaiGenre::StraightPair);
				}
			}
		}

		return 0;
	}
}
