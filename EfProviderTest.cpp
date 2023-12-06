#include <iostream>
#include "EFProviderDrives.hpp"

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
    void SetName(CVariant name) { Name = "Test";/*name.toType<std::string>();*/ }
    CVariant GetName() {
        return Name; }
    void SetDisplay(CVariant display) { Display = "Test D";/*display.toType<std::string>();*/ }
    CVariant GetDisplay() {
        return Display; }
};


namespace EFprovider{

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
    CSQLightWrapper(std::string db_address,std::string database_name) : CAbstractSQLDatabase()
        ,LastQuery(new QSqlQuery)
    {
        // create db file
        QFile f(QString::fromStdString(db_address));
        if(f.open(QIODevice::ReadWrite));
        // initialize db file
        LocalDatabase = QSqlDatabase::addDatabase("QSQLITE");
        LocalDatabase.setHostName(QString::fromStdString(db_address));
        LocalDatabase.setDatabaseName(QString::fromStdString(database_name));
        LocalDatabase.open();

        QString cmd = "create database " + QString::fromStdString(database_name) + ";";
        LocalDatabase.exec(cmd);
        LocalDatabase.close();
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
    using namespace EFprovider;
    auto db_provider = new CSQLightEFprovider<MCountry>("MyDBFile.db");
    db_provider->Initialize(new CSQLightWrapper("MyDBFile.db","MyDB"),EDatabaseType::SQLight2);

    QVariant aa;

    MCountry my_country;
    my_country.SetName(CVariant()/*"Malaysia"*/);
    my_country.SetDisplay(CVariant()/*"Malaysia In East Asia"*/);

    db_provider->Append(&my_country);
    db_provider->SaveChanges();

    std::cout << "Hello World!\n";
}
