#pragma once
#include <list>
#include <string>
#include "DatabaseModel.h"

namespace EFprovider
{
	struct CBaseEFProvider;

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
		virtual std::string Value(int index) = 0;
	};

	class CAbstractSQLDatabase
	{
	public:
		virtual bool Open() = 0;
		virtual bool Close() = 0;
		virtual CAbstracSqlDataBaseQuery Execute(std::string cmd) = 0;
	};

	//*************************************
	//*************************************
	class CBaseEFProvider
	{
	public:
		void AddRelationship(SDatabaseRelationship relationship) { Relationships.push_back(relationship); }

	protected:
		std::list<SDatabaseRelationship> Relationships;
		CAbstractSQLDatabase* DatabaseObject;

	protected:
		virtual void UpdateModel(CAbstractDatabaseModel* model) = 0;
		virtual void CreateModel(CAbstractDatabaseModel* model) = 0;
		virtual CAbstractDatabaseModel FindModel(std::string foreign_key) = 0;
		virtual CAbstractDatabaseModel GetById(int id) = 0;
	};

	//*************************************
	//*************************************
	template <class T>
	class CAbstractEFProvider : public CBaseEFProvider
	{
	public:
		void Initialize(CAbstractSQLDatabase database_object, EDatabaseType type);

	public:
		virtual T SingleOrDefault(std::string condition) = 0;
		virtual T LastItem() = 0;
		virtual T Find(std::string condition) = 0;
		virtual std::list<std::list<std::string>> Select(std::string condition) = 0;
		virtual std::list<T> Where(std::string condition) = 0;
		virtual std::list<T> ToList() = 0;
		virtual std::list<T> GetPagedData(int pageIndex, int numberOfItemPerPage, std::string orderByProperty) = 0;
		virtual int Count() = 0;
		virtual int GetMaxID() = 0;

	public:
		void Append(T& item);
		void Remove(T& item);
		void Update(T& item);
		void SaveChanges();

	private:
		void UpdateModel(CAbstractDatabaseModel* model) override;
		void CreateModel(CAbstractDatabaseModel* model) override;
		CAbstractDatabaseModel* FindModel(std::string foreign_key) override;
		CAbstractDatabaseModel* GetById(int id) override;
		void UpdateRelationships(T& item);
		void UpdateWithRelationships(T& item);

	protected:
		virtual void UpdateExecute(T& item) = 0;
		virtual void CreateExecute(T& item) = 0;
		virtual void DeleteExecute(T& item) = 0;
		virtual void CheckDatabaseTable() = 0;

	protected:
		/// The history of data sets for update in the database
		std::list<T*> ItemsToUpdate;
		/// The history of data sets for delete of the database
		std::list<T*> ItemsToDelete;
		/// The history of data sets for insert in the database
		std::list<T*> ItemsToInsert;
		std::string TableName;
	};
}
