// NiuNiuLoader.h

#ifndef _NIU_MA_NIU_NIU_LOADER_H_
#define _NIU_MA_NIU_NIU_LOADER_H_

#include "Venue/VenueLoader.h"
#include "NiuNiuRule.h"

namespace NiuMa
{
	class NiuNiuLoader : public VenueLoader
	{
	public:
		NiuNiuLoader();
		virtual ~NiuNiuLoader();

	public:
		virtual Venue::Ptr load(const std::string& id) override;

	private:
		std::shared_ptr<NiuNiuRule> _rule;
	};
}

#endif // !_NIU_MA_NIU_NIU_LOADER_H_
