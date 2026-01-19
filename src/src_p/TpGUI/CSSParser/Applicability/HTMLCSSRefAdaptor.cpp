/*
 * HTMLCSSRefAdaptor.cpp
 *
 *  Created on: 2018-8-9
 *      Author: 1m0nster
 */

#include "HTMLCSSRefAdaptor.h"
#include "StringUtil.h"
#include "PseudoSelectorRefChiefJudge.hpp"

extern const int HTMLTAGMAXSIZE = 257;
const char* HTMLTagNames[] = {
    "A", "ABBR", "ACRONYM", "ADDRESS", "ALTGLYPH", "ALTGLYPHDEF", "ALTGLYPHITEM",
    "ANIMATE",
    "ANIMATECOLOR",
    "ANIMATEMOTION",
    "ANIMATETRANSFORM",
    "ANNOTATION_XML",
    "APPLET",
    "AREA",
    "ARTICLE",
    "ASIDE",
    "AUDIO",
    "B",
    "BASE",
    "BASEFONT",
    "BDI",
    "BDO",
    "BGSOUND",
    "BIG",
    "BLINK",
    "BLOCKQUOTE",
    "BODY",
    "BR",
    "BUTTON",
    "CANVAS",
    "CAPTION",
    "CENTER",
    "CIRCLE",
    "CITE",
    "CLIPPATH",
    "CODE",
    "COL",
    "COLGROUP",
    "COLOR_PROFILE",
    "CURSOR",
    "DATA",
    "DATALIST",
    "DD",
    "DEFS",
    "DEL",
    "DESC",
    "DETAILS",
    "DFN",
    "DIR",
    "DIV",
    "DL",
    "DT",
    "ELLIPSE",
    "EM",
    "EMBED",
    "FEBLEND",
    "FECOLORMATRIX",
    "FECOMPONENTTRANSFER",
    "FECOMPOSITE",
    "FECONVOLVEMATRIX",
    "FEDIFFUSELIGHTING",
    "FEDISPLACEMENTMAP",
    "FEDISTANTLIGHT",
    "FEFLOOD",
    "FEFUNCA",
    "FEFUNCB",
    "FEFUNCG",
    "FEFUNCR",
    "FEGAUSSIANBLUR",
    "FEIMAGE",
    "FEMERGE",
    "FEMERGENODE",
    "FEMORPHOLOGY",
    "FEOFFSET",
    "FEPOINTLIGHT",
    "FESPECTACTUALRLIGHTING",
    "FESPOTLIGHT",
    "FETILE",
    "FETURBULENCE",
    "FIELDSET",
    "FIGCAPTION",
    "FIGURE",
    "FILTER",
    "FONT",
    "FONT_FACE",
    "FONT_FACE_FORMAT",
    "FONT_FACE_NAME",
    "FONT_FACE_SRC",
    "FONT_FACE_URI",
    "FOOTER",
    "FOREIGNOBJECT",
    "FORM",
    "FRAME",
    "FRAMESET",
    "G",
    "GLYPH",
    "GLYPHREF",
    "H1",
    "H2",
    "H3",
    "H4",
    "H5",
    "H6",
    "HEAD",
    "HEADER",
    "HGROUP",
    "HKERN",
    "HR",
    "HTML",
    "I",
    "IFRAME",
    "IMAGE",
    "IMG",
    "INPUT",
    "INS",
    "ISINDEX",
    "KBD",
    "KEYGEN",
    "LABEL",
    "LEGEND",
    "LI",
    "LINE",
    "LINEARGRADIENT",
    "LINK",
    "LISTING",
    "MACTION",
    "MAIN",
    "MALIGNGROUP",
    "MALIGNMARK",
    "MAP",
    "MARK",
    "MARKER",
    "MARQUEE",
    "MASK",
    "MATH",
    "MENCLOSE",
    "MENU",
    "MENUITEM",
    "MERROR",
    "META",
    "METADATA",
    "METER",
    "MFENCED",
    "MFRAC",
    "MGLYPH",
    "MI",
    "MISSING_GLYPH",
    "MLABELEDTR",
    "MLONGDIV",
    "MMULTISCRIPTS",
    "MN",
    "MO",
    "MOVER",
    "MPADDED",
    "MPATH",
    "MPHANTOM",
    "MPRESCRIPTS",
    "MROOT",
    "MROW",
    "MS",
    "MSCARRIES",
    "MSCARRY",
    "MSGROUP",
    "MSLINE",
    "MSPACE",
    "MSQRT",
    "MSROW",
    "MSTACK",
    "MSTYLE",
    "MSUB",
    "MSUBSUP",
    "MSUP",
    "MTABLE",
    "MTD",
    "MTEXT",
    "MTR",
    "MULTICOL",
    "MUNDER",
    "MUNDEROVER",
    "NAV",
    "NEXTID",
    "NOBR",
    "NOEMBED",
    "NOFRAMES",
    "NONE",
    "NOSCRIPT",
    "OBJECT",
    "OL",
    "OPTGROUP",
    "OPTION",
    "OUTPUT",
    "P",
    "PARAM",
    "PATH",
    "PATTERN",
    "PLAINTEXT",
    "POLYGON",
    "POLYLINE",
    "PRE",
    "PROGRESS",
    "Q",
    "RADIALGRADIENT",
    "RB",
    "RECT",
    "RP",
    "RT",
    "RTC",
    "RUBY",
    "S",
    "SAMP",
    "SCRIPT",
    "SECTION",
    "SELECT",
    "SEMANTICS",
    "SET",
    "SMALL",
    "SOURCE",
    "SPACER",
    "SPAN",
    "STOP",
    "STRIKE",
    "STRONG",
    "STYLE",
    "SUB",
    "SUMMARY",
    "SUP",
    "SVG",
    "SWITCH",
    "SYMBOL",
    "TABLE",
    "TBODY",
    "TD",
    "TEMPLATE",
    "TEXT",
    "TEXTPATH",
    "TEXTAREA",
    "TFOOT",
    "TH",
    "THEAD",
    "TIME",
    "TITLE",
    "TR",
    "TRACK",
    "TREF",
    "TSPAN",
    "TT",
    "U",
    "UL",
    "USE",
    "VAR",
    "VIDEO",
    "VIEW",
    "VKERN",
    "WBR",
    "XMP",
    "UNKNOWN",
    "LAST"
};

#define BackupNodes(newList, array, size) for (int i = 0 ; i < size; i++) {\
newList[i] = *(array+i);\
}
namespace future {
    
    static const char* CLASSATTRIBUTE = "class";
    static const char* IDATTRIBUTE = "id";
    std::list<HTMLCSSRefAdaptor::GumboArray> SMemoryBackup;
    HTMLCSSRefAdaptor::HTMLCSSRefAdaptor()
    {
    }
    
    HTMLCSSRefAdaptor::~HTMLCSSRefAdaptor()
    {
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToSelector(GumboNode*** node, Selector*selector, int *potentialSize)
    {
        if (!potentialSize) {
            int potential = 1;
            potentialSize = &potential;
        }
        Selector::SelectorType type = selector->getType();
        switch(type) {
            case Selector::TypeSelector: {
                return nodeAdaptToTypeSelector(node, dynamic_cast<TypeSelector *>(selector), potentialSize);
            }
            case Selector::IDSelector: {
                return nodeAdaptToIDSelector(node, dynamic_cast<IdSelector *>(selector), potentialSize);
            }
            case Selector::ClassSelector: {
                return nodeAdaptToClassSelector(node, dynamic_cast<ClassSelector *>(selector), potentialSize);
            }
            case Selector::UniversalSelector: {
                return true;
            }
            case Selector::AttributeSelector: {
                return nodeAdaptToAttributeSelector(node, dynamic_cast<AttributeSelector *>(selector), potentialSize);
            }
            case Selector::PseudoSelector: {
                return nodeAdaptToPseudoSelector(node, dynamic_cast<PseudoSelector *>(selector), potentialSize);
            }
            case Selector::SimpleSelectorSequence: {
                return nodeAdaptToSequenceSelector(node, dynamic_cast<SequenceSelector *>(selector), potentialSize);
            }
            case Selector::CombineSelector: {
                return nodeAdaptToCombineSelector(node, dynamic_cast<CombineSelector *>(selector), potentialSize);
            }
            case Selector::SelectorGroup: {
                return nodeAdaptToGroupSelector(node, dynamic_cast<GroupSelector *>(selector), potentialSize);
            }
            default:
                return false;
        }
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToClassSelector(GumboNode*** nodeArrayPtr, ClassSelector* selector, int *potentialSize)
    {
        bool ret = false;
        int potential = 1;
        int loopSize = *potentialSize;
        GumboNode** nodesArray = *nodeArrayPtr;
        std::list<GumboNode *>potentialNextNodes;
        for (int i = 0; i < loopSize; i++) {
            if ((nodesArray[i])->type != GUMBO_NODE_ELEMENT) {
                return false;
            }
            std::string selectorClass = selector->getClassIdentifier();
            // A node may have multiClasses, so using substring relation
            AttributeSelector s(CLASSATTRIBUTE, selectorClass, AttributeSelector::Substring);
            GumboArray elementA = nodesArray + i;
            ret = nodeAdaptToAttributeSelector(&elementA, &s, &potential);
            if (ret) {
                potentialNextNodes.push_back(nodesArray[i]);
            }
        }
        updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
        ret = !potentialNextNodes.empty();
        return ret;
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToIDSelector(GumboNode*** nodeArrayPtr, IdSelector* selector, int *potentialSize)
    {
        bool ret = false;
        int potential = 1;
        int loopSize = *potentialSize;
        std::list<GumboNode *>potentialNextNodes;
        GumboNode** nodesArray = *nodeArrayPtr;
        for (int i = 0; i < loopSize; i ++) {
            if (nodesArray[i]->type != GUMBO_NODE_ELEMENT) {
                return false;
            }
            std::string selectorId = selector->getIdIdentifier();
            AttributeSelector s(IDATTRIBUTE, selectorId, AttributeSelector::Equal);
            GumboArray elementA = nodesArray + i;
            bool ret = nodeAdaptToAttributeSelector(&elementA, &s, &potential);
            if (ret) {
                potentialNextNodes.push_back(nodesArray[i]);
            }
        }
        updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
        ret = !potentialNextNodes.empty();
        return ret;
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToTypeSelector(GumboNode*** nodeArrayPtr, TypeSelector* selector, int *potentialSize)
    {
        bool ret = false;
        GumboArray nodesArray = *nodeArrayPtr;
        std::list<GumboNode *>potentialNextNodes;
        for (int i = 0; i < *potentialSize; i++) {
            GumboNode* node = nodesArray[i];
            GumboTag tag;
            std::string tagName;
            if (node->type == GUMBO_NODE_ELEMENT) {
                tag = node->v.element.tag;
                if (tag >= HTMLTAGMAXSIZE) {
                    continue;
                }
                tagName = HTMLTagNames[tag];
            } else if (node->type == GUMBO_NODE_DOCUMENT) {
                tagName = "html";
            } else {
                continue;
            }
            std::string selectorTagName = StringUtil::toupper(selector->getTagName());
            ret = selectorTagName == tagName;
            if (ret) {
                potentialNextNodes.push_back(node);
            }
        }
        updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
        ret = !potentialNextNodes.empty();
        return ret;
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToAttributeSelector(GumboNode*** nodeArrayPtr, AttributeSelector* selector, int *potentialSize)
    {
        bool ret = false;
        int loopSize = *potentialSize;
        GumboArray nodesArray = *nodeArrayPtr;
        std::list<GumboNode *>potentialNextNodes;
        for (int i = 0; i < loopSize; i++) {
            GumboNode* node = nodesArray[i];
            if (node->type != GUMBO_NODE_ELEMENT) {
                continue;
            }
            std::string key = selector->getKey();
            std::string value = StringUtil::tolower(selector->getValue());
            AttributeSelector::attributeFilterRule kvRule = selector->getKVRule();
            GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, key.c_str());
            if (!attr) {
                continue;
            }
            std::string nodeAttrValue = StringUtil::tolower(attr->value);
            if (key.empty()) {
                continue;
            }
            switch (kvRule) {
                case AttributeSelector::Equal: {
                    ret = nodeAttrValue == value;
                    break;
                }
                case AttributeSelector::DashMatch: {
                    if (nodeAttrValue.find("-") == std::string::npos) {
                        break;
                    }
                    std::list<std::string> attrs = StringUtil::SeperatorBy(nodeAttrValue, '-');
                    ret = *attrs.begin() == value;
                    break;
                }
                case AttributeSelector::Prefix: {
                    ret = nodeAttrValue.find(value, 0) == 0;
                    break;
                }
                case AttributeSelector::Suffix: {
                    ret = nodeAttrValue.rfind(value) + value.length() == nodeAttrValue.length();
                    break;
                }
                case AttributeSelector::Include: {
                    if (nodeAttrValue.find(" ") == std::string::npos) {
                        break;
                    }
                    std::list<std::string> attrs = StringUtil::SeperatorBy(nodeAttrValue, ' ');
                    std::list<std::string>::iterator it = attrs.begin();
                    std::list<std::string>::iterator end = attrs.end();
                    while (it != end) {
                        if (*it++ == value) {
                            ret = true;
                            break;
                        }
                    }
                    break;
                }
                case AttributeSelector::Substring: {
                    ret = nodeAttrValue.find(value, 0) != std::string::npos;
                    break;
                }
                case AttributeSelector::NoRule: {
                    ret = true;
                    break;
                }
                default:
                    ret = false;
            }
            if (ret) {
                potentialNextNodes.push_back(node);
            }
        }
        updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
        ret = !potentialNextNodes.empty();
        return ret;
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToPseudoSelector(GumboNode ***nodePtr, PseudoSelector* selector, int *potentialSize)
    {
        // TODO need finish
        bool ret = true;
        GumboNode* nodesBackup[*potentialSize];
        BackupNodes(nodesBackup, *nodePtr, *potentialSize);
        int loopSize = *potentialSize;
        std::list<GumboNode *>potentialNextNodes;
        std::string pseudo = selector->getPseudoData();
        if (pseudo.empty()) {
            return false;
        }
        for (int i = 0; i < loopSize; i++) {
            GumboNode* node = nodesBackup[i];
            ret = PseudoSelectorRefChiefJudge::nodeAdaptToPseudo(node, selector);
            if (!ret) {
                continue;
            }
            potentialNextNodes.push_back(node);
        }
        updateNextNodes(potentialNextNodes, nodePtr, potentialSize);
        ret = !potentialNextNodes.empty();
        return ret;
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToSequenceSelector(GumboNode ***nodePtr, SequenceSelector* selector, int *potentialSize)
    {
        bool ret = true;
        const int size = *potentialSize;
        GumboNode* nodesBackup[size];
        BackupNodes(nodesBackup, *nodePtr, *potentialSize);
        int loopSize = *potentialSize;
        std::list<GumboNode *>potentialNextNodes;
        for (int i = 0 ; i < loopSize; i++) {
            std::list<Selector *>ss = selector->getContrains();
            std::list<Selector *>::iterator it = ss.begin();
            std::list<Selector *>::iterator end = ss.end();
            while(it != end) {
                GumboArray elementA = nodesBackup + i;
                int *temp = new int(1);
                ret = nodeAdaptToSelector(&elementA, *it++, temp) && ret;
                delete temp;
                if (!ret) {
                    continue;
                }
            }
            if (ret) {
                potentialNextNodes.push_back(nodesBackup[i]);
            }
        }
        updateNextNodes(potentialNextNodes, nodePtr, potentialSize);
        ret = !potentialNextNodes.empty();
        return ret;
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToGroupSelector(GumboNode ***nodePtr, GroupSelector* selector, int *potentialSize)
    {
        bool ret = false;
        GumboNode* nodesBackup[*potentialSize];
        BackupNodes(nodesBackup, *nodePtr, *potentialSize);
        std::list<GumboNode *>potentialNextNodes;
        int loopSize = *potentialSize;
        for (int i = 0 ; i < loopSize; i ++) {
            std::list<Selector *>ss = selector->getAllSelectors();
            std::list<Selector *>::iterator it = ss.begin();
            std::list<Selector *>::iterator end = ss.end();
            while(it != end) {
                GumboArray elementA = nodesBackup + i;
                int *temp = new int(1);
                ret = nodeAdaptToSelector(&elementA, *it++, temp) || ret;
            }
            if (ret) {
                potentialNextNodes.push_back(nodesBackup[i]);
            }
        }
        updateNextNodes(potentialNextNodes, nodePtr, potentialSize);
        ret = !potentialNextNodes.empty();
        return ret;
    }
    
    bool HTMLCSSRefAdaptor::nodeAdaptToCombineSelector(GumboNode ***nodeArrayPtr, CombineSelector* selector, int *potentialSize)
    {
        Selector* before = selector->getBefore();
        Selector* after = selector->getAfter();
        bool afterMatch = false;
        bool beforeMatch = false;
        bool ret = false;
        if (!before || !after) {
            return false;
        }
        switch (selector->getCombineType()) {
            case CombineSelector::NormalSibling: {
                GumboNode* nodesBackup[*potentialSize];
                BackupNodes(nodesBackup, *nodeArrayPtr, *potentialSize);
                int outterLoopSize = *potentialSize;
                std::list<GumboNode *>potentialNextNodes;
                for (int i = 0; i < outterLoopSize; i++) {
                    GumboNode* node = nodesBackup[i];
                    unsigned int c_idx = 0;
                    GumboNode* parent = node->parent;
                    if (!parent || parent->type != GUMBO_NODE_ELEMENT) {
                        continue;
                    }
                    GumboArray elementA = nodesBackup + i;
                    afterMatch = nodeAdaptToSelector(&elementA, after, potentialSize);
                    if (!afterMatch) {
                        continue;
                    }
                    GumboNode* beforeNodes[*potentialSize];
                    BackupNodes(beforeNodes, elementA, *potentialSize);
                    int innnerLoopSize = *potentialSize;
                    for (int j = 0; j < innnerLoopSize; j++) {
                        parent = beforeNodes[j]->parent;
                        c_idx = beforeNodes[j]->index_within_parent;
                        if (!parent || parent->type != GUMBO_NODE_ELEMENT) {
                            continue;
                        }
                        GumboVector children = parent->v.element.children;
                        for (unsigned int i = 0; i < children.length; i++) {
                            GumboNode* sibling = (GumboNode *)children.data[i];
                            if (sibling->index_within_parent >= c_idx) {
                                continue;
                            }
                            GumboArray elementB = &sibling;
                            int *temp = new int(1);
                            beforeMatch = nodeAdaptToSelector(&elementB, before, temp) || beforeMatch;
                            if (beforeMatch) {
                                potentialNextNodes.push_back(sibling);
                            }
                        }
                    }
                }
                {
                    updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
                }
                ret = !potentialNextNodes.empty();
                return ret;
                break;
            }
            case CombineSelector::InstanceSibling: {
                GumboNode* nodesBackup[*potentialSize];
                BackupNodes(nodesBackup, *nodeArrayPtr, *potentialSize);
                int outterLoopSize = *potentialSize;
                std::list<GumboNode *>potentialNextNodes;
                for (int i = 0; i < outterLoopSize; i++) {
                    GumboNode* node = nodesBackup[i];
                    GumboNode* parent = node->parent;
                    if (!parent || parent->type != GUMBO_NODE_ELEMENT) {
                        continue;
                    }
                    GumboArray elementA = nodesBackup + i;
                    afterMatch = nodeAdaptToSelector(&elementA, after, potentialSize);
                    if (!afterMatch) {
                        continue;
                    }
                    GumboNode* beforeNodes[*potentialSize];
                    BackupNodes(beforeNodes, elementA, *potentialSize);
                    int innnerLoopSize = *potentialSize;
                    for(int j = 0; j < innnerLoopSize; j ++) {
                        parent = (*(beforeNodes + j))->parent;
                        if (!parent || parent->type != GUMBO_NODE_ELEMENT) {
                            continue;
                        }
                        GumboVector children = parent->v.element.children;
                        GumboNode* lastElementNode = 0;
                        for (unsigned int k = 0; k < children.length; k++) {
                            GumboNode* sibling = (GumboNode *)children.data[k];
                            if (sibling == node && lastElementNode) {
                                GumboArray elementB = &lastElementNode;
                                int *temp = new int(1);
                                beforeMatch = nodeAdaptToSelector(&elementB, before, temp) || beforeMatch;
                                if (beforeMatch) {
                                    potentialNextNodes.push_back(lastElementNode);
                                }
                            }
                            if (sibling->type == GUMBO_NODE_ELEMENT) {
                                lastElementNode = sibling;
                            }
                        }
                    }
                }
                {
                    updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
                }
                ret = !potentialNextNodes.empty();
                return ret;
                break;
            }
            case CombineSelector::InstanceInherical: {
                GumboNode* nodesBackup[*potentialSize];
                BackupNodes(nodesBackup, *nodeArrayPtr, *potentialSize);
                int outterLoopSize = *potentialSize;
                std::list<GumboNode *>potentialNextNodes;
                for (int i = 0; i < outterLoopSize; i ++) {
                    GumboNode* node = nodesBackup[i];
                    GumboNode* parent = node->parent;
                    if (!parent) {
                        continue;
                    }
                    GumboArray elementA = nodesBackup + i;
                    afterMatch = nodeAdaptToSelector(&elementA, after, potentialSize);
                    if (!afterMatch) {
                        continue;
                    }
                    int innerSize = *potentialSize;
                    GumboNode* beforeNodes[innerSize];
                    BackupNodes(beforeNodes, elementA, innerSize);
                    for (int j = 0; j < innerSize; j++) {
                        parent = beforeNodes[j]->parent;
                        GumboArray elementB = &parent;
                        int *temp = new int(1);
                        beforeMatch = nodeAdaptToSelector(&elementB, before, temp);
                        if (beforeMatch) {
                            potentialNextNodes.push_back(parent);
                        }
                    }
                }
                {
                    updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
                }
                ret = !potentialNextNodes.empty();
                return ret;
                break;
            }
            case CombineSelector::NormalInherical: {
                GumboNode* nodesBackup[*potentialSize];
                BackupNodes(nodesBackup, *nodeArrayPtr, *potentialSize);
                int outterLoopSize = *potentialSize;
                std::list<GumboNode *>potentialNextNodes;
                for (int i = 0; i < outterLoopSize; i++) {
                    GumboArray elementA = nodesBackup + i;
                    afterMatch = nodeAdaptToSelector(&elementA, after, potentialSize);
                    if (!afterMatch) {
                        continue;
                    }
                    int innerSize = *potentialSize;
                    GumboNode* beforeNodes[innerSize];
                    BackupNodes(beforeNodes, elementA, innerSize);
                    for (int j = 0; j < innerSize; j++) {
                        GumboNode* parent = beforeNodes[j]->parent;
                        while(parent) {
                            GumboArray elementB = &parent;
                            int *temp = new int(1);
                            beforeMatch = nodeAdaptToSelector(&elementB, before, temp);
                            if (beforeMatch) {
                                potentialNextNodes.push_back(parent);
                            }
                            parent = parent->parent;
                        }
                    }
                }
                {
                    updateNextNodes(potentialNextNodes, nodeArrayPtr, potentialSize);
                }
                ret = !potentialNextNodes.empty();
                return ret;
                break;
            }
            default:
                return false;
        }
    }
    
    void HTMLCSSRefAdaptor::updateNextNodes(const std::list<GumboNode *>& potentialNextNodes, future::HTMLCSSRefAdaptor::GumboArrayPtr nodeArrayPtr, int *potentialSize)
    {
        std::list<GumboNode *>::const_iterator it = potentialNextNodes.begin();
        std::list<GumboNode *>::const_iterator end = potentialNextNodes.end();
        int idx = 0;
        int size = (int)potentialNextNodes.size();
        if (size) {
            GumboNode** potentialNodesArray = new GumboNode*[size];
            while (it != end) {
                potentialNodesArray[idx++] = *it++;
            }
            *potentialSize = size;
            *nodeArrayPtr = potentialNodesArray;
            SMemoryBackup.push_back(potentialNodesArray);
        }
    }
    
    void HTMLCSSRefAdaptor::CleanResource()
    {
        SMemoryBackup.unique();
        std::list<HTMLCSSRefAdaptor::GumboArray>::iterator it = SMemoryBackup.begin();
        std::list<HTMLCSSRefAdaptor::GumboArray>::iterator end = SMemoryBackup.end();
        while (it != end) {
            delete [] *it++;
        }
        SMemoryBackup.clear();
    }
    
}
