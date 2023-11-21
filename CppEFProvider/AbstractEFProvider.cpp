#include "AbstractEFProvider.h"

using namespace EFprovider;

template<class T>
inline void CAbstractEFProvider<T>::Initialize(CAbstractSQLDatabase database_object, EDatabaseType type)
{
}

template<class T>
inline void CAbstractEFProvider<T>::Append(T& item)
{
    ItemsToInsert.push_back(item);
}

template<class T>
inline void CAbstractEFProvider<T>::Remove(T& item)
{
    ItemsToDelete.push_back(item);
}

template<class T>
void CAbstractEFProvider<T>::Update(T& item)
{
    ItemsToUpdate.push_back(item);
}

template<class T>
void CAbstractEFProvider<T>::SaveChanges()
{
    //while (!DatabaseObject.Open()) { std::thread::msleep(500); }

    // check for delete
    for each(T * item in ItemsToUpdate)
    {
        UpdateExecute(item);
        UpdateRelationships(item);
    }

    // check for insert
    for each (T * item in ItemsToInsert)
        CreateExecute(item);

    for each (T * item in ItemsToDelete)
        DeleteExecute(item);

    DatabaseObject.Close();

    ItemsToUpdate.clear();
    ItemsToInsert.clear();
    ItemsToDelete.clear();
}

template<class T>
inline void CAbstractEFProvider<T>::UpdateModel(CAbstractDatabaseModel* model)
{
    this->UpdateExecute(static_cast<T*>(model));
    this->SaveChanges();
}

template<class T>
inline void CAbstractEFProvider<T>::CreateModel(CAbstractDatabaseModel* model)
{
    this->Append(static_cast<T*>(model));
    this->SaveChanges();
}

template<class T>
inline CAbstractDatabaseModel* CAbstractEFProvider<T>::FindModel(std::string foreign_key)
{
    return static_cast<CAbstractDatabaseModel*>(Find(foreign_key));
}

template<class T>
inline CAbstractDatabaseModel* CAbstractEFProvider<T>::GetById(int id)
{
	return new CAbstractDatabaseModel();
}

template<class T>
void CAbstractEFProvider<T>::UpdateRelationships(T& item)
{
}

template<class T>
void CAbstractEFProvider<T>::UpdateWithRelationships(T& item)
{
    // set relationship
    for each(SDatabaseRelationship& rel in Relationships)
    {
        auto item_obj = static_cast<CAbstractDatabaseModel*>(item);
        QString          foreignKey = item_obj->property(rel.ForeignKey.c_str()).toString();
        auto             temp = rel.EFProvider->FindModel(foreignKey);
        RelationshipItem relItem;

        relItem.RelModel = temp;
        relItem.ForeignKey = rel.ForeignKey;
        item_obj->addRelationship(relItem);
    }
}
