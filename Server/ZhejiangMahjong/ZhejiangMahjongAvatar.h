// ZhejiangMahjongAvatar.h
// 浙江麻将玩家替身（只管手牌，牌型判定交给 ZhejiangHuChecker）

#ifndef _NIU_MA_ZHEJIANG_MAHJONG_AVATAR_H_
#define _NIU_MA_ZHEJIANG_MAHJONG_AVATAR_H_

#include "Game/GameAvatar.h"

#include <vector>

namespace NiuMa
{
	class ZhejiangMahjongAvatar : public GameAvatar
	{
	public:
		ZhejiangMahjongAvatar(const std::string& playerId, int seat, bool robot);
		virtual ~ZhejiangMahjongAvatar();

	public:
		// 手牌
		void setHand(const std::vector<int>& hand);
		const std::vector<int>& getHand() const;
		int getHandNum() const;
		void addTile(int tile);
		bool removeTile(int tile);
		void sortHand();

		// 碰/杠牌组（每组记一张代表牌）
		void addMeld(int tile, bool kong = false);
		const std::vector<int>& getMelds() const;
		int getMeldCount() const;

		// 该牌是否已经是杠（4 张），否则视为碰（3 张）
		bool isKongMeld(int tile) const;

		// 把碰升级为杠（补杠）
		bool upgradeToKong(int tile);

		// 清空
		void clear();

	private:
		std::vector<int> _hand;
		std::vector<int> _melds;
		std::vector<int> _kongs;		// 已升级为杠的牌
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_AVATAR_H_
