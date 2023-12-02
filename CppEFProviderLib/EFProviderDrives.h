#pragma once
#include "AbstractEFProvider.h"
#include <mutex>
#include <string>
#include <map>
#include <list>

namespace EFprovider
{
	template <class T>
    class CSQLightEFprovider : public CAbstractEFProvider<T>
	{
	public:
		CSQLightEFprovider(std::string database_file_name) : CAbstractEFProvider<T>()
		{
		}

	public:
		T* SingleOrDefault(std::string condition) override
		{
			std::list<T*> temp = Where(condition);

			if (temp.size() > 0)
                return temp.front();
			else
				return new T();
		}
		T* LastItem() override
		{
			int topIndex = GetMaxID();
			T* temp_model = Find(topIndex);

            this->UpdateRelationships(temp_model);
			return temp_model;
		}
		T* Find(int id) override
		{
			T* temp = SingleOrDefault("id='" + std::to_string(id) + "'");

			return temp;
		}
		std::list<std::list<std::string>> Select(std::string property_name, std::string condition) override
		{
            std::list<std::list<std::string>> result_matrix(1);

			std::string command;
			std::string whereCommand = "";

			if (condition != "")
				whereCommand = " where " + condition;

			command = "select " + property_name + " from " + this->TableName + whereCommand + ";";
			// Complete query.
			LocalMutex.lock();

			this->DatabaseObject->Open();
			auto q = this->DatabaseObject->Execute(command);
			this->DatabaseObject->Close();

            while (q->Next())
            {
//                std::list<std::string> r =  { q->Value(0).ToString() };
//				result_matrix.push_back(r);
			}

			LocalMutex.unlock();

			return result_matrix;
		}
		std::list<T*> Where(std::string condition) override
		{
			LocalMutex.lock();
			auto command = "select * from " + this->TableName + " where " + condition + ";";
			//while (!this->DatabaseObject->Open()) { QThread::msleep(500); }

			this->DatabaseObject->Open();
			auto q = this->DatabaseObject->Execute(command);
			this->DatabaseObject->Close();

			std::list<T*> model_temp_list;

            while (q->Next())
			{
				auto model_temp = static_cast<CAbstractDatabaseModel*>(new T);
				auto all_property = model_temp->GetAllPropertie();

				for (auto item = all_property.cbegin(); item != all_property.cend(); ++item)
				{
					model_temp->SetProperty(item->first, item->second);
				}
				model_temp_list.push_back(static_cast<T*>(model_temp));
			}

			for (auto item = model_temp_list.cbegin(); item != model_temp_list.cend(); ++item)
            {
                this->UpdateRelationships(*item);
			}
			LocalMutex.unlock();
            return model_temp_list;
		}

		std::list<T*> ToList() override
		{
            return std::list<T*>();
		}
		std::list<T*> Top(int number) override
		{
			std::list<T*> temp_list;
			int        topIndex = GetMaxID();
			int        lowIndex = topIndex - number;

			for (int i = topIndex; i >= lowIndex; i--)
			{
				auto temp = Find(i);

                this->UpdateRelationships(temp);
				temp_list.push_back(temp);
			}
			return temp_list;
		}
		std::list<T*> GetPagedData(int pageIndex, int numberOfItemPerPage, std::string orderByProperty) override
		{
            return std::list<T*>();
		}
		int Count() override
		{
			return 0;
		}
		int GetMaxID() override
		{
			return 0;
		}

	private:
		std::mutex LocalMutex;

	protected:
		void UpdateExecute(T* item) override {

		}
		void CreateExecute(T* item) override {

		}
		void DeleteExecute(T* item) override
		{

		}
		bool CheckDatabaseTable() override
		{
			return false;
		}
	};
}
