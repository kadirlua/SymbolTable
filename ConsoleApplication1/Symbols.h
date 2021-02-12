// Symbols.h : header file
// 
// Symbols Interface for PLCiManagementConsole App
// Version: 1.2
// Date: February 2021
// Authors: Kadir ALTINDAG, Suat ARICANLI
// Email: kadir.altindag@aricanli.com.tr, suat@aricanli.com.tr
// Copyright (c) 2021. All Rights Reserved.

// Changelog:
//  Version 1.0:
//  *Initial Release.
//  Version 1.1:
//  *Fix accesing private members of std::map
//  Version 1.2:
//  *Avoid copying data if possible

#pragma once
#include "object_ids.h"
#include <any>
#include <string>
#include <map>
//#include <unordered_map>    //if you do not want to sortable map, uncomment this line.

namespace Symbols {
    /*
    *   the class we created should work any type of variables.
    *   Designed with modern c++.
    *   TODO: improve this class with a return type which is uncertain.
    */

    class CSymbol {
    public:
        CSymbol() = default;   //default constructor
        ~CSymbol() = default;  //destructor

        /*
        *   if you want to noncopyable, just uncomment
        *   but it probably should not work.
        */
        /*CSymbol(const CSymbol& r) = delete;
        CSymbol& operator=(const CSymbol& r) = delete;*/

        CSymbol(ObjectId id, const std::any& val) :
            m_id(id),
            m_value(val)
        {

        }

        CSymbol(ObjectId id, std::any&& val) :
            m_id(id),
            m_value(std::move(val))
        {

        }

        /*
        *   sets the value of the object we stored in any via move semantic.
        *   returns nothing.
        */
        void set(std::any&& value) noexcept {
            m_value = std::move(value);
        }

        /*
        *   sets the value of the object we stored in any via copy semantic.
        *   returns nothing.
        */
        void set(const std::any& value) noexcept {
            m_value = value;
        }

        /*
        *   sets the object id of the object we stored in any.
        *   returns nothing.
        */
        void setObjectId(ObjectId id) noexcept {
            m_id = id;
        }

        /*
        *   get the return type of the object we stored in any
        *   via metaprogramming.
        *   returns the address of the object we created early, otherwise nullptr.
        */
        template<typename returnType>
        const returnType* get() const noexcept {
            return std::any_cast<returnType>(&m_value);
        }

        /*
        *   get the type of the object we stored in any.
        *   returns the type of the object we created early.
        */
        ObjectId getType() const noexcept {
            return m_id;
        }

    private:
        ObjectId m_id{ ObjectId::Null };    //object id
        std::any m_value;   //can be any value of object
    };

    //our map to hold whole datas
    using treeMap = std::map<std::string, CSymbol>;    //sortable map class
    //using treeMap = std::unordered_map<std::string, CSymbol>;    //do not want this for now

    /*
    *   Symbol table class to hold symbol data which is set of unknown variables.
    *   You can set value of an object any time you want but it erases the old one if contains any.
    *   One more thing, there should be something wrong with storing pointers, this class does not guaranteed 
    *   to clean up pointer addresses. So be careful with dynamic memory allocations. 
    */
    class CSymbolTable : public treeMap
    {
    public:
        CSymbolTable() = default;    //default constructor
        ~CSymbolTable() = default;   //destructor

        //noncopyable CSymbolTable interface
        CSymbolTable(const CSymbolTable& r) = delete;
        CSymbolTable& operator=(const CSymbolTable& r) = delete;

    public:
        /*
        *   Get value of a given name symbol instance.
        *   Params:
        *   name: given name which is key of map.
        *   Returns: returns value of map entry, otherwise empty class.
        */
        CSymbol GetValue(std::string name) const;

        /*
        *   Set value of a given name symbol instance.
        *   Params:
        *   name: given name which is key of map.
        *   oId: type of variable we send.
        *   value: any type of variable to hold into map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool SetValue(std::string name, ObjectId oId, std::any&& value);

    };
}