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

typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;

namespace Symbols {

    SymbolEvent::EventFireType Symbol::compare(const std::any& value) const {
        SymbolEvent::EventFireType result{ SymbolEvent::EventFireType::eft_None };
        switch (m_type)
        {
        case SymbolType::st_Null:
            break;

        case SymbolType::st_Boolean:
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

        case SymbolType::st_Float:
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

        case SymbolType::st_Double:
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

        case SymbolType::st_String:  //it's not a raw string
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

        case SymbolType::st_Integer:
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

    Symbol SymbolTable::GetValue(uint32_t id) const
    {
        Symbol bRet;
        auto it = find(id);
        if (it != cend())
            bRet = it->second;
        return bRet;
    }

    Symbol SymbolTable::GetValue(std::string name) const
    {
        Symbol bRet;
        int index = getSymbolIdByName(name);
        if (index > 0)
        {
            bRet = GetValue(index);
        }
        return bRet;
    }

    bool SymbolTable::AddEvent(uint32_t id, Symbols::SymbolEvent symbolEvent)
    {
        bool bRet = false;
        auto it = find(id);
        if (it != end())
        {
            bRet = true;
            it->second.addEvent(symbolEvent.getEventId(), symbolEvent);
        }
        return bRet;
    }

    bool SymbolTable::AddEvent(std::string name, Symbols::SymbolEvent symbolEvent)
    {
        int index = getSymbolIdByName(name);
        if (index > 0)
        {
            return AddEvent(index, symbolEvent);
        }
        return false;
    }

    bool SymbolTable::SetValue(uint32_t id, std::any value)
    {
        bool bRet = false;
        auto it = find(id);
        if (it != end())
        {
            // 1: get the current value
            const std::any& oldVal = it->second.get();

            // 2: determine how the value changed
            Symbols::SymbolEvent::EventFireType theChange = it->second.compare(value);

            // 3: update with new value
            it->second.set(value);

            if (theChange != Symbols::SymbolEvent::EventFireType::eft_None)
            {
                //for (auto itm = it->second.events.begin(); itm != it->second.events.end(); itm++)
                //{
                //    // 4: SATISFY Symbols::SymbolEvent::EventFireType
                //    if (itm->second.getEventFireType() != Symbols::SymbolEvent::EventFireType::eft_AnyChange &&
                //        itm->second.getEventFireType() != theChange)
                //        continue;

                //    // 5: construct arguments for specified event type and fire event
                //    if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_OpcServer)
                //    {
                //        Symbols::SymbolEvent::OpcServerArgs arg(name, it->second.getType(), oldVal, value);
                //        itm->second.m_event(&arg);
                //    }
                //    else if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_OpcClient)
                //    {
                //        Symbols::SymbolEvent::OpcClientArgs arg(name, it->second.getType(), oldVal, value);
                //        itm->second.m_event(&arg);
                //    }
                //    else if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_Database)
                //    {
                //        Symbols::SymbolEvent::DatabaseArgs arg(name, it->second.getType(), oldVal, value, 1);
                //        itm->second.m_event(&arg);
                //    }
                //    else if (itm->second.getEventType() == Symbols::SymbolEvent::EventType::et_Transaction)
                //    {
                //        Symbols::SymbolEvent::TransactionArgs arg(name, it->second.getType(), oldVal, value, 1);
                //        itm->second.m_event(&arg);
                //    }
                //}
            }
            bRet = true;
        }
        return bRet;
    }

    bool SymbolTable::SetValue(std::string name, std::any value)
    {
        bool bRet = false;
        int index = getSymbolIdByName(name);
        if (index > 0)
        {
            bRet = SetValue(index, value);
        }
        return bRet;
    }

    bool SymbolTable::InsertFromStringValue(uint32_t id, std::string name, std::string desc,
        SymbolType type, std::string value)
    {
        int is;
        std::string val;
        std::any anyVal;

        if (value.length() == 0) val = "0";
        else val = value;

        switch (type)
        {
        case SymbolType::st_Boolean:
            anyVal = true;
            if (val.substr(0, 1) == "0") anyVal = false;
            break;
        case SymbolType::st_SByte:
        case SymbolType::st_Byte:
            anyVal = val[0];
            break;
        case SymbolType::st_Int16:
            is = static_cast<int>(std::strtol(val.c_str(), nullptr, 0));
            if (is > SHRT_MAX)
                anyVal = static_cast<short>(SHRT_MAX);
            else if (is < SHRT_MIN)
                anyVal = static_cast<short>(SHRT_MIN);
            else
                anyVal = static_cast<short>(is);
            break;
        case SymbolType::st_UInt16:
            is = static_cast<int>(std::strtol(val.c_str(), nullptr, 0));
            if (is > USHRT_MAX)
                anyVal = static_cast<unsigned short>(USHRT_MAX);
            else if (is < 0)
                anyVal = static_cast<unsigned short>(0);
            else
                anyVal = static_cast<unsigned short>(is);
            break;
        case SymbolType::st_Integer:
        case SymbolType::st_Int32:
            anyVal = static_cast<int>(std::strtol(val.c_str(), nullptr, 0));
            break;
        case SymbolType::st_UInteger:
        case SymbolType::st_UInt32:
            anyVal = static_cast<unsigned int>(std::strtoul(val.c_str(), nullptr, 0));
            break;
        case SymbolType::st_Int64:
            anyVal = static_cast<long long>(std::strtoll(val.c_str(), nullptr, 0));
            break;
        case SymbolType::st_UInt64:
            anyVal = static_cast<unsigned long long>(std::strtoull(val.c_str(), nullptr, 0));
            break;
        case SymbolType::st_Number:
        case SymbolType::st_Float:
            anyVal = static_cast<float>(std::strtof(val.c_str(), nullptr));
            break;
        case SymbolType::st_Double:
            anyVal = static_cast<float>(std::strtod(val.c_str(), nullptr));
            break;
        case SymbolType::st_String:
            anyVal = val;
            break;
        case SymbolType::st_DateTime:
            anyVal = static_cast<unsigned long long>(std::strtoull(val.c_str(), nullptr, 0));
            break;
        case SymbolType::st_Guid:
            GUID guid;
            sscanf_s(val.c_str(),
                "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
                &guid.Data1, &guid.Data2, &guid.Data3,
                &guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
                &guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);
            anyVal = guid;
            break;
        case SymbolType::st_WideString:
            anyVal = val;
            break;
        case SymbolType::st_Null:
        default:
            anyVal = val;
            break;
        }

        return InsertValue(id, name, desc, type, anyVal);
    }

    bool SymbolTable::InsertValue(uint32_t id, std::string name, std::string desc, SymbolType type, std::any value)
    {
        auto result = emplace(id, Symbol{ id, name, desc, type, value });
        return result.second;
    }

    bool SymbolTable::DeleteValue(std::string name)
    {
        int index = getSymbolIdByName(name);
        if (index > 0)
        {
            return DeleteValue(index);
        }
        return false;
    }

    bool SymbolTable::DeleteValue(uint32_t id)
    {
        auto it = find(id);
        if (it != end())
        {
            erase(it);
            return true;
        }
        return false;
    }

    int SymbolTable::getSymbolIdByName(std::string name) const noexcept
    {
        for (const auto& item : *this)
        {
            if (item.second.getName() == name)
                return item.first;
        }
        return 0;
    }

    std::vector<unsigned char> SymbolTable::SerializeXML() const
    {
        tinyxml2::XMLElement* root = nullptr, * pElm = nullptr;

        std::vector<unsigned char> charVec;

        std::map<std::string, Symbol> nameMap;
        std::map<std::string, Symbol>::iterator itn;

        auto pDoc = std::make_unique<tinyxml2::XMLDocument>();
        tinyxml2::XMLNode* pRoot = pDoc->NewElement(XML_ELEMENT_SYMBOLTABLE);
        tinyxml2::XMLNode* pTemp, *pSearch;
        pDoc->InsertFirstChild(pRoot);

        for (const auto& item : *this)
        {
            nameMap.emplace(item.second.getName(), item.second);
        }

        for (auto itn = nameMap.cbegin(); itn != nameMap.cend(); itn++)
        {
            bool lastSubs = false;
            std::istringstream f(itn->first);
            std::string s;

            if (itn->first.length() == 0)
                continue;

            pTemp = pRoot;

            // split name into substrings and search
            while (getline(f, s, '.'))
            {
                // is substring same as string or is substring last substring
                if (itn->first == s || itn->first.rfind('.' + s) == itn->first.length() - s.length() - 1)
                    lastSubs = true;

                pSearch = pTemp->FirstChildElement(s.c_str());

                // substring not found? create
                if (!pSearch)
                {
                    // if base variable type add variable
                    if (lastSubs)
                    {
                        pElm = pDoc->NewElement(XML_ELEMENT_SYMBOL);
                        pElm->SetAttribute(XML_ELEMENT_ID, itn->second.getId());
                        pElm->SetAttribute(XML_ELEMENT_NAME, s.c_str());
                        pElm->SetAttribute(XML_ELEMENT_DESC, itn->second.getDescription().c_str());
                        pElm->SetAttribute(XML_ELEMENT_TYPE, static_cast<int>(itn->second.getType()));
                        pTemp->LinkEndChild(pElm);
                    }
                    else // if folder type add folder
                    {
                        pElm = pDoc->NewElement(XML_ELEMENT_FOLDER);
                        pElm->SetAttribute(XML_ELEMENT_NAME, s.c_str());
                        pTemp->LinkEndChild(pElm);
                        pTemp = pElm;
                    }
                }
                else // substring found
                {
                    if (!lastSubs)
                    {
                        pTemp = pSearch;
                    }
                }
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
}