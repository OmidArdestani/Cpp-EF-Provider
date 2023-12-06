# C++ Entity Framework Provider

[![Build Status](https://travis-ci.org/your-username/cpp-ef-provider.svg?branch=master)](https://travis-ci.org/your-username/cpp-ef-provider)

## Description

The C++ Entity Framework Provider is a versatile database provider for C++ that follows a code-first and model-based approach. Inspired by C# Entity Framework (EF) Core, our library provides a cross-platform, open-source, lightweight, and extensible solution for C++ developers to work seamlessly with various relational databases using model objects. This eliminates the need for boilerplate data access code, enabling a more efficient and expressive development experience.
## Features

- **Code-First Approach:** Define your database schema using C++ code.
- **Model-Based:** Utilize models to represent database entities and relationships.
- **Cross-Platform:** Enjoy the benefits of a cross-platform solution for database interaction.
- **Support for Multiple Databases:** Works seamlessly with various relational databases, including:
  - SQLite
  - MSSQL
  - MySQL
  - PostgreSQL
  - Microsoft Access

- **Data Shaping:** Shape your data according to your needs.
- **Data Editing and Validation:** Simplify data editing and validation processes.
- **Searching and Filtering:** Efficiently search and filter data.
- **Layout Customization:** Customize the layout of your database interactions.

## Getting Started

### Prerequisites

Ensure you have the following installed:

- C++ 17 compiler

### Usage
1- First create your database model as a class.

```bash
class MCountry :public CAbstractDatabaseModel
{
    REGISTER_PROPERTY(std::string, Name, MCountry, &MCountry::SetName, &MCountry::GetName);
    REGISTER_PROPERTY(std::string, Display, MCountry, &MCountry::SetDisplay, &MCountry::GetDisplay);

public:
    MCountry() : CAbstractDatabaseModel(){}
    void SetName(CVariant name) { Name = name; }
    CVariant GetName() {
        return Name; }
    void SetDisplay(CVariant display) { Display  = display; }
    CVariant GetDisplay() {
        return Display; }
};
```
2- Create an object of database you want to work with.
```bash
using namespace EFprovider;
auto db_provider = new CSQLightEFprovider<MCountry>("MyDBFile.db");
db_provider->Initialize(new CSQLightWrapper("MyDBFile.db","MyDB"),EDatabaseType::SQLight2);
```
3- work with the EF-Object and then save that to apply changes in the database.
```bash
MCountry my_country;
my_country.SetName("Malaysia");
my_country.SetDisplay("Malaysia In East Asia");

db_provider->Append(&my_country);
db_provider->SaveChanges();
```
* After running this code, an SQLight file will be created named "MyDBFile.db", and a table will be created in the database named "MCountry".
After running the section 3 code, a row will be added to the "MCountry" table.

## Library Class Diagram (UML)
![EF-Provider Class Diagram](https://github.com/Omid91a/CppEFProvider/blob/main/CppEFProviderLib/Class%20Driagram.png)

