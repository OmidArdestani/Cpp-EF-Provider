#ifndef MYMODELS_H
#define MYMODELS_H
#include "DatabaseModel.h"

#define BUILD_PROPERY(name)  public: void Set##name(CVariant value){name = value.toInt();} CVariant Get##name(){return name;}
#define DEFAULT_REGISTER_PROPERTY(type, prop_name, class_name) BUILD_PROPERY(prop_name) REGISTER_PROPERTY(type, prop_name, class_name, &class_name::Set##prop_name, &class_name::Get##prop_name)


class LunchOption  :public CAbstractDatabaseModel
{
    DEFAULT_REGISTER_PROPERTY(std::string, MealName ,LunchOption);
    DEFAULT_REGISTER_PROPERTY(std::string, Description ,LunchOption);
};

class Menu  :public CAbstractDatabaseModel
{
    DEFAULT_REGISTER_PROPERTY(std::string, Name ,Menu);
    DEFAULT_REGISTER_PROPERTY(std::string, Date ,Menu);
};

class Order:public CAbstractDatabaseModel
{
    DEFAULT_REGISTER_PROPERTY(int, UserId ,Order);
    DEFAULT_REGISTER_PROPERTY(int, OrderMenuId ,Order);
    DEFAULT_REGISTER_PROPERTY(std::string, Description ,Order);
};

class OrderMenu  :public CAbstractDatabaseModel
{
    DEFAULT_REGISTER_PROPERTY(int, LunchOptionId ,OrderMenu);
    DEFAULT_REGISTER_PROPERTY(int, MenuId ,OrderMenu);
    DEFAULT_REGISTER_PROPERTY(double, Price ,OrderMenu);
    DEFAULT_REGISTER_PROPERTY(std::string, Description ,OrderMenu);
};

class Transaction  :public CAbstractDatabaseModel
{
    DEFAULT_REGISTER_PROPERTY(int, UserId ,Transaction);
    DEFAULT_REGISTER_PROPERTY(double, Value ,Transaction);
    DEFAULT_REGISTER_PROPERTY(std::string, Date ,Transaction);
    DEFAULT_REGISTER_PROPERTY(std::string, Description ,Transaction);
};

class User :public CAbstractDatabaseModel
{
    DEFAULT_REGISTER_PROPERTY(int, UserRuleId ,User);
    DEFAULT_REGISTER_PROPERTY(std::string, UserName ,User);
    DEFAULT_REGISTER_PROPERTY(std::string, FirstName ,User);
    DEFAULT_REGISTER_PROPERTY(double, Balance ,User);
};

class UserRule  :public CAbstractDatabaseModel
{
    DEFAULT_REGISTER_PROPERTY(std::string, Name ,UserRule);
    DEFAULT_REGISTER_PROPERTY(std::string, Description ,UserRule);
};
#endif // MYMODELS_H
