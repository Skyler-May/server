// LandlordLoader.h

#ifndef _NIU_MA_LANDLORD_LOADER_H_
#define _NIU_MA_LANDLORD_LOADER_H_

#include "Venue/VenueLoader.h"
#include "LandlordRule.h"

namespace NiuMa
{
	/**
	 * 斗地主房间加载器
	 */
	class LandlordLoader : public VenueLoader
	{
	public:
		LandlordLoader();
		virtual ~LandlordLoader();

	public:
		virtual Venue::Ptr load(const std::string& id) override;

	private:
		std::shared_ptr<LandlordRule> _rule;
	};
}

#endif // !_NIU_MA_LANDLORD_LOADER_H_
