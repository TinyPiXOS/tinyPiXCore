//
//  ClassSelector.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef ClassSelector_hpp
#define ClassSelector_hpp

#include <stdio.h>
#include <iostream>
#include "Selector.hpp"

namespace future {
    class ClassSelector: public Selector {
    public:
        ClassSelector(const std::string& cls)
        {
            m_class = cls;
            m_selectorType = Selector::ClassSelector;
        };
        inline std::string getClassIdentifier()
        {
            return m_class;
        }
        bool isBaseSelector();
        int weight();
        virtual std::string description();
    private:
        std::string m_class;
    };
}

#endif /* ClassSelector_hpp */
