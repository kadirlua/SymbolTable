// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Symbols.h"

//Should be only one instance
//TODO: Improve for thread safety
static inline void UpdateCallback(Symbols::SymbolEvent::BaseArgs* args)
{
    if (auto e = dynamic_cast<Symbols::SymbolEvent::OpcServerArgs*>(args); e) {    //if a type of OpcServerArgs...
        std::cout << "OpcServer event UpdateCallback: the value of '" << e->m_symbolName << "' ";
        if (e->m_symbolName == std::string("folder1.folder1a.folder1a1.i"))
            std::cout << "became: " << std::any_cast<int>(*e->m_newVal) << std::endl;
        else if (e->m_symbolName == std::string("folder1.d"))
            std::cout << "became: " << std::any_cast<double>(*e->m_newVal) << std::endl;
        else if (e->m_symbolName == std::string("f"))
            std::cout << "became: " << std::any_cast<float>(*e->m_newVal) << std::endl;
        else
            std::cout << " is unsolicited." << std::endl;
    }
    else if (auto e = dynamic_cast<Symbols::SymbolEvent::TransactionArgs*>(args); e) {    //if a type of TransactionArgs...
        std::cout << "The id " << e->m_deviceTransactionId << " of transaction event UpdateCallback: the value of '" << e->m_symbolName << "' ";
        if (e->m_symbolName == std::string("folder1.folder1a.folder1a1.i"))
            std::cout << "became: " << std::any_cast<int>(*e->m_newVal) << std::endl;
        else if (e->m_symbolName == std::string("folder1.d"))
            std::cout << "became: " << std::any_cast<double>(*e->m_newVal) << std::endl;
        else if (e->m_symbolName == std::string("f"))
            std::cout << "became: " << std::any_cast<float>(*e->m_newVal) << std::endl;
        else
            std::cout << " is unsolicited." << std::endl;
    }
}

class CSymbolTest
{
public:
    CSymbolTest(unsigned int nThreads = std::thread::hardware_concurrency()) 
    {
        std::cout << nThreads << " threads will start working soon" << "\n\n";
        for (unsigned int i = 0; i < nThreads; i++)
            m_VecThreads.emplace_back(std::thread(&CSymbolTest::DummyThreadTest, this, 10 * i, 50 * i + 100));

    }

    ~CSymbolTest() 
    {
        const auto vecSize = m_VecThreads.size();
        //join threads
        for (auto& th : m_VecThreads)
        {
            if (th.joinable())
                th.join();
        }

        //all threads joined
        std::cout << vecSize << " threads joined" << "\n\n";
        std::cout << "SymbolTable size : " << symbols.size() << "\n\n";
        std::cout << "SymbolTable started to be destroyed! Goodbye!" << "\n\n";
    };

    void insertItems()
    {
        symbols.InsertValue("folder1.folder1a.folder1a1.b", OpcUAObjectId::Boolean, true);
        symbols.InsertValue("folder1.folder1a.folder1a1.i", OpcUAObjectId::Integer, 45);
        symbols.InsertValue("folder1.folder1a.folder1a1.d", OpcUAObjectId::Double, 1.2);
        symbols.InsertValue("folder1.folder1a.folder1a1.f", OpcUAObjectId::Float, 45.3f);
        symbols.InsertValue("folder1.folder1a.folder1a1.s", OpcUAObjectId::String, std::string("folder1.folder1a.folder1a1.s"));

        symbols.InsertValue("folder1.b", OpcUAObjectId::Boolean, true);
        symbols.InsertValue("folder1.i", OpcUAObjectId::Integer, 41);
        symbols.InsertValue("folder1.d", OpcUAObjectId::Double, 1.1);
        symbols.InsertValue("folder1.f", OpcUAObjectId::Float, 45.1f);
        symbols.InsertValue("folder1.s", OpcUAObjectId::String, std::string("folder1.s"));

        symbols.InsertValue("folder2.b", OpcUAObjectId::Boolean, false);
        symbols.InsertValue("folder2.i", OpcUAObjectId::Integer, 21);
        symbols.InsertValue("folder2.d", OpcUAObjectId::Double, 2.1);
        symbols.InsertValue("folder2.f", OpcUAObjectId::Float, 25.1f);
        symbols.InsertValue("folder2.s", OpcUAObjectId::String, std::string("folder2.s"));

        symbols.InsertValue("b", OpcUAObjectId::Boolean, false);
        symbols.InsertValue("i", OpcUAObjectId::Integer, 40);
        symbols.InsertValue("d", OpcUAObjectId::Double, 1.01);
        symbols.InsertValue("f", OpcUAObjectId::Float, 45.01f);
        symbols.InsertValue("s", OpcUAObjectId::String, std::string("s"));

        displayValue("folder1.folder1a.folder1a1.b", symbols.GetValue("folder1.folder1a.folder1a1.b"));
        displayValue("folder1.folder1a.folder1a1.i", symbols.GetValue("folder1.folder1a.folder1a1.i"));
        displayValue("folder1.folder1a.folder1a1.d", symbols.GetValue("folder1.folder1a.folder1a1.d"));
        displayValue("folder1.folder1a.folder1a1.f", symbols.GetValue("folder1.folder1a.folder1a1.f"));
        displayValue("folder1.folder1a.folder1a1.s", symbols.GetValue("folder1.folder1a.folder1a1.s"));
        displayValue("folder1.b", symbols.GetValue("folder1.b"));
        displayValue("folder1.i", symbols.GetValue("folder1.i"));
        displayValue("folder1.d", symbols.GetValue("folder1.d"));
        displayValue("folder1.f", symbols.GetValue("folder1.f"));
        displayValue("folder1.s", symbols.GetValue("folder1.s"));
        displayValue("folder2.b", symbols.GetValue("folder2.b"));
        displayValue("folder2.i", symbols.GetValue("folder2.i"));
        displayValue("folder2.d", symbols.GetValue("folder2.d"));
        displayValue("folder2.f", symbols.GetValue("folder2.f"));
        displayValue("folder2.s", symbols.GetValue("folder2.s"));
        displayValue("b", symbols.GetValue("b"));
        displayValue("i", symbols.GetValue("i"));
        displayValue("d", symbols.GetValue("d"));
        displayValue("f", symbols.GetValue("f"));
        displayValue("s", symbols.GetValue("s"));

        std::cout << "\n\n";
    }

    void updateItems()
    {
        std::cout << "Update 'folder1.folder1a.folder1a1.i' with 1000." << std::endl;
        symbols.SetValue("folder1.folder1a.folder1a1.i", 1000);
        std::cout << "Update 'folder1.d' with 123.321." << std::endl;
        symbols.SetValue("folder1.d", 123.321);
        std::cout << "Update 'folder2.f' with 987.654f." << std::endl;
        symbols.SetValue("folder2.f", 987.654f);
        std::cout << "Update 's' with 'string?'." << std::endl;
        symbols.SetValue("s", std::string("string?"));
        std::cout << "Update 'b' with true." << std::endl;
        symbols.SetValue("b", true);

        std::cout << "\n\n";

        displayValue("folder1.folder1a.folder1a1.i", symbols.GetValue("folder1.folder1a.folder1a1.i"));
        displayValue("folder1.d", symbols.GetValue("folder1.d"));
        displayValue("folder2.f", symbols.GetValue("folder2.f"));
        displayValue("s", symbols.GetValue("s"));
        displayValue("b", symbols.GetValue("b"));

        std::cout << "\n\n";

    }

    std::vector<unsigned char> SerializeXML()
    {
        return symbols.SerializeXML();
    }

    static Symbols::SymbolTable symbols;

private:
    void displayValue(std::string path, const Symbols::Symbol& val)
    {
        std::cout << "the value of '" << path << "' is: ";

        switch (val.getObjectId())
        {
        case OpcUAObjectId::Null:
            std::cout << "NULL";
            break;

        case OpcUAObjectId::Boolean:
            std::cout << *val.get<bool>();
            break;

        case OpcUAObjectId::Float:
            std::cout << *val.get<float>();
            break;

        case OpcUAObjectId::Double:
            std::cout << *val.get<double>();
            break;

        case OpcUAObjectId::String:  //it's not a raw string
            std::cout << "'" << *val.get<std::string>() << "'";
            break;

        case OpcUAObjectId::Integer:
            std::cout << *val.get<int>();
            break;

        case OpcUAObjectId::FolderType:
            std::cout << "Folder";
            break;

        }

        std::cout << std::endl;
    }

    void DummyThreadTest(unsigned int iStart, unsigned int iEnd)
    {
        for (unsigned i = iStart; i < iEnd; i++)
        {
            std::string strVal("i");
            strVal.append(std::to_string(i));

            symbols.InsertValue(strVal, OpcUAObjectId::Integer , i);
        }
    }

    protected:
        std::vector<std::thread> m_VecThreads;  //threads into vector
};

class COpcServerSubscriptionTest
{
public:
    COpcServerSubscriptionTest() = default;   //default constructor
    ~COpcServerSubscriptionTest() = default;  //destructor

    void assignEvents()
    {
        CSymbolTest::symbols.AddEvent("folder1.folder1a.folder1a1.i",
                Symbols::SymbolEvent(
                0, 
                Symbols::SymbolEvent::EventType::et_OpcServer, 
                Symbols::SymbolEvent::EventFireType::eft_AnyChange, 
                &UpdateCallback
            ));
        CSymbolTest::symbols.AddEvent("folder1.d", 
            Symbols::SymbolEvent(0, Symbols::SymbolEvent::EventType::et_OpcServer, 
                Symbols::SymbolEvent::EventFireType::eft_AnyChange, 
                &UpdateCallback));

        CSymbolTest::symbols.AddEvent("f", 
            Symbols::SymbolEvent(0, Symbols::SymbolEvent::EventType::et_OpcServer, 
                Symbols::SymbolEvent::EventFireType::eft_AnyChange, 
                &UpdateCallback));
    }
};

//Transaction Test
class CTransactionSubscriptionTest
{
public:
    CTransactionSubscriptionTest() = default;   //default constructor
    ~CTransactionSubscriptionTest() = default;  //destructor

    void assignEvents()
    {
        CSymbolTest::symbols.AddEvent("folder1.folder1a.folder1a1.i",
            Symbols::SymbolEvent(
                15,
                Symbols::SymbolEvent::EventType::et_Transaction,
                Symbols::SymbolEvent::EventFireType::eft_AnyChange,
                &UpdateCallback
            ));
        CSymbolTest::symbols.AddEvent("folder1.d", 
            Symbols::SymbolEvent(5, Symbols::SymbolEvent::EventType::et_Transaction, 
            Symbols::SymbolEvent::EventFireType::eft_AnyChange, 
            &UpdateCallback));

        CSymbolTest::symbols.AddEvent("f", 
            Symbols::SymbolEvent(2, Symbols::SymbolEvent::EventType::et_Transaction, 
            Symbols::SymbolEvent::EventFireType::eft_AnyChange, 
            &UpdateCallback));
    }
};

//i cannot inherit from SymbolTable
//class mySymbolTable : public Symbols::SymbolTable
//{
//public:
//    mySymbolTable() = default;
//    ~mySymbolTable() = default;
//};

Symbols::SymbolTable CSymbolTest::symbols;

int main()
{
    CSymbolTest test;   //send a parameter for how many threads you want to work with
    COpcServerSubscriptionTest opcServerTest;
    //COpcClientSubscriptionTest opcClientTest;
    //CDatabaseSubscriptionTest databaseTest;
    CTransactionSubscriptionTest transactionTest;

    test.insertItems();

    //root item for testing
    auto sym = test.symbols.GetValue("");
    std::cout << "type: " << static_cast<int>(sym.getObjectId()) << "\n\n\n";

    opcServerTest.assignEvents();
    //opcClientTest.assignEvents();
    //databaseTest.assignEvents();
    transactionTest.assignEvents();

    test.updateItems();
    std::cout << "\n\n\n";

    std::vector<unsigned char> input = test.SerializeXML();
    std::string s(input.begin(), input.end());
    std::cout << s << "\n\n\n";

    return 0;
}
