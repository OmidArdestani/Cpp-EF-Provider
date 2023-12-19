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

void ToListTest(EFProvider::CEFDriverSQLight<AreaRegion>* area_regin_ef)
{
    qDebug() <<"----------------------------------------";
    qDebug() <<"---------------ToList Query-------------";
    qDebug() <<"----------------------------------------";
    auto all_area = area_regin_ef->ToList();

    qDebug() <<"Area ID |"<<"Area Name |"<<"Country Id |"<<"Country Display";
    foreach (auto item, all_area) {
        qDebug() << item->GetId().toInt() << "\t"
                 << QString::fromStdString(item->GetName().toString()) << "\t"
                 << item->GetCountryId().toInt() << "\t"
                 << QString::fromStdString(item->GetCountry()->GetDisplay().toString());
    }

}

void SingleOrDefaultTest(EFProvider::CEFDriverSQLight<Country>* coutry_ef)
{
    qDebug() <<"----------------------------------------";
    qDebug() <<"----------Single Or Default Query-------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto malaysia_country = coutry_ef->SingleOrDefault("Name == 'Malaysia'");

    qDebug() << malaysia_country->GetId().toInt() << "\t"
             << QString::fromStdString(malaysia_country->GetName().toString()) << "\t"
             << QString::fromStdString(malaysia_country->GetDisplay().toString());
}

void WhereTest(EFProvider::CEFDriverSQLight<Country>* coutry_ef)
{
    qDebug() <<"----------------------------------------";
    qDebug() <<"---------------Where Query--------------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto greater_id = coutry_ef->Where("Id > 4");

    foreach (auto item, greater_id) {
        qDebug() << item->GetId().toInt() << "\t"
                 << QString::fromStdString(item->GetName().toString()) << "\t"
                 << QString::fromStdString(item->GetDisplay().toString());
    }

}

void TopTest(EFProvider::CEFDriverSQLight<Country>* coutry_ef)
{
    qDebug() <<"----------------------------------------";
    qDebug() <<"-----------------Top Query--------------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto top_countries = coutry_ef->Top(4);

    foreach (auto item, top_countries) {
        qDebug() << item->GetId().toInt() << "\t"
                 << QString::fromStdString(item->GetName().toString()) << "\t"
                 << QString::fromStdString(item->GetDisplay().toString());
    }
}

void FindTest(EFProvider::CEFDriverSQLight<Country>* coutry_ef)
{
    qDebug() <<"----------------------------------------";
    qDebug() <<"-----------------Find Query-------------";
    qDebug() <<"----------------------------------------";
    qDebug() << "Country Id |" << "Country Name |" << "Country Display";
    auto find_id = coutry_ef->Find(4);

    qDebug() << find_id->GetId().toInt() << "\t"
             << QString::fromStdString(find_id->GetName().toString()) << "\t"
             << QString::fromStdString(find_id->GetDisplay().toString());
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
    //----------------Feed data test-------------
    //-------------------------------------------

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

    //-------------------------------------------
    //-----------------Query test----------------
    //-------------------------------------------
    SingleOrDefaultTest(country_db_provider);
    WhereTest(country_db_provider);
    TopTest(country_db_provider);
    FindTest(country_db_provider);
}
