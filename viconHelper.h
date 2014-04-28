#ifndef _VICONHELPER_H
#define _VICONHELPER_H


#include "ViconClient.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace ViconDataStreamSDK::CPP;
namespace
{
	std::string Adapt( const bool i_Value )
	{
		return i_Value ? "True" : "False";
	}

	std::string Adapt( const Direction::Enum i_Direction )
	{
		switch( i_Direction )
		{
			case Direction::Forward:
				return "Forward";
			case Direction::Backward:
				return "Backward";
			case Direction::Left:
				return "Left";
			case Direction::Right:
				return "Right";
			case Direction::Up:
				return "Up";
			case Direction::Down:
				return "Down";
			default:
				return "Unknown";
		}
	}
}
extern int viconInit();
extern void viconExit();
extern void viconTranslation(std::map<std::string,std::vector<float>> trackingData);
#endif