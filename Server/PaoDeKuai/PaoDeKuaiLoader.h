// PaoDeKuaiLoader.h

#ifndef _NIU_MA_PAO_DE_KUAI_LOADER_H_
#define _NIU_MA_PAO_DE_KUAI_LOADER_H_

#include "Venue/VenueLoader.h"
#include "PaoDeKuaiRule.h"

namespace NiuMa
{
	/**
	 * 跑得快房间加载器
	 */
	class PaoDeKuaiLoader : public VenueLoader
	{
	public:
		PaoDeKuaiLoader();
		virtual ~PaoDeKuaiLoader();

	public:
		virtual Venue::Ptr load(const std::string& id) override;

	private:
		std::shared_ptr<PaoDeKuaiRule> _rule;
	};
}

#endif // !_NIU_MA_PAO_DE_KUAI_LOADER_H_
