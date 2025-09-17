//
//  IdSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "IdSelector.hpp"

namespace future {

	bool IdSelector::isBaseSelector()
	{
		return true;
	}

	int IdSelector::weight()
	{
		return 100;
	}
    
    std::string IdSelector::description()
    {
        return "IdSelector (id is " + m_id + ")\n";
    }
}
