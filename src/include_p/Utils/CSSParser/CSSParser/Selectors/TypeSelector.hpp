//
//  TypeSelector.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef TypeSelector_hpp
#define TypeSelector_hpp

#include <stdio.h>
#include <iostream>
#include "Selector.hpp"

namespace future {
    class TypeSelector: public Selector {
    public:
        TypeSelector(const std::string& typeName)
        {
            m_typeName = typeName;
            m_selectorType = Selector::TypeSelector;
        }

        inline std::string getTagName()
        {
        	return m_typeName;
        }

        bool isBaseSelector();
        int weight();
        virtual std::string description();
    private:
        std::string m_typeName;
    };
}

#endif /* TypeSelector_hpp */
