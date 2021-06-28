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
//  *Added SymbolEvent class, and Symbol events map. Each Symbol may be assigned one or more events.
//   Event parameter defined as void*, may be one of ????BaseArgs classes
//   BaseArgs classes will evolve as we go
//  *Added Symbol.compare()
//  *Added Symbol.get()
//  Version 1.4:
//  *Added thread safe implementation for std::map
//  *Events work properly now
//  *Removed move semantic for SymbolTable methods which do not make any sense.
//  *Added some tests for transaction and thread safety.
//  *Improved event args by inheritance
//  *Symbol::compare moved into cpp source file.
//  *Some other improvements.
//  Version 1.5:
//  *Added SymbolTable::SerializeXML() returns vector of unsigned char.


#pragma once
#include <any>
#include <functional>
#include "ThreadSafeMap.h"
#include <tinyxml2/tinyxml2.h>

namespace Symbols {
    enum class SymbolType {
        st_Null = 0,
        st_Boolean = 1,
        st_SByte = 2,
        st_Byte = 3,
        st_Int16 = 4,
        st_UInt16 = 5,
        st_Int32 = 6,
        st_UInt32 = 7,
        st_Int64 = 8,
        st_UInt64 = 9,
        st_Float = 10,
        st_Double = 11,
        st_String = 12,
        st_DateTime = 13,
        st_Guid = 14,
        //ByteString = 15,
        //XmlElement = 16,
        //NodeId = 17,
        //ExpandedNodeId = 18,
        //StatusCode = 19,
        //QualifiedName = 20,
        //LocalizedText = 21,
        //ExtensionObject = 22,
        //DataValue = 23,
        //Variant = 24,
        //DiagnosticInfo = 25,
        st_WideString = 25,
        st_Number = 26,
        st_Integer = 27,
        st_UInteger = 28,
        //st_FolderType = 61
    };

    class Symbol;   //incomplete type declaration

    //our map to hold whole datas
    using treeMap = aricanli::container::ThreadSafeMap<int, Symbol>;    //sortable map class

    class SymbolEvent
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

        //the base class for sending args between events
        class BaseArgs {
        public:
            BaseArgs() = default;
            virtual ~BaseArgs() {};    //This makes BaseArgs a polymorphic type

            std::string m_symbolName;
            SymbolType m_type{ SymbolType::st_Null };
            const std::any* m_oldVal = nullptr;
            const std::any* m_newVal = nullptr;
        };

        class OpcServerArgs : public BaseArgs
        {
        public:
            OpcServerArgs() = default;   //default constructor
            ~OpcServerArgs() = default;  //destructor
            OpcServerArgs(std::string symbolName, SymbolType type, const std::any& oldVal, const std::any& newVal)
            {
                m_symbolName = symbolName;
                m_type = type;
                m_oldVal = &oldVal;
                m_newVal = &newVal;
            }
        };

        class OpcClientArgs : public BaseArgs
        {
        public:
            OpcClientArgs() = default;   //default constructor
            ~OpcClientArgs() = default;  //destructor
            OpcClientArgs(std::string symbolName, SymbolType type, const std::any& oldVal, const std::any& newVal)
            {
                m_symbolName = symbolName;
                m_type = type;
                m_oldVal = &oldVal;
                m_newVal = &newVal;
            }
        };

        class DatabaseArgs : public BaseArgs
        {
        public:
            DatabaseArgs() = default;   //default constructor
            ~DatabaseArgs() = default;  //destructor
            DatabaseArgs(std::string symbolName, SymbolType type, const std::any& oldVal, const std::any& newVal, int transactionId) :
                m_transactionId(transactionId)
            {
                m_symbolName = symbolName;
                m_type = type;
                m_oldVal = &oldVal;
                m_newVal = &newVal;
            }

            int m_transactionId{};
        };

        class TransactionArgs : public BaseArgs
        {
        public:
            TransactionArgs() = default;   //default constructor
            ~TransactionArgs() = default;  //destructor
            TransactionArgs(std::string symbolName, SymbolType type, const std::any& oldVal, const std::any& newVal, int deviceTransactionId) :
                m_deviceTransactionId(deviceTransactionId)
            {
                m_symbolName = symbolName;
                m_type = type;
                m_oldVal = &oldVal;
                m_newVal = &newVal;
            }

            int m_deviceTransactionId{};
        };

        using symbol_event_t = std::function<void(BaseArgs*)>;

    public:
        SymbolEvent() = default;   //default constructor
        ~SymbolEvent() = default;  //destructor
        SymbolEvent(int eventId, EventType type, EventFireType fireType, const symbol_event_t& callback) :
            m_eventId(eventId),
            m_type(type),
            m_fireType(fireType),
            m_event(std::bind(callback, std::placeholders::_1))
        {

        }

        int getEventId() const noexcept {
            return m_eventId;
        }

        EventType getEventType() const noexcept {
            return m_type;
        }

        EventFireType getEventFireType() const noexcept {
            return m_fireType;
        }

        symbol_event_t m_event;

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

    class Symbol {
    public:
        Symbol() = default;   //default constructor
        ~Symbol() = default;  //destructor

        /*
        *   if you want to noncopyable, just uncomment
        *   but it probably should not work.
        */
        /*Symbol(const Symbol& r) = delete;
        Symbol& operator=(const Symbol& r) = delete;*/

        Symbol(uint32_t id, std::string name, SymbolType type, const std::any& val) :
            m_id(id),
            m_name(name),
            m_type(type),
            m_value(val)
        {

        }

        Symbol(uint32_t id, std::string name, SymbolType type, std::any&& val) :
            m_id(id),
            m_name(name),
            m_type(type),
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
        *   get the name of the symbol.
        *   returns the name of an object we created earlier.
        */
        std::string getName() const noexcept {
            return m_name;
        }

        /*
        *   sets the object type of the object we stored in any.
        *   returns nothing.
        */
        void setType(SymbolType type) noexcept {
            m_type = type;
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
        *   returns the object itself for reading.
        */
        const std::any& get() const noexcept {
            return m_value;
        }

        /*
        *   compare a value with this one.
        *   returns if there was a change and then if it increased or decreased.
        */
        SymbolEvent::EventFireType compare(const std::any& value) const;

        /*
        *   get the type of the object we stored in any.
        *   returns the type of the object we created earlier.
        */
        SymbolType getType() const noexcept {
            return m_type;
        }

        /*
        *   get the id of the object we stored in any.
        *   returns the type of the object we created earlier.
        */
        uint32_t getId() const noexcept {
            return m_id;
        }

        void addEvent(int eventId, SymbolEvent symbolEvent)
        {
            events.insert(std::make_pair(eventId, symbolEvent));
        }

        void removeEvent(int eventId)
        {
            events.erase(eventId);
        }

        aricanli::container::ThreadSafeMap<int, SymbolEvent> events;

    private:
        SymbolType m_type{ SymbolType::st_Null };
        uint32_t m_id{};
        std::string m_name;
        std::any m_value;   //can be any value of object

    };

    /*
    *   Symbol table class to hold symbol data which is set of unknown variables.
    *   You can set value of an object any time you want but it erases the old one if contains any.
    *   One more thing, there should be something wrong with storing pointers, this class does not guaranteed
    *   to clean up pointer addresses. So be careful with dynamic memory allocations.
    */
    class SymbolTable final : public treeMap
    {
        static inline constexpr auto XML_ELEMENT_SYMBOLTABLE = "symboltable";
        static inline constexpr auto XML_ELEMENT_FOLDER = "folder";
        static inline constexpr auto XML_ELEMENT_SYMBOL = "symbol";
        static inline constexpr auto XML_ELEMENT_NAME = "name";
        static inline constexpr auto XML_ELEMENT_TYPE = "type";
        static inline constexpr auto XML_ELEMENT_ID = "id";

    public:
        SymbolTable() = default;    //default constructor
        ~SymbolTable() = default;   //destructor

        //noncopyable SymbolTable interface
        SymbolTable(const SymbolTable& r) = delete;
        SymbolTable& operator=(const SymbolTable& r) = delete;

    public:
        /*
        *   Get value of a symbol instance by name.
        *   Params:
        *   name: Symbol name.
        *   Returns: returns value of map entry, otherwise empty class.
        */
        Symbol GetValue(std::string name) const;

        /*
        *   Get value of a symbol instance by Id.
        *   Params:
        *   id: Symbol Id which is the key of the map.
        *   Returns: returns value of map entry, otherwise empty class.
        */
        Symbol GetValue(int id) const;

        /*
        *   Set value of a symbol instance by name.
        *   Params:
        *   name: Symbol name.
        *   value: any type of variable to hold into map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool SetValue(std::string name, std::any value);

        /*
        *   Set value of a symbol instance by Id.
        *   Params:
        *   id: Symbol Id which is the key of the map.
        *   value: any type of variable to hold into map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool SetValue(int id, std::any value);

        /*
        *   Add an event to a symbol instance by name.
        *   Params:
        *   name: Symbol name.
        *   symbolEvent: a Symbols::SymbolEvent instance.
        *   Returns: returns true if successful, otherwise false.
        */
        bool AddEvent(std::string name, Symbols::SymbolEvent symbolEvent);

        /*
        *   Add an event to a symbol instance by Id.
        *   Params:
        *   id: Symbol Id which is the key of the map.
        *   symbolEvent: a Symbols::SymbolEvent instance.
        *   Returns: returns true if successful, otherwise false.
        */
        bool AddEvent(int id, Symbols::SymbolEvent symbolEvent);

        /*
        *   Insert a symbol.
        *   Params:
        *   id: Symbol id.
        *   name: Symbol name.
        *   type: type of variable we send.
        *   value: any type of variable to hold into map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool InsertValue(int id, std::string name, SymbolType type, std::any value);

        /*
        *   Insert a symbol by name. Converts string parameter to any<type>.
        *   Params:
        *   name: Symbol name.
        *   type: type of variable we send.
        *   value: any type of variable to hold into map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool InsertFromStringValue(int id, std::string name, SymbolType type, std::string value);

        /*
        *   Delete a symbol by name.
        *   Params:
        *   name: Symbol name.
        *   Returns: returns true if successful, otherwise false.
        */
        bool DeleteValue(std::string name);

        /*
        *   Delete a symbol by Id.
        *   Params:
        *   id: Symbol Id which is the key of the map.
        *   Returns: returns true if successful, otherwise false.
        */
        bool DeleteValue(int id);

        /*
        *   Serialize the symbol table to XML.
        *   Params: None
        *   Returns: std::vector<unsigned char> containing the XML.
        */
        std::vector<unsigned char> SerializeXML() const;

    private:
        //void recurseFolders(const treeMap* folder, const std::unique_ptr<tinyxml2::XMLDocument>& doc,
        //    tinyxml2::XMLNode* pNode) const;

        int getSymbolIdByName(std::string name) const;

    };
}