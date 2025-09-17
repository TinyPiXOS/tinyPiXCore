
//
//  ClassSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "ClassSelector.hpp"

namespace future {
	bool ClassSelector::isBaseSelector()
	{
		return true;
	}

	int ClassSelector::weight()
	{
		return 10;
	}
    
    std::string ClassSelector::description()
    {
        return "Class Selector (Class name is \"" + m_class + "\")";
    }
}
