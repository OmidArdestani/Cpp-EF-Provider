// Add this line at the top of your source file
#pragma execution_character_set("utf-8")
#pragma warning(disable : 5033 4996)

#include <iostream>
#include "EFDrivers/EFDriverSQLight.hpp"
#include <AbstractEFProvider.hpp>

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QFile>

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


namespace EFProvider{

class QSqlQueryWrapper : public CAbstracSqlDataBaseQuery
{
public:
    QSqlQueryWrapper(QSqlQuery* q) : LocalQuery(q) {}

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
        return new QSqlQueryWrapper(LastQuery);
    }

private:
    QSqlDatabase LocalDatabase;
    QSqlQuery* LastQuery = nullptr;
};
}

int main()
{
    using namespace EFProvider;

    // construct db providers
    auto country_db_provider = new CEFDriverSQLight<Country>();
    auto area_region_db_provider = new CEFDriverSQLight<AreaRegion>();

    // initialize db connection for providers
    auto sql_connection = new CSQLightWrapper("","MyDBFile.db");
    country_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);
    area_region_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);

    // initialize relationships
    SDatabaseRelationship rel;
    rel.EFProvider = country_db_provider;
    rel.ForeignKey = "CountryId";
    area_region_db_provider->AddRelationship(rel);

    //-------------------------------------------
    //-------------------------------------------
    //-------------------------------------------

//    Country my_country;
//    my_country.SetName(std::string("Malaysia"));
//    my_country.SetDisplay(std::string("Malaysia In East Asia"));

//    country_db_provider->Append(&my_country);
//    country_db_provider->SaveChanges();

//    AreaRegion my_reg;
//    my_reg.SetName(std::string("Asia"));
//    my_reg.SetCountryId(my_country.id);

//    area_region_db_provider->Append(&my_reg);
//    area_region_db_provider->SaveChanges();

    auto all_area = area_region_db_provider->ToList();
    qDebug() <<"----------------------------------------";
    qDebug() <<"Area ID |"<<"Area Name |"<<"Country Id |"<<"Country Display";
    foreach (auto item, all_area) {
        qDebug() << item->GetId().toInt() << "\t"
                 << QString::fromStdString(item->GetName().toString()) << "\t"
                 << item->GetCountryId().toInt() << "\t"
                 << QString::fromStdString(item->GetCountry()->GetDisplay().toString());
    }

    std::cout << "Hello World!\n";
}
