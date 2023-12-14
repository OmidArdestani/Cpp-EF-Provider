# C++ Entity Framework Provider

[![Build Status](https://github.com/Omid91a/CppEFProvider/actions/workflows/msbuild.yml/badge.svg)](https://github.com/Omid91a/CppEFProvider/actions)


## Description

The C++ Entity Framework Provider is a versatile database provider for C++ that follows a code-first and model-based approach. Inspired by C# Entity Framework (EF) Core, our library provides a cross-platform, open-source, lightweight, and extensible solution for C++ developers to work seamlessly with various relational databases using model objects. This eliminates the need for boilerplate data access code, enabling a more efficient and expressive development experience.
## Features

- **Code-First Approach:** Define your database schema using C++ code.
- **Model-Based:** Utilize models to represent database entities and relationships.
- **Cross-Platform:** Enjoy the benefits of a cross-platform solution for database interaction.
- **Support for Multiple Databases:** Works seamlessly with various relational databases, including:
  - SQLite -> Done
  - MSSQL -> To Do
  - MySQL -> To Do
  - PostgreSQL -> Done
  - Microsoft Access -> To Do

- **Data Shaping:** Shape your data according to your needs.
- **Data Editing and Validation:** Simplify data editing and validation processes.
- **Searching and Filtering:** Efficiently search and filter data.
- **Layout Customization:** Customize the layout of your database interactions.

## Getting Started

### Prerequisites

Ensure you have the following installed:

- C++ 20 compiler

### Usage
1- First create your database model as a class.

```bash
class Country :public CAbstractDatabaseModel
{
    REGISTER_PROPERTY(std::string, Name, Country, &Country::SetName, &Country::GetName)
    REGISTER_PROPERTY(std::string, Display, Country, &Country::SetDisplay, &Country::GetDisplay)

public:
    Country() : CAbstractDatabaseModel(){}
    void SetName(CVariant name) { Name = name.toString(); }
    CVariant GetName() {return Name; }
    void SetDisplay(CVariant display) { Display = display.toString(); }
    CVariant GetDisplay() {return Display; }
};

class AreaRegion:public CAbstractDatabaseModel
{
    REGISTER_PROPERTY(std::string, name, AreaRegion, &AreaRegion::SetName, &AreaRegion::GetName)
    REGISTER_PROPERTY(int, CountryId, AreaRegion, &AreaRegion::SetCountryId, &AreaRegion::GetCountryId)

public:
    AreaRegion():CAbstractDatabaseModel(){}

    void SetName(CVariant name){this->name = name.toString();}
    CVariant GetName(){return this->name;}

    void SetCountryId(CVariant id){this->CountryId = id.toInt();}
    CVariant GetCountryId(){return this->CountryId;}
    Country* GetCountry(){return GetRelationship<Country>("CountryId");}
};
```
2- Create an object of database you want to work with.
```bash
    using namespace EFProvider;

    // construct db providers
    auto country_db_provider = new CEFDriverSQLight<Country>();
    auto area_region_db_provider = new CEFDriverSQLight<AreaRegion>();

    // initialize db connection for providers
    auto sql_connection = new CSQLightWrapper("","MyDBFile.db");
    country_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);
    area_region_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);
```
  * To add relationship between two table:
```bash
    // initialize relationships
    SDatabaseRelationship rel;
    rel.EFProvider = country_db_provider;
    rel.ForeignKey = "CountryId";
    area_region_db_provider->AddRelationship(rel);
```
  * In this case, I have used [QSqlDatabase](https://doc.qt.io/qt-6/qsqldatabase.html) component that exist in Qt-Creator.
  * I have created a wrapper to working with [QSqlDatabase](https://doc.qt.io/qt-6/qsqldatabase.html) and [QSqlQuery](https://doc.qt.io/qt-6/qsqlquery.html).
```bash
namespace EFProvider{

class CQSqlQueryWrapper : public CAbstracSqlDataBaseQuery
{
public:
    CQSqlQueryWrapper(QSqlQuery* q) : LocalQuery(q) {}

    bool Next() override
    {
        return LocalQuery->next();
    }

    CVariant Value(int index) override
    {
        auto v = LocalQuery->value(index);
        return CVariant(v.toString().toStdString());
    }

    CVariant Value(std::string key) override
    {
        auto v = LocalQuery->value(QString::fromStdString(key));
        return CVariant(v.toString().toStdString());
    }


private:
    QSqlQuery *LocalQuery;
};

class CSQLightWrapper : public CAbstractSQLDatabase
{
public:
    CSQLightWrapper(std::string db_path,std::string database_name) : CAbstractSQLDatabase()
        ,LastQuery(new QSqlQuery)
    {
        // create db file
        QString db_address = QString::fromStdString(database_name);
        if(db_path != "")
            db_address = QString::fromStdString(db_path + "\\" + database_name);

        QFile f(db_address);
        if(!f.open(QIODevice::ReadWrite))
            assert(false);
        f.close();

        // initialize db file
        LocalDatabase = QSqlDatabase::addDatabase("QSQLITE");
        LocalDatabase.setHostName(db_address);
        LocalDatabase.setDatabaseName(QString::fromStdString(database_name));
    }

public:
    bool Open() override {return LocalDatabase.open();}
    bool Close() override {LocalDatabase.close(); return true;}
    CAbstracSqlDataBaseQuery* Execute(std::string cmd) override
    {
        *LastQuery = LocalDatabase.exec(QString::fromStdString(cmd));
        return new CQSqlQueryWrapper(LastQuery);
    }

private:
    QSqlDatabase LocalDatabase;
    QSqlQuery* LastQuery = nullptr;
};
}
```
3- work with the EF-Object and then use SaveChanges() to apply changes in the database.
```bash
    // Add a country in the Countrys table
    Country my_country;
    my_country.SetName(std::string("Malaysia"));
    my_country.SetDisplay(std::string("Malaysia In East Asia"));

    country_db_provider->Append(&my_country);
    country_db_provider->SaveChanges();

    // Add a region in the AreaRegions table
    AreaRegion my_reg;
    my_reg.SetName(std::string("Asia"));
    my_reg.SetCountryId(my_country.id);

    area_region_db_provider->Append(&my_reg);
    area_region_db_provider->SaveChanges();
```
* After running this code, an SQLight file will be created named "MyDBFile.db", and a table will be created in the database named "Country".
After running the section 3 code, a row will be added to the "Country" table.

### Testing Functionality
  * I have used Qt for testing this module.
    
  1- ToList() Query
```bash
    auto all_area = area_region_db_provider->ToList();
    qDebug() <<"----------------------------------------";
    qDebug() <<"---------------ToList Query-------------";
    qDebug() <<"----------------------------------------";
    qDebug() <<"Area ID |"<<"Area Name |"<<"Country Id |"<<"Country Display";
    foreach (auto item, all_area) {
        qDebug() << item->GetId().toInt() << "\t"
                 << QString::fromStdString(item->GetName().toString()) << "\t"
                 << item->GetCountryId().toInt() << "\t"
                 << QString::fromStdString(item->GetCountry()->GetDisplay().toString());
    }
```
  2- SingleOrDefault() Query
```bash
    qDebug() <<"----------------------------------------";
    qDebug() <<"----------Single Or Default Query-------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto malaysia_country = country_db_provider->SingleOrDefault("Name == 'Malaysia'");

    qDebug() << malaysia_country->GetId().toInt() << "\t"
             << QString::fromStdString(malaysia_country->GetName().toString()) << "\t"
             << QString::fromStdString(malaysia_country->GetDisplay().toString());

```
  3- Where() Query
```bash
    qDebug() <<"----------------------------------------";
    qDebug() <<"---------------Where Query--------------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto greater_id = country_db_provider->Where("Id > 4");

    foreach (auto item, greater_id) {
        qDebug() << item->GetId().toInt() << "\t"
                 << QString::fromStdString(item->GetName().toString()) << "\t"
                 << QString::fromStdString(item->GetDisplay().toString());
    }

```
  4- Top() Query
```bash
    qDebug() <<"----------------------------------------";
    qDebug() <<"-----------------Top Query--------------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto top_countries = country_db_provider->Top(4);

    foreach (auto item, top_countries) {
        qDebug() << item->GetId().toInt() << "\t"
                 << QString::fromStdString(item->GetName().toString()) << "\t"
                 << QString::fromStdString(item->GetDisplay().toString());
    }

```
  5- Find() Query
```bash
    qDebug() <<"----------------------------------------";
    qDebug() <<"-----------------Find Query-------------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto find_id = country_db_provider->Find(4);

    qDebug() << find_id->GetId().toInt() << "\t"
             << QString::fromStdString(find_id->GetName().toString()) << "\t"
             << QString::fromStdString(find_id->GetDisplay().toString());
```

## Library Class Diagram (UML)
![EF-Provider Class Diagram](https://github.com/Omid91a/CppEFProvider/blob/main/CppEFProviderLib/Class%20Driagram.png)

