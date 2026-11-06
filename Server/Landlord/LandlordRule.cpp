// LandlordRule.cpp

#include "LandlordRule.h"
#include "PokerUtilities.h"

#include <algorithm>
#include <map>

namespace NiuMa
{
	LandlordRule::LandlordRule() {
		if (_orderTable == nullptr)
			_orderTable = new CardOrderTable();
		CardOrderTable* table = _orderTable;
		(void)table;
		// 点数顺序：3 < 4 < 5 < 6 < 7 < 8 < 9 < 10 < J < Q < K < A < 2 < 王
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
		_orderTable->setPointOrder(static_cast<int>(PokerPoint::Joker), 13);
		// 花色顺序（斗地主不比花色，仅保证大小王的相对关系）
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Diamond), 0);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Club), 1);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Heart), 2);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Spade), 3);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Little), 4);
		_orderTable->setSuitOrder(static_cast<int>(PokerSuit::Big), 5);
	}

	LandlordRule::~LandlordRule() {}

	int LandlordRule::getBombOrder(int genre) const {
		if (genre == static_cast<int>(LandlordGenre::Bomb))
			return 1;
		if (genre == static_cast<int>(LandlordGenre::Rocket))
			return 2;
		return -1;
	}

	int LandlordRule::getBombByOrder(int order) const {
		if (order == 1)
			return static_cast<int>(LandlordGenre::Bomb);
		if (order == 2)
			return static_cast<int>(LandlordGenre::Rocket);
		return static_cast<int>(LandlordGenre::Invalid);
	}

	int LandlordRule::predicateCardGenre(PokerGenre& pcg) const {
		const CardArray& cards = pcg.getCards();
		int nums = pcg.getCardNums();
		if (nums <= 0)
			return 0;

		// 统计每个点数的张数
		std::map<int, int> counts;
		for (int i = 0; i < nums; i++)
			counts[cards[i].getPoint()]++;

		int little = 0;
		int big = 0;
		for (std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
			if (it->first == static_cast<int>(PokerPoint::Joker)) {
				// 王：按花色区分大小王
				for (int i = 0; i < nums; i++) {
					if (cards[i].getPoint() != it->first)
						continue;
					if (cards[i].getSuit() == static_cast<int>(PokerSuit::Little))
						little++;
					else
						big++;
				}
			}
		}

		// 火箭：大小王各一张
		if (nums == 2 && little == 1 && big == 1) {
			pcg.setGenre(static_cast<int>(LandlordGenre::Rocket));
			pcg.setOfficer(cards[0]);
			return 1;
		}

		// 炸弹：四张同点
		if (nums == 4 && counts.size() == 1) {
			pcg.setGenre(static_cast<int>(LandlordGenre::Bomb));
			pcg.setOfficer(cards[0]);
			return 1;
		}

		// 单张、对子、三条
		if (counts.size() == 1) {
			if (nums == 1) {
				pcg.setGenre(static_cast<int>(LandlordGenre::Single));
				pcg.setOfficer(cards[0]);
				return 1;
			}
			if (nums == 2) {
				pcg.setGenre(static_cast<int>(LandlordGenre::Pair));
				pcg.setOfficer(cards[0]);
				return 1;
			}
			if (nums == 3) {
				pcg.setGenre(static_cast<int>(LandlordGenre::Triple));
				pcg.setOfficer(cards[0]);
				return 1;
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
			if (triplePoint >= 0 && nums == 4 && otherNums == 1) {
				pcg.setGenre(static_cast<int>(LandlordGenre::Triple1));
				for (int i = 0; i < nums; i++) {
					if (cards[i].getPoint() == triplePoint) {
						pcg.setOfficer(cards[i]);
						break;
					}
				}
				return 1;
			}
			if (triplePoint >= 0 && nums == 5 && otherNums == 2) {
				pcg.setGenre(static_cast<int>(LandlordGenre::Triple2));
				for (int i = 0; i < nums; i++) {
					if (cards[i].getPoint() == triplePoint) {
						pcg.setOfficer(cards[i]);
						break;
					}
				}
				return 1;
			}
			return 0;
		}

		// 顺子：5张以上连续单牌，且不含2和王
		if (counts.size() == static_cast<size_t>(nums) && nums >= 5) {
			bool ok = true;
			int prevOrder = -1;
			for (std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
				int point = it->first;
				if (point == static_cast<int>(PokerPoint::Two) || point == static_cast<int>(PokerPoint::Joker)) {
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
				pcg.setGenre(static_cast<int>(LandlordGenre::Straight));
				pcg.setOfficer(cards[0]);
				return 1;
			}
		}

		// 连对：3对以上连续对子，且不含2和王
		if (counts.size() >= 3 && nums == static_cast<int>(counts.size()) * 2) {
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
					if (point == static_cast<int>(PokerPoint::Two) || point == static_cast<int>(PokerPoint::Joker)) {
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
					pcg.setGenre(static_cast<int>(LandlordGenre::StraightPair));
					pcg.setOfficer(cards[0]);
					return 1;
				}
			}
		}

		return 0;
	}
}
