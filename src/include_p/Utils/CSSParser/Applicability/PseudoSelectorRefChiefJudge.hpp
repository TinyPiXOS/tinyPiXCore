//
//  PseudoSelectorRefChiefJudge.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/9/12.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef PseudoSelectorRefChiefJudge_hpp
#define PseudoSelectorRefChiefJudge_hpp

#include <map>
#include <stdio.h>
#include "gumbo.h"
#include "PseudoSelector.hpp"

namespace future {
    class PseudoSelector;
    class PseudoSelectorRefChiefJudge {
    public:
        static bool nodeAdaptToPseudo(GumboNode *, PseudoSelector*);
    private:
        typedef void(*traverseAction)(GumboNode* node, bool& stop, void* userData);
    private:
        static std::map<std::string, bool>& getDynamicPseudoClassMap();
        
        static void traverseElementNodeSiblings(GumboNode *node, traverseAction ac, void *userData);
        
        static int indexOfSiblings(GumboNode* node);
        static int indexEqualTypeOfSiblings(GumboNode* node);
        
        static int lastIndexOfSiblings(GumboNode* node);
        static int lastIndexEqualTypeOfSiblings(GumboNode* node);
        
        static bool indexAdaptToPoly(PseudoSelector::Parameter* parameter, int idx);
        
        static bool formulaHaveIntergerSolution(PseudoSelector::Parameter* parameter, int idx);
    };
}

#endif /* PseudoSelectorRefChiefJudge_hpp */
