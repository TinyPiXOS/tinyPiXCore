//
//  SelectorSequence.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef SelectorSequence_hpp
#define SelectorSequence_hpp

#include <stdio.h>
#include <iostream>
#include <list>
#include "Selector.hpp"

namespace future {
    class SequenceSelector: public Selector {
    public:
        SequenceSelector()
        {
            m_selectorType = Selector::SimpleSelectorSequence;
        }
        ~SequenceSelector();
        void appendSelector(Selector *);
        std::list<Selector *> getContrains()
        {
            return m_selectors;
        }
        bool isBaseSelector();
        int weight();
        virtual std::string description();
    private:
        std::list<Selector *> m_selectors;
    };
}

#endif /* SelectorSequence_hpp */
