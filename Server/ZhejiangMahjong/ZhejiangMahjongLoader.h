// ZhejiangMahjongLoader.h

#ifndef _NIU_MA_ZHEJIANG_MAHJONG_LOADER_H_
#define _NIU_MA_ZHEJIANG_MAHJONG_LOADER_H_

#include "Venue/VenueLoader.h"

namespace NiuMa
{
	class ZhejiangMahjongLoader : public VenueLoader
	{
	public:
		ZhejiangMahjongLoader();
		virtual ~ZhejiangMahjongLoader();

	public:
		virtual Venue::Ptr load(const std::string& id) override;
	};
}

#endif // !_NIU_MA_ZHEJIANG_MAHJONG_LOADER_H_
