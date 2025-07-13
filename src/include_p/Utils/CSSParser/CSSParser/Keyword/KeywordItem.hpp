//
//  KeywordItem.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/8.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef KeywordItem_hpp
#define KeywordItem_hpp

#include <stdio.h>
#include <iostream>

namespace future {
    class KeywordItem {
    public:
        KeywordItem(const std::string& name);
        ~KeywordItem();
        void setData(const std::string& data)
        {
            m_data = data;
        };
        
        std::string& getData()
        {
            return m_data;
        }
        
        std::string getName()
        {
            return m_name;
        }
    private:
        std::string m_name;
        std::string m_data;
    };
}

#endif /* KeywordItem_hpp */
