// ZhejiangMahjongAvatar.cpp

#include "ZhejiangMahjongAvatar.h"

#include <algorithm>

namespace NiuMa
{
	ZhejiangMahjongAvatar::ZhejiangMahjongAvatar(const std::string& playerId, int seat, bool robot)
		: GameAvatar(playerId, robot)
	{
		setSeat(seat);
	}

	ZhejiangMahjongAvatar::~ZhejiangMahjongAvatar() {}

	void ZhejiangMahjongAvatar::setHand(const std::vector<int>& hand) {
		_hand = hand;
		sortHand();
	}

	const std::vector<int>& ZhejiangMahjongAvatar::getHand() const {
		return _hand;
	}

	int ZhejiangMahjongAvatar::getHandNum() const {
		return static_cast<int>(_hand.size());
	}

	void ZhejiangMahjongAvatar::addTile(int tile) {
		_hand.push_back(tile);
		sortHand();
	}

	bool ZhejiangMahjongAvatar::removeTile(int tile) {
		for (size_t i = 0; i < _hand.size(); i++) {
			if (_hand[i] == tile) {
				_hand.erase(_hand.begin() + i);
				return true;
			}
		}
		return false;
	}

	void ZhejiangMahjongAvatar::sortHand() {
		std::sort(_hand.begin(), _hand.end(), [](int a, int b) {
			// 按网狐牌值排序：花色在前，点数在后
			int ca = (a & 0xF0) >> 4, cb = (b & 0xF0) >> 4;
			if (ca != cb)
				return ca < cb;
			return (a & 0x0F) < (b & 0x0F);
		});
	}

	void ZhejiangMahjongAvatar::addMeld(int tile, bool kong) {
		_melds.push_back(tile);
		if (kong)
			_kongs.push_back(tile);
	}

	bool ZhejiangMahjongAvatar::isKongMeld(int tile) const {
		for (size_t i = 0; i < _kongs.size(); i++) {
			if (_kongs[i] == tile)
				return true;
		}
		return false;
	}

	bool ZhejiangMahjongAvatar::upgradeToKong(int tile) {
		for (size_t i = 0; i < _melds.size(); i++) {
			if (_melds[i] != tile)
				continue;
			if (isKongMeld(tile))
				return false;
			_kongs.push_back(tile);
			return true;
		}
		return false;
	}

	const std::vector<int>& ZhejiangMahjongAvatar::getMelds() const {
		return _melds;
	}

	int ZhejiangMahjongAvatar::getMeldCount() const {
		return static_cast<int>(_melds.size());
	}

	void ZhejiangMahjongAvatar::clear() {
		_hand.clear();
		_melds.clear();
		_kongs.clear();
		setReady(false);
	}
}
