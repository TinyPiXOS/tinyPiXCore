//
//  AttributeSelector.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef AttributeSelector_hpp
#define AttributeSelector_hpp

#include <stdio.h>
#include <iostream>
#include "Selector.hpp"

namespace future {
    class AttributeSelector: public Selector {
    public:
        enum attributeFilterRule {
            Prefix,
            Suffix,
            Include,
            Equal,
            Substring,
            DashMatch,
            NoRule
        };
    public:
        AttributeSelector(const std::string& key, const std::string& value, attributeFilterRule rule)
        {
            m_key = key;
            m_value = value;
            m_filterRule = rule;
            m_selectorType = Selector::AttributeSelector;
        }

        inline std::string getKey()
        {
        	return m_key;
        }

        inline std::string getValue()
        {
        	return m_value;
        }

        inline attributeFilterRule getKVRule()
        {
        	return m_filterRule;
        }

        bool isBaseSelector();
        int weight();
        virtual std::string description();
    private:
        std::string m_key;
        std::string m_value;
        attributeFilterRule m_filterRule;
    };
}

#endif /* AttributeSelector_hpp */
