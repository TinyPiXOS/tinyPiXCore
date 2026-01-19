//
//  SignSelector.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef SignSelector_hpp
#define SignSelector_hpp

#include <stdio.h>
#include "Selector.hpp"

namespace future {
    class SignSelector: public Selector {
    public:
        enum SignType {
            NormalInherit,
            Plus,
            Greater,
            Tidle,
            Concat,
            Comma,
        };
        SignSelector(SignType type)
        {
            m_SignType = type;
            m_selectorType = Selector::SignSelector;
        }
        inline SignType getSignType()
        {
            return m_SignType;
        }
        bool operator >(SignSelector *);
        bool isBaseSelector();
        int weight();
    private:
        SignType m_SignType;
    };
}

#endif /* SignSelector_hpp */
