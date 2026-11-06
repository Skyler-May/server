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

		// 清空
		void clear();

	private:
		std::vector<int> _hand;
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_AVATAR_H_
