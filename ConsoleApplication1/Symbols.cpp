// Symbols.cpp : implementation file
// 
// Symbols Interface for PLCiManagementConsole App
// Version: 1.4
// Date: February 2021
// Authors: Kadir ALTINDAG, Suat ARICANLI
// Email: kadir.altindag@aricanli.com.tr, suat@aricanli.com.tr
// Copyright (c) 2021. All Rights Reserved.

#include "Symbols.h"
#include <sstream>

namespace Symbols {

    SymbolEvent::EventFireType Symbol::compare(const std::any& value) const {
        SymbolEvent::EventFireType result{ SymbolEvent::EventFireType::eft_None };
        switch (m_objectId)
        {
        case OpcUAObjectId::Null:
            break;

        case OpcUAObjectId::Boolean:
        {
            const auto& b = *get<bool>();
            auto comp = std::any_cast<bool>(value);
            if (comp > b)
                result = SymbolEvent::EventFireType::eft_Increase;
            else if (comp < b)
                result = SymbolEvent::EventFireType::eft_Decrease;
            else
                result = SymbolEvent::EventFireType::eft_None;
        }
        break;

        case OpcUAObjectId::Float:
        {
            const auto& f = *get<float>();
            auto comp = std::any_cast<float>(value);
            if (comp > f)
                result = SymbolEvent::EventFireType::eft_Increase;
            else if (comp < f)
                result = SymbolEvent::EventFireType::eft_Decrease;
            else
                result = SymbolEvent::EventFireType::eft_None;
        }
        break;

        case OpcUAObjectId::Double:
        {
            const auto& d = *get<double>();
            auto comp = std::any_cast<double>(value);
            if (comp > d)
                result = SymbolEvent::EventFireType::eft_Increase;
            else if (comp < d)
                result = SymbolEvent::EventFireType::eft_Decrease;
            else
                result = SymbolEvent::EventFireType::eft_None;
        }
        break;

        case OpcUAObjectId::String:  //it's not a raw string
        {
            const auto& s = *get<std::string>();
            auto comp = std::any_cast<std::string>(value);
            if (comp > s)
                result = SymbolEvent::EventFireType::eft_Increase;
            else if (comp < s)
                result = SymbolEvent::EventFireType::eft_Decrease;
            else
                result = SymbolEvent::EventFireType::eft_None;
        }
        break;

        case OpcUAObjectId::Integer:
        {
            const auto& i = *get<int>();
            auto comp = std::any_cast<int>(value);
            if (comp > i)
                result = SymbolEvent::EventFireType::eft_Increase;
            else if (comp < i)
                result = SymbolEvent::EventFireType::eft_Decrease;
            else
                result = SymbolEvent::EventFireType::eft_None;
        }
        break;

        }
        return result;
    }

    Symbol SymbolTable::GetValue(std::string name) const
    {
        //copy elision for name should work for c++ 17
        Symbol bRet;

        bool lastSubs = false;
        const treeMap* m = this;
        std::istringstream f(name);
        std::string s;

        if (name.length() == 0)
            return Symbol(OpcUAObjectId::FolderType, m);

        // split name into substrings and search
        while (getline(f, s, '.'))
        {
            // is substring same as string or is substring last substring
            if (name == s || name.rfind("." + s) == name.length() - s.length() - 1)
                lastSubs = true;

            auto it = m->find(s);
            // substring not found? error
            if (it == m->cend())
            {
                break;
            }
            else // substring found
            {
                if (lastSubs)
                {
                    // return the value
                    bRet = it->second;
                }
                else
                {
                    if (it->second.getObjectId() == OpcUAObjectId::FolderType)
                    {
                        // advance
                        m = it->second.get<treeMap>();
                    }
                    else
                    {
                        // problem: Symbol not a folder
                        break;
                    }
                }
            }
        }
        return bRet;
    }

    bool SymbolTable::AddEvent(std::string name, Symbols::SymbolEvent symbolEvent)
    {
        //copy elision for name should work for c++ 17
        bool bRet = true;

        bool lastSubs = false;
        treeMap* m = this;
        std::istringstream f(name);
        std::string s;

        if (name.length() == 0)
            return false;

        // split name into substrings and search
        while (getline(f, s, '.'))
        {
            // is substring same as string or is substring last substring
            if (name == s || name.rfind("." + s) == name.length() - s.length() - 1)
                lastSubs = true;

            auto it = m->find(s);
            // substring not found? error
            if (it == m->end())
            {
                bRet = false;
                break;
            }
            else // substring found
            {
                if (lastSubs)
                {
                    // add the event
                    it->second.addEvent(symbolEvent.getEventId(), symbolEvent);
                }
                else
                {
                    if (it->second.getObjectId() == OpcUAObjectId::FolderType)
                    {
                        // advance
                        m = const_cast<treeMap*>(it->second.get<treeMap>());
                    }
                    else
                    {
                        // problem: Symbol not a folder
                        bRet = false;
                        break;
                    }
                }
            }
        }
        return bRet;
    }

    bool SymbolTable::SetValue(std::string name, std::any value)
    {
        //copy elision for name should work for c++ 17
        bool bRet = true;
        bool lastSubs = false;
        treeMap* m = this;
        std::istringstream f(name);
        std::string s;

        if (name.length() == 0)
            return false;

        // split name into substrings and search
        while (getline(f, s, '.'))
        {
            // is substring same as string or is substring last substring
            if (name == s || name.rfind("." + s) == name.length() - s.length() - 1)
                lastSubs = true;

            auto it = m->find(s);
            // substring not found? error
            if (it == m->end())
            {
                bRet = false;
                break;
            }
            else // substring found
            {
                if (lastSubs)
                {
                    // 1: get the current value
                    const std::any& oldVal = it->second.get();

                    // 2: determine how the value changed
                    Symbols::SymbolEvent::EventFireType theChange = it->second.compare(value);

                    // 3: update with new value
                    it->second.set(value);

                    if (theChange != Symbols::SymbolEvent::EventFireType::eft_None)
                    {
                        for (auto itm = it->second.events.begin(); itm != it->second.events.end(); itm++)
                        {
                            // 4: SATISFY Symbols::SymbolEvent::EventFireType
                            if (itm->second.getEventFireType() != Symbols::SymbolEvent::EventFireType::eft_AnyChange &&
                                itm->second.getEventFireType() != theChange)
                                continue;

                            // 5: construct arguments for specified event type and fire event
                            if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_OpcServer)
                            {
                                Symbols::SymbolEvent::OpcServerArgs arg(name, it->second.getObjectId(), oldVal, value);
                                itm->second.m_event(&arg);
                            }
                            else if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_OpcClient)
                            {
                                Symbols::SymbolEvent::OpcClientArgs arg(name, it->second.getObjectId(), oldVal, value);
                                itm->second.m_event(&arg);
                            }
                            else if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_Database)
                            {
                                Symbols::SymbolEvent::DatabaseArgs arg(name, it->second.getObjectId(), oldVal, value, 1);
                                itm->second.m_event(&arg);
                            }
                            else if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_Transaction)
                            {
                                Symbols::SymbolEvent::TransactionArgs arg(name, it->second.getObjectId(), oldVal, value, 1);
                                itm->second.m_event(&arg);
                            }
                        }
                    }
                }
                else
                {
                    if (it->second.getObjectId() == OpcUAObjectId::FolderType)
                    {
                        // advance
                        m = const_cast<treeMap*>(it->second.get<treeMap>());
                    }
                    else
                    {
                        // problem: Symbol not a folder
                        bRet = false;
                        break;
                    }
                }
            }
        }
        return bRet;
    }

    bool SymbolTable::InsertValue(std::string name, OpcUAObjectId oId, std::any value)
    {
        //copy elision for name should work for c++ 17
        bool bRet = true;
        bool lastSubs = false;
        treeMap* m = this;
        std::istringstream f(name);
        std::string s;

        if (name.length() == 0)
            return false;

        // split name into substrings and search
        while (getline(f, s, '.'))
        {
            // is substring same as string or is substring last substring
            if (name == s || name.rfind("." + s) == name.length() - s.length() - 1)
                lastSubs = true;

            auto it = m->find(s);
            // substring not found? create
            if (it == m->end())
            {
                // if base variable type add variable
                if (lastSubs && oId != OpcUAObjectId::FolderType)
                {
                    m->insert(std::make_pair(s, Symbol(oId, value)));
                }
                else // if folder type add folder
                {
                    m->insert(std::make_pair(s, Symbol(OpcUAObjectId::FolderType, treeMap{})));
                    m = const_cast<treeMap*>(m->find(s)->second.get<treeMap>());
                }
            }
            else // substring found
            {
                if (lastSubs)
                {
                    if (it->second.getObjectId() == oId)
                    {
                        // update value
                        it->second.set(value);
                    }
                    else
                    {
                        // problem: Symbol already defined
                        bRet = false;
                        break;
                    }
                }
                else
                {
                    if (it->second.getObjectId() == OpcUAObjectId::FolderType)
                    {
                        // advance
                        m = const_cast<treeMap*>(it->second.get<treeMap>());
                    }
                    else
                    {
                        // problem: Symbol not a folder
                        bRet = false;
                        break;
                    }
                }
            }
        }
        return bRet;
    }

    bool SymbolTable::DeleteValue(std::string name)
    {
        //copy elision for name should work for c++ 17
        bool bRet = true;
        bool lastSubs = false;
        treeMap* m = this;
        std::istringstream f(name);
        std::string s;

        if (name.length() == 0)
            return false;

        // split name into substrings and search
        while (getline(f, s, '.'))
        {
            // is substring same as string or is substring last substring
            if (name == s || name.rfind("." + s) == name.length() - s.length() - 1)
                lastSubs = true;

            auto it = m->find(s);
            // substring not found? error
            if (it == m->end())
            {
                bRet = false;
                break;
            }
            else // substring found
            {
                if (lastSubs)
                {
                    m->erase(it);
                }
                else
                {
                    if (it->second.getObjectId() == OpcUAObjectId::FolderType)
                    {
                        // advance
                        m = const_cast<treeMap*>(it->second.get<treeMap>());
                    }
                    else
                    {
                        // problem: Symbol not a folder
                        bRet = false;
                        break;
                    }
                }
            }
        }
        return bRet;
    }

    std::vector<unsigned char> SymbolTable::SerializeXML() const
    {
        tinyxml2::XMLElement* root = nullptr, * pElm = nullptr;
        
        std::vector<unsigned char> charVec;
        const treeMap* m = this;

        auto pDoc = std::make_unique<tinyxml2::XMLDocument>();
        tinyxml2::XMLNode* pRoot = pDoc->NewElement(XML_ELEMENT_SYMBOLTABLE);
        pDoc->InsertFirstChild(pRoot);

        for (auto it = m->cbegin(); it != m->cend(); it++)
        {
            if (it->second.getObjectId() == OpcUAObjectId::FolderType)
            {
                pElm = pDoc->NewElement(XML_ELEMENT_FOLDER);
                pElm->SetAttribute(XML_ELEMENT_NAME, it->first.c_str());
                recurseFolders(it->second.get<treeMap>(), pDoc, pElm);
                pRoot->LinkEndChild(pElm);
            }
            else
            {
                pElm = pDoc->NewElement(XML_ELEMENT_SYMBOL);
                pElm->SetAttribute(XML_ELEMENT_NAME, it->first.c_str());
                pElm->SetAttribute(XML_ELEMENT_TYPE, static_cast<int>(it->second.getObjectId()));
                pRoot->LinkEndChild(pElm);
            }
        }

        //print
        tinyxml2::XMLPrinter printer;
        pDoc->Accept(&printer);

        const char* chIn = printer.CStr();

        const char* end = chIn + strlen(chIn);

        charVec.insert(charVec.end(), chIn, end);
        
        return charVec;
    }

    void SymbolTable::recurseFolders(const treeMap* m, const std::unique_ptr<tinyxml2::XMLDocument>& doc, 
        tinyxml2::XMLNode* pNode) const
    {
        tinyxml2::XMLElement* pElm;

        for (auto it = m->cbegin(); it != m->cend(); it++)
        {
            if (it->second.getObjectId() == OpcUAObjectId::FolderType)
            {
                pElm = doc->NewElement(XML_ELEMENT_FOLDER);
                pElm->SetAttribute(XML_ELEMENT_NAME, it->first.c_str());
                recurseFolders(it->second.get<treeMap>(), doc, pElm);
                pNode->LinkEndChild(pElm);
            }
            else
            {
                pElm = doc->NewElement(XML_ELEMENT_SYMBOL);
                pElm->SetAttribute(XML_ELEMENT_NAME, it->first.c_str());
                pElm->SetAttribute(XML_ELEMENT_TYPE, static_cast<int>(it->second.getObjectId()));
                pNode->LinkEndChild(pElm);
            }
        }
    }
}