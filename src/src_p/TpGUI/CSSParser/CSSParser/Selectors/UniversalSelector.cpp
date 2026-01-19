//
//  UniversalSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "UniversalSelector.hpp"

namespace future {

	bool UniversalSelector::isBaseSelector()
	{
		return true;
	}

	int UniversalSelector::weight()
	{
		return 0;
	}
    
    std::string UniversalSelector::description()
    {
        return "UniversalSelector";
    }
}
