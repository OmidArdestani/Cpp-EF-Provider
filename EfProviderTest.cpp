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

class MCountry :public CAbstractDatabaseModel
{
    REGISTER_PROPERTY(std::string, Name, MCountry, &MCountry::SetName, &MCountry::GetName);
    REGISTER_PROPERTY(std::string, Display, MCountry, &MCountry::SetDisplay, &MCountry::GetDisplay);

public:
    MCountry() : CAbstractDatabaseModel(){}
    void SetName(CVariant name) { Name = name.toString(); }
    CVariant GetName() {return Name; }
    void SetDisplay(CVariant display) { Display = display.toString(); }
    CVariant GetDisplay() {return Display; }
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
    };

private:
    QSqlDatabase LocalDatabase;
    QSqlQuery* LastQuery = nullptr;
};
}

int main()
{
    using namespace EFProvider;
    auto db_provider = new CEFDriverSQLight<MCountry>("MyDBFile.db");
    db_provider->Initialize(new CSQLightWrapper("","MyDBFile.db"),EDatabaseType::SQLight2);

    QVariant aa;

    MCountry my_country;
    my_country.SetName(std::string("Malaysia"));
    my_country.SetDisplay(std::string("Malaysia In East Asia"));

    db_provider->Append(&my_country);
    db_provider->SaveChanges();

    std::cout << "Hello World!\n";
}
