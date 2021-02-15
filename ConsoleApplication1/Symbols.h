// Symbols.h : header file
// 
// Symbols Interface for PLCiManagementConsole App
// Version: 1.3
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
//  Version 1.3:
//  *Removed type parameter from method SetValue. Set value does not create anymore.
//  *Added methods InsertValue and DeleteValue. SetValue no longer inserts a symbol.
//  *GetValue now returns root folder if parameter is empty string
//  *Added CSymbolEvent class, and CSymbol events map. Each Symbol may be assigned one or more events.
//   Event parameter defined as void*, may be one of ????Args classes
//   Args classes will evolve as we go
//  *Added CSymbol.compare()
//  *Added CSymbol.get()

#pragma once
#include "object_ids.h"
#include <any>
#include <functional>
#include <string>
#include <map>
//#include <unordered_map>    //if you do not want to sortable map, uncomment this line.

namespace Symbols {

    class CSymbolEvent
    {
    public:
        enum class EventType {
            et_None = 0,
            // An OPC Server subscribed to a change in the symbol data
            // This happens because an external OPC system asked to be notified.
            et_OpcServer,
            // An OPC Client subscribed to a change in the symbol data
            // This happens because we are configured to notify an external OPC system
            et_OpcClient,
            // A Database subscribed to a change in the symbol data
            // This happens because we are configured to save changes to a database
            et_Database,
            // An External Client subscribed to a change in the symbol data
            // 1. This may happen because a PLCi client such as a form subscibed to show up to date values
            //   forms subscribe when they are on a page that needs the updates, but unsubscribe when leaving the page or the form app terminates
            // 2. This may happen because we are configured to update another PLCi device with up to date values
            et_Transaction,
            et_Coded
        };

        // this enum is used 1) to mark an event, 2) so save symbol update status
        enum class EventFireType {
            // no change in value
            eft_None = 0,
            // execute event when any change in value occurs
            eft_AnyChange,
            // execute event when any value increases
            eft_Increase,
            // execute event when any value decreases
            eft_Decrease
        };

        class OpcServerArgs
        {
        public:
            OpcServerArgs() = default;   //default constructor
            ~OpcServerArgs() = default;  //destructor
            OpcServerArgs(std::string symbolName, OpcUAObjectId objectId, std::any& oldVal, std::any& newVal) :
                m_symbolName(symbolName),
                m_objectId(objectId),
                m_oldVal(oldVal),
                m_newVal(newVal)
            {

            }
            std::string m_symbolName;
            OpcUAObjectId m_objectId{ OpcUAObjectId::Null };
            std::any& m_oldVal;
            std::any& m_newVal;
        };

        class OpcClientArgs
        {
        public:
            OpcClientArgs() = default;   //default constructor
            ~OpcClientArgs() = default;  //destructor
            OpcClientArgs(std::string symbolName, OpcUAObjectId objectId, std::any& oldVal, std::any& newVal) :
                m_symbolName(symbolName),
                m_objectId(objectId),
                m_oldVal(oldVal),
                m_newVal(newVal)
            {

            }
            std::string m_symbolName;
            OpcUAObjectId m_objectId{ OpcUAObjectId::Null };
            std::any& m_oldVal;
            std::any& m_newVal;
        };

        class DatabaseArgs
        {
        public:
            DatabaseArgs() = default;   //default constructor
            ~DatabaseArgs() = default;  //destructor
            DatabaseArgs(std::string symbolName, OpcUAObjectId objectId, std::any& oldVal, std::any& newVal, int transactionId) :
                m_symbolName(symbolName),
                m_objectId(objectId),
                m_oldVal(oldVal),
                m_newVal(newVal),
                m_transactionId(transactionId)
            {

            }
            std::string m_symbolName;
            OpcUAObjectId m_objectId{ OpcUAObjectId::Null };
            std::any& m_oldVal;
            std::any& m_newVal;
            int m_transactionId;
        };

        class TransactionArgs
        {
        public:
            TransactionArgs() = default;   //default constructor
            ~TransactionArgs() = default;  //destructor
            TransactionArgs(std::string symbolName, OpcUAObjectId objectId, std::any& oldVal, std::any& newVal, int deviceTransactionId) :
                m_symbolName(symbolName),
                m_objectId(objectId),
                m_oldVal(oldVal),
                m_newVal(newVal),
                m_deviceTransactionId(deviceTransactionId)
            {

            }
            std::string m_symbolName;
            OpcUAObjectId m_objectId{ OpcUAObjectId::Null };
            std::any& m_oldVal;
            std::any& m_newVal;
            int m_deviceTransactionId;
        };

        using SymbolEvent = std::function<void(void* args)>;

    public:
        CSymbolEvent() = default;   //default constructor
        ~CSymbolEvent() = default;  //destructor
        CSymbolEvent(int eventId, EventType type, EventFireType fireType, const SymbolEvent& callback) :
            m_eventId(eventId),
            m_type(type),
            m_fireType(fireType),
            m_event(callback)
        {

        }

        int getEventId() {
            return m_eventId;
        }

        EventType getEventType() {
            return m_type;
        }

        EventFireType getEventFireType() {
            return m_fireType;
        }

        SymbolEvent m_event;

    private:
        int m_eventId = 0;
        EventType m_type = EventType::et_None;
        EventFireType m_fireType = EventFireType::eft_AnyChange;
    };


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

        CSymbol(OpcUAObjectId id, const std::any& val) :
            m_objectId(id),
            m_value(val)
        {

        }

        CSymbol(OpcUAObjectId id, std::any&& val) :
            m_objectId(id),
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
        void setObjectId(OpcUAObjectId id) noexcept {
            m_objectId = id;
        }

        /*
        *   get the typed object we stored in std::any via metaprogramming.
        *   returns the address of the object we created early, otherwise nullptr.
        */
        template<typename returnType>
        const returnType* get() const noexcept {
            return std::any_cast<returnType>(&m_value);
        }

        /*
        *   get the std::any value we had stored.
        *   returns the address of the object we created early, otherwise nullptr.
        */
        const std::any get() {
            return m_value;
        }

        /*
        *   compare a value with this one.
        *   returns if there was a change and then if it increased or decreased.
        */
        CSymbolEvent::EventFireType compare(const std::any& value) {
            CSymbolEvent::EventFireType result = CSymbolEvent::EventFireType::eft_None;
            switch (m_objectId)
            {
            case OpcUAObjectId::Null:
                break;

            case OpcUAObjectId::Boolean:
                if (std::any_cast<bool>(value) > std::any_cast<bool>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Increase;
                else if (std::any_cast<bool>(value) < std::any_cast<bool>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Decrease;
                else
                    result = CSymbolEvent::EventFireType::eft_None;
                break;

            case OpcUAObjectId::Float:
                if (std::any_cast<float>(value) > std::any_cast<float>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Increase;
                else if (std::any_cast<float>(value) < std::any_cast<float>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Decrease;
                else
                    result = CSymbolEvent::EventFireType::eft_None;
                break;

            case OpcUAObjectId::Double:
                if (std::any_cast<double>(value) > std::any_cast<double>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Increase;
                else if (std::any_cast<double>(value) < std::any_cast<double>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Decrease;
                else
                    result = CSymbolEvent::EventFireType::eft_None;
                break;

            case OpcUAObjectId::String:  //it's not a raw string
                if (std::any_cast<std::string>(value) > std::any_cast<std::string>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Increase;
                else if (std::any_cast<std::string>(value) < std::any_cast<std::string>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Decrease;
                else
                    result = CSymbolEvent::EventFireType::eft_None;
                break;

            case OpcUAObjectId::Integer:
                if (std::any_cast<int>(value) > std::any_cast<int>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Increase;
                else if (std::any_cast<int>(value) < std::any_cast<int>(m_value))
                    result = CSymbolEvent::EventFireType::eft_Decrease;
                else
                    result = CSymbolEvent::EventFireType::eft_None;
                break;

            }
            return result;
        }

        /*
        *   get the type of the object we stored in any.
        *   returns the type of the object we created earlier.
        */
        OpcUAObjectId getObjectId() const noexcept {
            return m_objectId;
        }

        void addEvent(int eventId, CSymbolEvent symbolEvent)
        {
            events.insert(std::make_pair(eventId, symbolEvent));
        }

        void removeEvent(int eventId)
        {
            events.erase(eventId);
        }

        std::map<int, CSymbolEvent> events;

    private:
        OpcUAObjectId m_objectId{ OpcUAObjectId::Null };
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
        *   value: any type of variable to hold into map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool SetValue(std::string name, std::any&& value);

        /*
        *   Add an event to a given name symbol instance.
        *   Params:
        *   name: given name which is key of map.
        *   symbolEvent: a Symbols::CSymbolEvent instance.
        *   Returns: returns true if successful, otherwise false.
        */
        bool AddEvent(std::string name, Symbols::CSymbolEvent symbolEvent);

        /*
        *   Insert a value of a given name symbol instance.
        *   Params:
        *   name: given name which is key of map.
        *   oId: type of variable we send.
        *   value: any type of variable to hold into map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool InsertValue(std::string name, OpcUAObjectId oId, std::any&& value);

        /*
        *   Delete a value of a given name symbol instance.
        *   Params:
        *   name: given name which is key of map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool DeleteValue(std::string name);
    };
}