// Symbols.cpp : implementation file
// 
// Symbols Interface for PLCiManagementConsole App
// Version: 1.0
// Date: February 2021
// Authors: Kadir ALTINDAG, Suat ARICANLI
// Email: kadir.altindag@aricanli.com.tr, suat@aricanli.com.tr
// Copyright (c) 2021. All Rights Reserved.

#include "Symbols.h"
#include <sstream>

namespace Symbols {

    CSymbol CSymbolTable::GetValue(std::string name) const
    {
        //copy elision for name should work for c++ 17
        CSymbol bRet;

        bool lastSubs = false;
        const treeMap* m = this;
        std::istringstream f(name);
        std::string s;

        if (name.length() == 0)
            return CSymbol(OpcUAObjectId::FolderType, m);

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

    bool CSymbolTable::AddEvent(std::string name, Symbols::CSymbolEvent symbolEvent)
    {
        //copy elision for name should work for c++ 17
        bool bRet = true;

        bool lastSubs = false;
        const treeMap* m = this;
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
                    ((CSymbol)it->second).addEvent(symbolEvent.getEventId(), symbolEvent);
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
                        bRet = false;
                        break;
                    }
                }
            }
        }
        return bRet;
    }

    bool CSymbolTable::SetValue(std::string name, std::any&& value)
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
                    std::any oldVal = it->second.get();

                    // 2: determine how the value changed
                    Symbols::CSymbolEvent::EventFireType theChange = it->second.compare(value);

                    // 3: update with new value
                    it->second.set(std::move(value));

                    if (theChange != Symbols::CSymbolEvent::EventFireType::eft_None)
                    {
                        std::map<int, CSymbolEvent>::iterator itm;
                        for (itm = it->second.events.begin(); itm != it->second.events.end(); itm++)
                        {
                            // 4: SATISFY Symbols::CSymbolEvent::EventFireType
                            if (itm->second.getEventFireType() != Symbols::CSymbolEvent::EventFireType::eft_AnyChange &&
                                itm->second.getEventFireType() != theChange)
                                continue;

                            // 5: construct arguments for specified event type and fire event
                            if (itm->second.getEventType() == Symbols::CSymbolEvent::EventType::et_OpcServer)
                            {
                                Symbols::CSymbolEvent::OpcServerArgs arg = Symbols::CSymbolEvent::OpcServerArgs(name, it->second.getObjectId(), oldVal, value);
                                itm->second.m_event(&arg);
                            }
                            else if (itm->second.getEventType() == Symbols::CSymbolEvent::EventType::et_OpcClient)
                            {
                                Symbols::CSymbolEvent::OpcClientArgs arg = Symbols::CSymbolEvent::OpcClientArgs(name, it->second.getObjectId(), oldVal, value);
                                itm->second.m_event(&arg);
                            }
                            else if (itm->second.getEventType() == Symbols::CSymbolEvent::EventType::et_Database)
                            {
                                Symbols::CSymbolEvent::DatabaseArgs arg = Symbols::CSymbolEvent::DatabaseArgs(name, it->second.getObjectId(), oldVal, value, 1);
                                itm->second.m_event(&arg);
                            }
                            else if (itm->second.getEventType() == Symbols::CSymbolEvent::EventType::et_Transaction)
                            {
                                Symbols::CSymbolEvent::TransactionArgs arg = Symbols::CSymbolEvent::TransactionArgs(name, it->second.getObjectId(), oldVal, value, 1);
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
                        m = (treeMap*)it->second.get<treeMap>();
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

    bool CSymbolTable::InsertValue(std::string name, OpcUAObjectId oId, std::any&& value)
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
                    m->insert(std::make_pair(s, CSymbol(oId, std::move(value))));
                }
                else // if folder type add folder
                {
                    m->insert(std::make_pair(s, CSymbol(OpcUAObjectId::FolderType, treeMap())));
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
                        it->second.set(std::move(value));
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
                        m = (treeMap*)it->second.get<treeMap>();
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

    bool CSymbolTable::DeleteValue(std::string name)
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
                        m = (treeMap*)it->second.get<treeMap>();
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
}