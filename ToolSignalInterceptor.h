#pragma once

#include <Spore\BasicIncludes.h>
#include <Spore/Simulator/cSpaceToolData.h>
#include <Spore/Simulator/cDefaultBeamTool.h>

#define ToolSignalInterceptorPtr intrusive_ptr<ToolSignalInterceptor>

class ToolSignalInterceptor 
	:public Simulator::cDefaultBeamTool
{
public:
	static const uint32_t TYPE = id("ToolSignalInterceptor");
	
	ToolSignalInterceptor();
	~ToolSignalInterceptor();

	//bool OnHit(Simulator::cSpaceToolData* pTool, const Vector3& position, Simulator::SpaceToolHit hitType, int) override;
	//virtual bool OnSelect(Simulator::cSpaceToolData* pTool) override;
private:

	enum TargetPref {
		Creature,
		Building,
		Vehicle,
	};
};
