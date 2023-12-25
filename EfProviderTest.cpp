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

class EFProviderBasicTests: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        QFile::remove("MyDBFile.db");
    }
    void creationTest()
    {
        // given
        using namespace EFProvider;
        auto country_db_provider = std::make_unique<CEFDriverSQLight<Country>>();
        auto area_region_db_provider = std::make_unique<CEFDriverSQLight<AreaRegion>>();

        auto sql_connection = std::make_unique<CSQLightWrapper>("","MyDBFile.db");
        country_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);
        area_region_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);

        SDatabaseRelationship rel;
        rel.EFProvider = country_db_provider;
        rel.ForeignKey = "CountryId";
        area_region_db_provider->AddRelationship(rel);

        // when
        country_db_provider->SaveChanges();
        area_region_db_provider->SaveChanges();

        // then
        QFile db("MyDBFile.db");
        QVERIFY(db.exists());
    }

    void addRecordTest()
    {
        using namespace EFProvider;

        // given
        auto country_db_provider = std::make_unique<CEFDriverSQLight<Country>>();
        auto area_region_db_provider = std::make_unique<CEFDriverSQLight<AreaRegion>>();

        auto sql_connection = std::make_unique<CSQLightWrapper>("","MyDBFile.db");
        country_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);
        area_region_db_provider->Initialize(sql_connection, EDatabaseType::SQLight2);

        SDatabaseRelationship rel;
        rel.EFProvider = country_db_provider;
        rel.ForeignKey = "CountryId";
        area_region_db_provider->AddRelationship(rel);

        // when
        Country my_country;
        my_country.SetName(std::string("Malaysia"));
        my_country.SetDisplay(std::string("Malaysia In East Asia"));

        country_db_provider->Append(&my_country);
        country_db_provider->SaveChanges();

        AreaRegion my_reg;
        my_reg.SetName(std::string("Asia"));
        my_reg.SetCountryId(my_country.id);

        area_region_db_provider->Append(&my_reg);
        area_region_db_provider->SaveChanges();

        // then
        auto all_area = country_db_provider->ToList();
        QVERIFY(all_area.size() == 1);
        QCOMPARE(all_area.at(0)->GetId().toInt(), 0);
        QCOMPARE(all_area.at(0)->GetName().toString(), "Asia");
        QCOMPARE(all_area.at(0)->GetCountryId().toInt(), 0);
        QCOMPARE(all_area.at(0)->GetCountry()->GetDisplay().toString(), "Malaysia");
    }
};
