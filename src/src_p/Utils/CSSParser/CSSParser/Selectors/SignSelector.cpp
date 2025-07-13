//
//  SignSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "SignSelector.hpp"

namespace future {
    bool SignSelector::operator>(future::SignSelector *other)
    {
        SignType otherType = other->getSignType();
        return m_SignType == Concat && otherType != Concat;
    }

    bool SignSelector::isBaseSelector()
    {
    	return true;
    }

    int SignSelector::weight()
    {
    	return 0;
    }
}
