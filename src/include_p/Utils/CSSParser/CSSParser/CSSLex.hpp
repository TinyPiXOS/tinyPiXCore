//
//  CSSLex.hpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef CSSLex_hpp
#define CSSLex_hpp

#include <stdio.h>
#include <iostream>
#include <list>
#include <set>
#include "CSSLexStatus.h"

namespace future {
    class Lex {
    public:
        struct CSSToken {
            CSSTokenType type;
            std::string data;
            CSSToken()
            {
            }
            ~CSSToken()
            {
            }
        };
    public:
        Lex();
        ~Lex();
        CSSToken*       GetToken();
        void            CleanResource();
        void            SetBufferSource(const std::string& fileName);
        void            SetBufferString(const std::string& bufferString);
    private:
        CSSToken*   GetIdentToken();
        CSSToken*   GetNumberToken();
        CSSToken*   GetTextToken(char stringType);
        bool        isDigitalCharacter(char);
        bool        isLetter(char);
        bool        isHexCharacter(char);
        bool        isWs(char);
        std::string createData(size_t start, size_t end);
    private:
        std::string             m_fileName;
        const char*             m_buffer;
        size_t                  m_bufferSize;
        size_t                  m_firstPos;
        size_t                  m_forwardPos;
        std::set<CSSToken *>   m_tokenCache;
    };
}
#endif /* CSSLex_hpp */
