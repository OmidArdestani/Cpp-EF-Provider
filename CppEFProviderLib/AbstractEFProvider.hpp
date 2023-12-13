#pragma once
#include <list>
#include <string>
#include "DatabaseModel.h"

namespace EFProvider
{
	class CBaseEFProvider;

	enum class EDatabaseType
	{
		MySQL, SQLight1, SQLight2, MSSQL, Postgres
	};

	//*************************************
	//*************************************
	struct SDatabaseRelationship
	{
		CBaseEFProvider* EFProvider = nullptr;
		std::string ForeignKey;
	};

	//*************************************
	//*************************************
	class CAbstracSqlDataBaseQuery
	{
	public:
		virtual bool Next() = 0;
		virtual CVariant Value(int index) = 0;
		virtual CVariant Value(std::string key) = 0;
	};

	class CAbstractSQLDatabase
	{
	public:
		CAbstractSQLDatabase() {}

	public:
		virtual bool Open() = 0;
		virtual bool Close() = 0;
		virtual CAbstracSqlDataBaseQuery* Execute(std::string cmd) = 0;
	};

	//*************************************
	//*************************************
	class CBaseEFProvider
	{
	public:
		void AddRelationship(SDatabaseRelationship relationship) { Relationships.push_back(relationship); }
		std::string JoinString(std::list<std::string> string_list, std::string sep)
		{
			std::string result = "";

			for (auto item = string_list.cbegin(); item != string_list.cend(); ++item)
                result += *item + sep;

            result.erase(result.size() - sep.size(), sep.size());
            return result;
		}
		std::string GetTableName() { return TableName; }
		std::string ToLower(std::string str) { for (auto& x : str)x = std::tolower(x); return str; }

	protected:
		std::list<SDatabaseRelationship> Relationships;
		CAbstractSQLDatabase* DatabaseObject = nullptr;
		std::string TableName;
		EDatabaseType DBType;

	protected:
		virtual void UpdateModel(CAbstractDatabaseModel* model) = 0;
		virtual void CreateModel(CAbstractDatabaseModel* model) = 0;
		virtual CAbstractDatabaseModel* FindModel(int foreign_key) = 0;
		virtual CAbstractDatabaseModel* GetById(int id) = 0;
	};

	//*************************************
	//*************************************
	template <class T>
	class CAbstractEFProvider : public CBaseEFProvider
	{
	public:
		CAbstractEFProvider() :CBaseEFProvider()
		{

		}
		void Initialize(CAbstractSQLDatabase* database_object, EDatabaseType type)
		{
			DBType = type;
			DatabaseObject = database_object;

			TableName = GetType() + "s";
			CheckDatabaseTable();
		}

	public:
		virtual T* SingleOrDefault(std::string condition) = 0;
		virtual T* LastItem() = 0;
		virtual T* Find(int id) = 0;
		virtual std::list<std::list<std::string>> Select(std::string property_name, std::string condition) = 0;
		virtual std::list<T*> Where(std::string condition) = 0;
		virtual std::list<T*> ToList() = 0;
		virtual std::list<T*> Top(int number) = 0;
		virtual std::list<T*> GetPagedData(int pageIndex, int number_of_item_per_page, std::string orderByProperty) = 0;
		virtual int Count() = 0;
		virtual int GetMaxID() = 0;

	public:
		void Append(T* item)
		{
			ItemsToInsert.push_back(item);
		}
		void Remove(T* item)
		{
			ItemsToDelete.push_back(item);
		}
		void Update(T* item)
		{
			ItemsToUpdate.push_back(item);
		}
		void SaveChanges()
		{
			// check for delete
			for (auto item = ItemsToUpdate.cbegin(); item != ItemsToUpdate.cend(); ++item)
			{
				UpdateExecute(*item);
				UpdateRelationships(*item);
			}

			// check for insert
			for (auto item = ItemsToInsert.cbegin(); item != ItemsToInsert.cend(); ++item)
				CreateExecute(*item);

			for (auto item = ItemsToDelete.cbegin(); item != ItemsToDelete.cend(); ++item)
				DeleteExecute(*item);

			DatabaseObject->Close();

			ItemsToUpdate.clear();
			ItemsToInsert.clear();
			ItemsToDelete.clear();
		}
		std::string GetType() 
		{
            const std::string class_keyword = "class";
            std::string class_typename = typeid(T).name();

            // Find the position of "class" in the string
            size_t pos = class_typename.find(class_keyword);

            // Check if "class" is found
            if (pos != std::string::npos)
            {
                // Erase "class" and any following spaces
                class_typename.erase(pos, class_keyword.size());
            }

			return class_typename;
		}

	protected:
		void UpdateModel(CAbstractDatabaseModel* model) override
		{
			this->UpdateExecute(static_cast<T*>(model));
			this->SaveChanges();
		}
		void CreateModel(CAbstractDatabaseModel* model) override
		{
			this->Append(static_cast<T*>(model));
			this->SaveChanges();
		}
		CAbstractDatabaseModel* FindModel(int foreign_key) override
		{
			return static_cast<CAbstractDatabaseModel*>(Find(foreign_key));
		}
		CAbstractDatabaseModel* GetById(int id) override
		{
			return new CAbstractDatabaseModel();
		}
		void UpdateRelationships(T* item)
		{
		}
		void UpdateWithRelationships(T* item)
		{
			// set relationship
			for (auto rel = Relationships.cbegin(); rel != Relationships.cend(); ++rel)
			{
				auto item_obj = static_cast<CAbstractDatabaseModel*>(item);

				std::string foreignKey = item_obj->GetProperty(rel->ForeignKey);
				auto        temp = rel->EFProvider->FindModel(std::stoi(foreignKey));
				RelationshipItem rel_item;

				rel_item.RelModel = temp;
				rel_item.ForeignKey = rel->ForeignKey;
				item_obj->AddRelationship(rel_item);
			}
		}

	protected:
		virtual void UpdateExecute(T* item) = 0;
		virtual void CreateExecute(T* item) = 0;
		virtual void DeleteExecute(T* item) = 0;
		virtual bool CheckDatabaseTable() = 0;

	protected:
		/// The history of data sets for update in the database
		std::list<T*> ItemsToUpdate;
		/// The history of data sets for delete of the database
		std::list<T*> ItemsToDelete;
		/// The history of data sets for insert in the database
		std::list<T*> ItemsToInsert;
	};
}
