//
//  ContainerUtil.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/30.
//  Copyright © 2018年 1m0nster. All rights reserved.
//

#ifndef ContainerUtil_hpp
#define ContainerUtil_hpp

#include <stdio.h>
#include <set>

namespace future {
    /*
     * Container type must be STL Sequence
     * because of the unique and forword iterator conception
     */
    template<class VT, class T>
    void CleanContainer(T& container)
    {
        std::set<VT> nonrepeatContainer(container.begin(), container.end());
        auto it = nonrepeatContainer.begin();
        auto end = nonrepeatContainer.end();
        while(it != end) {
            delete *it++;
        }
        nonrepeatContainer.clear();
        container.clear();
    }
}

#endif /* ContainerUtil_hpp */
