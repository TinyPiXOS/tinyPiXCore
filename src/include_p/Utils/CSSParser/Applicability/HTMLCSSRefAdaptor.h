/*
 * HTMLCSSRefAdaptor.h
 *
 *  Created on: 2018-8-9
 *      Author: 1m0nster
 */

#ifndef HTMLCSSREFADAPTOR_H_
#define HTMLCSSREFADAPTOR_H_
#include "gumbo.h"
#include "SelectorsHeader.h"

namespace future {

class HTMLCSSRefAdaptor {
public:
    typedef GumboNode*** GumboArrayPtr;
    typedef GumboNode** GumboArray;
	HTMLCSSRefAdaptor();
	virtual ~HTMLCSSRefAdaptor();
    /**
     * Check Whether a selector can apply to the node in nodes
     * @param nodes the check nodes
     * @param selector the selector
     * @param potentialSize the size of nodes array
     * @note the nodes can be changed after the comparing.
     */
	static bool nodeAdaptToSelector(GumboArrayPtr nodes, Selector* selector, int *potentialSize);
    
    /**
     * Clean all resource after the compare
     */
    static void CleanResource();

private:
	static bool nodeAdaptToIDSelector(GumboArrayPtr node, IdSelector* selector, int *potentialSize);
	static bool nodeAdaptToClassSelector(GumboArrayPtr node, ClassSelector* selector, int *potentialSize);
	static bool nodeAdaptToTypeSelector(GumboArrayPtr node, TypeSelector* selector, int *potentialSize);
	static bool nodeAdaptToAttributeSelector(GumboArrayPtr node, AttributeSelector* selector, int *potentialSize);
	static bool nodeAdaptToPseudoSelector(GumboArrayPtr node, PseudoSelector* selector, int *potentialSize);
	static bool nodeAdaptToSequenceSelector(GumboArrayPtr node, SequenceSelector* selector, int *potentialSize);
	static bool nodeAdaptToCombineSelector(GumboArrayPtr node, CombineSelector* selector, int *potentialSize);
	static bool nodeAdaptToGroupSelector(GumboArrayPtr node, GroupSelector* selector, int *potentialSize);
    static void updateNextNodes(const std::list<GumboNode *>&, GumboArrayPtr arrayPtr, int *size);
};

}

#endif /* HTMLCSSREFADAPTOR_H_ */
