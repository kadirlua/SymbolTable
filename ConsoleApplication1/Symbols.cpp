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
                    if (it->second.getType() == ObjectId::FolderType)
                    {
                        // advance
                        m = m->find(s)->second.get<treeMap>();
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

    bool CSymbolTable::SetValue(std::string name, ObjectId oId, std::any&& value)
    {
        //copy elision for name should work for c++ 17
        bool bRet = true;
        bool lastSubs = false;
        treeMap* m = this;
        std::istringstream f(name);
        std::string s;

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
                if (lastSubs && oId != ObjectId::FolderType)
                {
                    m->insert(std::make_pair(s, CSymbol(oId, std::move(value))));
                }
                else // if folder type add folder
                {
                    m->insert(std::make_pair(s, CSymbol(ObjectId::FolderType, treeMap())));
                    m = const_cast<treeMap*>(m->find(s)->second.get<treeMap>());
                }
            }
            else // substring found
            {
                if (lastSubs)
                {
                    if (it->second.getType() == oId)
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
                    if (it->second.getType() == ObjectId::FolderType)
                    {
                        // advance
                        m = (treeMap*)m->find(s)->second.get<treeMap>();
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