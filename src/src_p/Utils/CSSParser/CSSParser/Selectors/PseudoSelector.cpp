//
//  PseudoSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "PseudoSelector.hpp"

namespace future {

    PseudoSelector::~PseudoSelector()
    {
        delete m_parameter;
        m_parameter = NULL;
    }

	bool PseudoSelector::isBaseSelector()
	{
		return true;
	}

	int PseudoSelector::weight()
	{
		return 10;
	}

    void PseudoSelector::setParameter(future::PseudoSelector::Parameter *p)
    {
        if (m_parameter == p) {
            return;
        }
        if (m_parameter) {
            delete m_parameter;
        }
        m_parameter = p;
    }

    PseudoSelector::Parameter* PseudoSelector::getParameter()
    {
        return m_parameter;
    }

    std::string PseudoSelector::getPseudoData()
    {
        return m_data;
    }
    
    std::string PseudoSelector::description()
    {
        std::string parament = "PseudoSelector: { pseudo name:" + m_data + "\n";
        if (m_parameter) {
            parament += "parament: ";
            char cnumber[256] = {'\0'};
            if (m_parameter->type == STRING) {
                parament += m_parameter->pString;
            } else if (m_parameter->type == NUMBER) {
                sprintf(cnumber, "%d", m_parameter->pNumber);
                parament += cnumber;
            } else if (m_parameter->type == POLYNOMIAL) {
                char coe[256] = {'\0'};
                char con[256] = {'\0'};
                std::string sign = m_parameter->polynomial.sign == 1 ? "+" : "-";
                sprintf(coe, "%d", m_parameter->polynomial.coefficient);
                sprintf(con, "%d", m_parameter->polynomial.constant);
                parament += std::string(coe) + "n" + sign + con;
            } else if (m_parameter->type == IDENT) {
                parament += m_parameter->pString;
            } else if (m_parameter->type == NONE) {
                parament += "None";
            }
        }
        parament += "}\n";
        return parament;
    }
}
