//
//  AttributeSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "AttributeSelector.hpp"

namespace future {
	bool AttributeSelector::isBaseSelector()
	{
		return true;
	}

	int AttributeSelector::weight()
	{
		return 10;
	}
    
    std::string AttributeSelector::description()
    {
        std::string relation = "no relation";
        switch (m_filterRule) {
            case Prefix: {
                relation = "prefix";
                break;
            }
            case Suffix: {
                relation = "suffix";
                break;
            }
            case Include: {
                relation = "include";
                break;
            }
            case Equal: {
                relation = "equal";
                break;
            }
            case Substring: {
                relation = "substring";
                break;
            }
            case DashMatch: {
                relation = "dashmatch";
                break;
            }
            case NoRule: {
                relation = "no rule";
                break;
            }
            default:
                break;
        }
        if (m_filterRule == Prefix) {
            relation = "prefix";
        }
        return "AttributeSelector: " + m_key + " " + relation + " " + m_value;
    }
}
