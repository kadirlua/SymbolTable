// Symbols.cpp : implementation file
// 
// Symbols Interface for PLCiManagementConsole App
// Version: 1.0
// Date: February 2021
// Authors: Kadir ALTINDAG, Suat ARICANLI
// Email: kadir.altindag@aricanli.com.tr, suat@aricanli.com.tr
// Copyright (c) 2021. All Rights Reserved.

#include "Symbols.h"
#include <iostream>
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
                std::cout << "Symbol '" << s << "' not found, return empty." << std::endl;
                break;
            }
            else // substring found
            {
                if (lastSubs)
                {
                    // return the value
                    bRet = it->second;
                    std::cout << "Symbol '" << s << "' found, return CSymbol." << std::endl;
                }
                else
                {
                    if (it->second.getType() == ObjectId::FolderType)
                    {
                        // advance
                        m = m->find(s)->second.get<treeMap>();
                        std::cout << "Symbol '" << s << "' is a folder, advancing." << std::endl;
                    }
                    else
                    {
                        // problem: Symbol not a folder
                        std::cout << "Symbol '" << s << "' not a folder, return empty." << std::endl;
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
                    std::cout << "Symbol '" << s << "' not found, inserting variable and setting value" << std::endl;
                }
                else // if folder type add folder
                {
                    m->insert(std::make_pair(s, CSymbol(ObjectId::FolderType, treeMap())));
                    m = const_cast<treeMap*>(m->find(s)->second.get<treeMap>());
                    std::cout << "Symbol '" << s << "' not found, inserting folder and advancing." << std::endl;
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
                        std::cout << "Symbol '" << s << "' already defined, value has been updated, exiting." << std::endl;
                    }
                    else
                    {
                        // problem: Symbol already defined
                        std::cout << "Symbol '" << s << "' already defined with different type, aborting." << std::endl;
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
                        std::cout << "Symbol '" << s << "' is a folder, advancing." << std::endl;
                    }
                    else
                    {
                        // problem: Symbol not a folder
                        std::cout << "Symbol '" << s << "' not a folder, aborting." << std::endl;
                        bRet = false;
                        break;
                    }
                }
            }
        }
        return bRet;
    }
}