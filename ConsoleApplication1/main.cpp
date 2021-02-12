// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Symbols.h"

void displayValue(std::string path, const Symbols::CSymbol& val)
{
    std::cout << "the value of '" << path << "' is: ";

    switch (val.getType())
    {
    case ObjectId::Null:
        std::cout << "NULL";
        break;

    case ObjectId::Boolean:
        std::cout << *val.get<bool>();
        break;

    case ObjectId::Float:
        std::cout << *val.get<float>();
        break;

    case ObjectId::Double:
        std::cout << *val.get<double>();
        break;

    case ObjectId::String:  //it's not a raw string
        std::cout << "'" << *val.get<std::string>() << "'";
        break;

    case ObjectId::Integer:
        std::cout << *val.get<int>();
        break;

    case ObjectId::FolderType:
        std::cout << "Folder";
        break;

    }

    std::cout << std::endl;
}

int main()
{
    Symbols::CSymbolTable symbols;

    symbols.SetValue("folder1.folder1a.folder1a1.b", ObjectId::Boolean, true);
    symbols.SetValue("folder1.folder1a.folder1a1.i", ObjectId::Integer, 45);
    symbols.SetValue("folder1.folder1a.folder1a1.d", ObjectId::Double, 1.2);
    symbols.SetValue("folder1.folder1a.folder1a1.f", ObjectId::Float, 45.3f);
    symbols.SetValue("folder1.folder1a.folder1a1.s", ObjectId::String, std::string("folder1.folder1a.folder1a1.s"));

    symbols.SetValue("folder1.b", ObjectId::Boolean, true);
    symbols.SetValue("folder1.i", ObjectId::Integer, 41);
    symbols.SetValue("folder1.d", ObjectId::Double, 1.1);
    symbols.SetValue("folder1.f", ObjectId::Float, 45.1f);
    symbols.SetValue("folder1.s", ObjectId::String, std::string("folder1.s"));

    symbols.SetValue("folder2.b", ObjectId::Boolean, false);
    symbols.SetValue("folder2.i", ObjectId::Integer, 21);
    symbols.SetValue("folder2.d", ObjectId::Double, 2.1);
    symbols.SetValue("folder2.f", ObjectId::Float, 25.1f);
    symbols.SetValue("folder2.s", ObjectId::String, std::string("folder2.s"));

    symbols.SetValue("b", ObjectId::Boolean, false);
    symbols.SetValue("i", ObjectId::Integer, 40);
    symbols.SetValue("d", ObjectId::Double, 1.01);
    symbols.SetValue("f", ObjectId::Float, 45.01f);
    symbols.SetValue("s", ObjectId::String, std::string("s"));

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

    std::cout << "Update 'folder1.folder1a.folder1a1.i' with 1000." << std::endl;
    symbols.SetValue("folder1.folder1a.folder1a1.i", ObjectId::Integer, 1000);
    std::cout << "Update 'folder1.d' with 123.321." << std::endl;
    symbols.SetValue("folder1.d", ObjectId::Double, 123.321);
    std::cout << "Update 'folder2.f' with 987.654f." << std::endl;
    symbols.SetValue("folder2.f", ObjectId::Double, 987.654f);
    std::cout << "Update 's' with 'string?'." << std::endl;
    symbols.SetValue("s", ObjectId::String, std::string("string?"));
    std::cout << "Update 'b' with true." << std::endl;
    symbols.SetValue("b", ObjectId::Boolean, true);

    std::cout << "\n\n";

    displayValue("folder1.folder1a.folder1a1.i", symbols.GetValue("folder1.folder1a.folder1a1.i"));
    displayValue("folder1.d", symbols.GetValue("folder1.d"));
    displayValue("folder2.f", symbols.GetValue("folder2.f"));
    displayValue("s", symbols.GetValue("s"));
    displayValue("b", symbols.GetValue("b"));

    std::cout << "\n\n" << "Exiting" << "\n\n";

    return 0;
}
