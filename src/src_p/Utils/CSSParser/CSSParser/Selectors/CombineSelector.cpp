//
//  CombineSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "CombineSelector.hpp"
#include <assert.h>

namespace future {
    
    CombineSelector::~CombineSelector()
    {
        CleanContainer<Selector *>(m_normalSiblingList);
        CleanContainer<Selector *>(m_instanceSiblingList);
        CleanContainer<Selector *>(m_normalInhericalList);
        CleanContainer<Selector *>(m_instanceSiblingList);
    }
    
    void CombineSelector::initialNormalSiblingList(future::Selector *head, future::Selector *sibling)
    {
        if (m_combineType != NoCombine) {
            assert(0);
        }
        if (!head || !sibling) {
            return;
        }
        CleanContainer<Selector *>(m_normalSiblingList);
        m_normalSiblingList.clear();
        m_normalSiblingList.push_back(head);
        m_normalSiblingList.push_back(sibling);
        m_combineType = NormalSibling;
    }
    
    void CombineSelector::initialInstanceSiblingList(future::Selector *head, future::Selector *sibling)
    {
        if (m_combineType != NoCombine) {
            assert(0);
        }
        if (!head || !sibling) {
            return;
        }
        CleanContainer<Selector *>(m_instanceSiblingList);
        m_instanceSiblingList.clear();
        m_instanceSiblingList.push_back(head);
        m_instanceSiblingList.push_back(sibling);
        m_combineType = InstanceSibling;
    }
    
    void CombineSelector::initialNormalInhericalList(future::Selector *root, future::Selector *child)
    {
        if (m_combineType != NoCombine) {
            assert(0);
        }
        if (!root || !child) {
            return;
        }
        CleanContainer<Selector *>(m_normalInhericalList);
        m_normalInhericalList.clear();
        m_normalInhericalList.push_back(root);
        m_normalInhericalList.push_back(child);
        m_combineType = NormalInherical;
    }
    
    void CombineSelector::initialInstanceInhericalList(future::Selector *root, future::Selector *child)
    {
        if (m_combineType != NoCombine) {
            assert(0);
        }
        if (!root || !child) {
            return;
        }
        CleanContainer<Selector *>(m_instanceInhericalList);
        m_instanceInhericalList.clear();
        m_instanceInhericalList.push_back(root);
        m_instanceInhericalList.push_back(child);
        m_combineType = InstanceInherical;
    }

    bool CombineSelector::isBaseSelector()
    {
    	return false;
    }

    int CombineSelector::weight()
    {
    	int w = 0;
    	std::list<Selector *>::iterator one;
    	std::list<Selector *>::iterator other;
    	do {
    		if (m_normalInhericalList.size() == 2) {
    			one = m_normalInhericalList.begin();
    			other = --m_normalInhericalList.end();
    			break;
    		}
    		if (m_instanceInhericalList.size() == 2) {
    			one = m_instanceInhericalList.begin();
    			other = --m_instanceInhericalList.end();
    			break;
    		}
    		if (m_normalSiblingList.size() == 2) {
    			one = m_normalSiblingList.begin();
    			other = --m_normalSiblingList.end();
    			break;
    		}
    		if (m_instanceSiblingList.size() == 2) {
    			one = m_instanceSiblingList.begin();
    			other = --m_instanceSiblingList.end();
    			break;
    		}
    	} while (0);
    	if (!*one || !*other) {
    		return w;
    	}
    	w += (*one)->weight() + (*other)->weight();
    	return w;
    }

    Selector* CombineSelector::getBefore()
    {
    	std::list<Selector *>::iterator before;
    	do {
    		if (m_normalInhericalList.size() == 2) {
    			before = m_normalInhericalList.begin();
    			break;
    		}
    		if (m_instanceInhericalList.size() == 2) {
    			before = m_instanceInhericalList.begin();
    			break;
    		}
    		if (m_normalSiblingList.size() == 2) {
    			before = m_normalSiblingList.begin();
    			break;
    		}
    		if (m_instanceSiblingList.size() == 2) {
    			before = m_instanceSiblingList.begin();
    			break;
    		}
    	} while (0);
    	return *before;
    }

    Selector* CombineSelector::getAfter()
    {
    	std::list<Selector *>::iterator after;
    	do {
    		if (m_normalInhericalList.size() == 2) {
    			after = --m_normalInhericalList.end();
    			break;
    		}
    		if (m_instanceInhericalList.size() == 2) {
    			after = --m_instanceInhericalList.end();
    			break;
    	    }
    		if (m_normalSiblingList.size() == 2) {
    			after = --m_normalSiblingList.end();
    			break;
    		}
    		if (m_instanceSiblingList.size() == 2) {
    			after = --m_instanceSiblingList.end();
    			break;
    		}
    	} while (0);
    	return *after;
    }
    
    CombineSelector::CombineType CombineSelector::getCombineType()
    {
        return m_combineType;
    }
    
    std::string CombineSelector::description()
    {
        std::string relation = "no relation";
        std::string desc = "CombineSelector: {\n";
        if (!m_instanceSiblingList.empty()) {
            relation = "instance sibling";
        } else if (!m_normalSiblingList.empty()) {
            relation = "normal sibling";
        } else if (!m_normalInhericalList.empty()) {
            relation = "normal inherical";
        } else if (!m_instanceInhericalList.empty()) {
            relation = "instance inherical";
        }
        desc += "(" + getBefore()->description() + ")" + relation +
        + "(" + getAfter()->description() + ")\n}";
        return desc;
    }

} // namespace future
