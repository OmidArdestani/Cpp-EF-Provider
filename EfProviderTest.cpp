#include <iostream>
#include "EFDrivers/EFDriverSQLight.hpp"
#include "MyModels.h"
#include <AbstractEFProvider.hpp>

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QFile>
#include <QTest>

#define DATABASE_FILE_NAME "MyDBFile.db"

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

private:
    // std::unique_ptr<EFProvider::CEFDriverSQLight<Country>> CountryDBProvider;
    // std::unique_ptr<EFProvider::CEFDriverSQLight<AreaRegion>> AreaRegionDBProvider;
    std::unique_ptr<EFProvider::CEFDriverSQLight<LunchOption>> LunchOptionDBProvider;
    std::unique_ptr<EFProvider::CEFDriverSQLight<Menu>>        MenuDBProvider;
    std::unique_ptr<EFProvider::CEFDriverSQLight<Order>>       OrderDBProvider;
    std::unique_ptr<EFProvider::CEFDriverSQLight<OrderMenu>>   OrderMenuDBProvider;
    std::unique_ptr<EFProvider::CEFDriverSQLight<Transaction>> TransactionDBProvider;
    std::unique_ptr<EFProvider::CEFDriverSQLight<User>>        UserDBProvider;
    std::unique_ptr<EFProvider::CEFDriverSQLight<UserRule>>    UserRuleDBProvider;

private slots:
    void initTestCase()
    {
        using namespace EFProvider;

        QFile::remove(DATABASE_FILE_NAME);

        LunchOptionDBProvider = std::make_unique<EFProvider::CEFDriverSQLight<LunchOption>>();
        MenuDBProvider        = std::make_unique<EFProvider::CEFDriverSQLight<Menu>>       ();
        OrderDBProvider       = std::make_unique<EFProvider::CEFDriverSQLight<Order>>      ();
        OrderMenuDBProvider   = std::make_unique<EFProvider::CEFDriverSQLight<OrderMenu>>  ();
        TransactionDBProvider = std::make_unique<EFProvider::CEFDriverSQLight<Transaction>>();
        UserDBProvider        = std::make_unique<EFProvider::CEFDriverSQLight<User>>       ();
        UserRuleDBProvider    = std::make_unique<EFProvider::CEFDriverSQLight<UserRule>>   ();


        auto sql_connection = new CSQLightWrapper("","MyDBFile.db");
        LunchOptionDBProvider ->Initialize(sql_connection, EDatabaseType::SQLight2);
        MenuDBProvider        ->Initialize(sql_connection, EDatabaseType::SQLight2);
        OrderDBProvider       ->Initialize(sql_connection, EDatabaseType::SQLight2);
        OrderMenuDBProvider   ->Initialize(sql_connection, EDatabaseType::SQLight2);
        TransactionDBProvider ->Initialize(sql_connection, EDatabaseType::SQLight2);
        UserDBProvider        ->Initialize(sql_connection, EDatabaseType::SQLight2);
        UserRuleDBProvider    ->Initialize(sql_connection, EDatabaseType::SQLight2);

    }
    // void initTestCase()
    // {
    //     using namespace EFProvider;

    //     QFile::remove(DATABASE_FILE_NAME);

    //     CountryDBProvider = std::make_unique<CEFDriverSQLight<Country>>();
    //     AreaRegionDBProvider = std::make_unique<CEFDriverSQLight<AreaRegion>>();

    //     auto sql_connection = new CSQLightWrapper("","MyDBFile.db");
    //     CountryDBProvider->Initialize(sql_connection, EDatabaseType::SQLight2);
    //     AreaRegionDBProvider->Initialize(sql_connection, EDatabaseType::SQLight2);
    // }

    // void CreationTest()
    // {
    //     // given
    //     using namespace EFProvider;

    //     SDatabaseRelationship rel;
    //     rel.EFProvider = CountryDBProvider.get();
    //     rel.ForeignKey = "CountryId";
    //     AreaRegionDBProvider->AddRelationship(rel);

    //     // when
    //     Country my_country;
    //     my_country.SetName(std::string("Malaysia"));
    //     my_country.SetDisplay(std::string("Malaysia In East Asia"));

    //     CountryDBProvider->Append(&my_country);
    //     CountryDBProvider->SaveChanges();

    //     AreaRegion my_reg;
    //     my_reg.SetName(std::string("Asia"));
    //     my_reg.SetCountryId(my_country.GetId());

    //     AreaRegionDBProvider->Append(&my_reg);
    //     AreaRegionDBProvider->SaveChanges();

    //     // then
    //     QFile db(DATABASE_FILE_NAME);
    //     QVERIFY(db.exists());
    // }

    // void ToListFunctionTest()
    // {
    //     auto all_area = AreaRegionDBProvider->ToList();

    //     QVERIFY(all_area.size() == 1);
    //     QCOMPARE(all_area.front()->GetId().toInt(), 1);
    //     QCOMPARE(all_area.front()->GetName().toString(), "Asia");
    //     QCOMPARE(all_area.front()->GetCountryId().toInt(), 1);
    //     QCOMPARE(all_area.front()->GetCountry()->GetDisplay().toString(), "Malaysia In East Asia");
    // }

    // void SingleOrDefaultTest()
    // {
    //     auto malaysia_country = CountryDBProvider->SingleOrDefault("Name == 'Malaysia'");

    //     QCOMPARE(malaysia_country->GetId().toInt(), 1);
    //     QCOMPARE(malaysia_country->GetName().toString(), "Malaysia");
    // }

    // void WhereTest()
    // {
    //     auto greater_id = CountryDBProvider->Where("Id > 0");

    //     QVERIFY(greater_id.size() == 1);
    //     QCOMPARE(greater_id.front()->GetId().toInt(), 1);
    //     QCOMPARE(greater_id.front()->GetName().toString(), "Malaysia");
    // }

    // void TopTest()
    // {
    //     auto top_countries = CountryDBProvider->Top(4);

    //     QVERIFY(top_countries.size() == 4);
    //     QCOMPARE(top_countries.front()->GetId().toInt(), 1);
    //     QCOMPARE(top_countries.front()->GetName().toString(), "Malaysia");
    // }

    // void FindTest()
    // {
    //     auto find_id = CountryDBProvider->Find(1);

    //     QCOMPARE(find_id->GetId().toInt(), 1);
    //     QCOMPARE(find_id->GetName().toString(), "Malaysia");
    // }
};

QTEST_MAIN(EFProviderBasicTests)
#include "EfProviderTest.moc"
