//
//  Selector.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef Selector_hpp
#define Selector_hpp

#include <stdio.h>
#include <iostream>
#include "ContainerUtil.hpp"

namespace future {
class Selector {
	friend class CombineSelector;
public:
	enum SelectorType {
		TypeSelector,
		IDSelector,
		ClassSelector,
		UniversalSelector,
		AttributeSelector,
		PseudoSelector,
		SimpleSelectorSequence,
		CombineSelector,
		SelectorGroup,
		SignSelector
	};
public:
	Selector();
	virtual ~Selector() = 0;
	inline std::string& getRuleData() {
		return m_ruleData;
	}

	void setRuleData(const std::string& data) {
		m_ruleData = data;
	}

	SelectorType getType() {
		return m_selectorType;
	}

	void setHostCSSFilePath(const std::string& path) {
		m_hostCSSFilePath = path;
	}

	std::string getHostCSSFilePath() {
		return m_hostCSSFilePath;
	}

	virtual bool isBaseSelector() = 0;
	virtual int weight() = 0;

	virtual std::string description();
protected:
	std::string m_hostCSSFilePath;
    std::string m_ruleData;
	SelectorType m_selectorType;
};
}

#endif /* Selector_hpp */
