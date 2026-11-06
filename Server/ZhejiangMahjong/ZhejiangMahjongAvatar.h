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
		void addMeld(int tile);
		const std::vector<int>& getMelds() const;
		int getMeldCount() const;

		// 清空
		void clear();

	private:
		std::vector<int> _hand;
		std::vector<int> _melds;
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_AVATAR_H_
