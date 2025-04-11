#pragma once

#include <Spore\BasicIncludes.h>

#include "TribeMemberManager.h"
#include "TribePlanManager.h"
#include "TribeToolStratManager.h"
#include "TribeToolManager.h"

#define HBTribePtr intrusive_ptr<HBTribe>


// TRG

class HBTribe 
{
public:
	
	static void Initialize() {
		auto trg_membermanager = new(cTribeMemberManager);
		auto trg_tribeplanmanager = new(cTribePlanManager);
		auto trg_toolstratmanager = new(cTribeToolStratManager);
		auto trg_toolmanager = new(cTribeToolManager);
	}


};
