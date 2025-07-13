//
//  UniversalSelector.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef UniversalSelector_hpp
#define UniversalSelector_hpp

#include <stdio.h>

#include <iostream>
#include "Selector.hpp"

namespace future {
    class UniversalSelector: public Selector {
    public:
        UniversalSelector()
        {
            m_selectorType = Selector::UniversalSelector;
        }
        bool isBaseSelector();
        virtual std::string description();
        int weight();
    };
}

#endif /* UniversalSelector_hpp */
