#pragma once
#include "AbstractCommonDatabaseCommand.hpp"
#include <mutex>
#include <string>
#include <map>
#include <list>
#include <format>
#include <assert.h>
#include <format>
#include <string>

namespace EFProvider
{
	template <class T>
	class CEFDriverSQLight : public CAbstractCommonDatabaseCommand<T>
	{
	public:
		CEFDriverSQLight() : CAbstractCommonDatabaseCommand<T>()
		{
		}

	public:
		std::list<std::list<std::string>> Select(std::string property_name, std::string condition) override
		{
			std::list<std::list<std::string>> result_matrix(1);

			std::string command;
			std::string whereCommand = "";

			if (condition != "")
				whereCommand = " where " + condition;

			command = "select " + property_name + " from " + this->TableName + whereCommand + ";";
			// Complete query.
            this->LocalMutex.lock();

			this->DatabaseObject->Open();
			auto q = this->DatabaseObject->Execute(command);
			this->DatabaseObject->Close();

			while (q->Next())
			{
                std::list<std::string> r = { q->Value(0).toString() };
				result_matrix.push_back(r);
			}

            this->LocalMutex.unlock();

			return result_matrix;
		}

		std::list<T*> Where(std::string condition) override
		{
            this->LocalMutex.lock();
			auto command = "select * from " + this->TableName + " where " + condition + ";";

			this->DatabaseObject->Open();
            auto q = this->DatabaseObject->Execute(command);

			std::list<T*> model_temp_list;

			while (q->Next())
			{
				auto model_temp = static_cast<CAbstractDatabaseModel*>(new T);
				auto all_property = model_temp->GetAllPropertie();

				for (auto itr_item = all_property.cbegin(); itr_item != all_property.cend(); ++itr_item)
				{
					model_temp->SetProperty(itr_item->first, q->Value(itr_item->first));
				}
				model_temp_list.push_back(static_cast<T*>(model_temp));
            }
            this->DatabaseObject->Close();

			for (auto item = model_temp_list.cbegin(); item != model_temp_list.cend(); ++item)
			{
                this->UpdateWithRelationships(*item);
			}
            this->LocalMutex.unlock();
			return model_temp_list;
		}

		std::list<T*> ToList() override
		{
			// save all changes before make list
			std::list<T*> temp_list;
			std::string command = "select * from " + this->TableName + ";";

			// Complete query.
			this->DatabaseObject->Open();
			auto q = this->DatabaseObject->Execute(command);

			while (q->Next())
			{
                auto model_temp = static_cast<CAbstractDatabaseModel*>(new T());
				auto all_property = model_temp->GetAllPropertie();

				for (auto itr_item = all_property.cbegin(); itr_item != all_property.cend(); ++itr_item)
				{
                    model_temp->SetProperty(itr_item->first, q->Value(itr_item->first));
				}
                temp_list.push_back(static_cast<T*>(model_temp));
			}
			this->DatabaseObject->Close();
			
			for(auto item = temp_list.cbegin(); item != temp_list.cend(); ++item)
			{
                this->UpdateWithRelationships(*item);
			}
			return temp_list;
		}

		std::list<T*> GetPagedData(int pageIndex, int number_of_item_per_page, std::string orderByProperty) override
		{
			return std::list<T*>();
		}

		int Count() override
		{
			std::string command;
			int count = 0;

			command = "select count(id) from " + this->TableName + ";";

			this->DatabaseObject->Open();
			auto q = this->DatabaseObject->Execute(command);

			if (q->Next())
			{
				count += q->Value(0).toInt();
			}
			this->DatabaseObject->Close();
			return count;
		}

		int GetMaxID() override
		{
			std::string command = "select MAX(id) from " + this->TableName + ";";

			this->DatabaseObject->Open();
			auto q = this->DatabaseObject->Execute(command);

			int       topIndex = 0;

			while (q->Next())
			{
				topIndex = q->Value(0).toInt();
			}
			this->DatabaseObject->Close();

			return topIndex;
		}

	protected:
		std::string GetPropertyDbType(CVariant::Type type)
		{
			switch (type)
			{
			case CVariant::Type::INT: return "integer";
			case CVariant::Type::DOUBLE: return "double precision";
			case CVariant::Type::STRING: return "char(50)";
			default:
				assert(false);
				return "";
			}
		}

		void UpdateExecute(T* item) override
		{
			auto model = static_cast<CAbstractDatabaseModel*>(item);
			assert(model);

			int     id = model->GetProperty("id").toInt();

			std::list<std::string> property_value_list;

            auto all_property = model->GetAllPropertie();

			for (auto itr_item = all_property.cbegin(); itr_item != all_property.cend(); ++itr_item)
			{
				if (this->ToLower(itr_item->first) != "id")
				{
					std::string prop_name = itr_item->first;
					std::string prop_value = itr_item->second.toString();
					property_value_list.push_back(std::format("{}='{}'", prop_name, prop_value));
				}
			}

			std::string command = std::format("update {} set {} where id={};",
				this->TableName,
				this->JoinString(property_value_list, ","),
				std::to_string(id));

			this->DatabaseObject->Open();
			this->DatabaseObject->Execute(command);
			this->DatabaseObject->Close();
		}

		void CreateExecute(T* item) override
		{
			std::list<std::string> property_value_list;
			std::list<std::string> table_column_list;

			auto model = static_cast<CAbstractDatabaseModel*>(item);
			assert(model);

            auto all_property = model->GetAllPropertie();

			for (auto itr_item = all_property.cbegin(); itr_item != all_property.cend(); ++itr_item)
			{
				// check if the object is not a relationship object.
				auto contains_star = itr_item->first.find('*');
				// check if propertie is not id. Becouse the id always is a auto incremental..
				if ((contains_star == std::string::npos) && (this->ToLower(itr_item->first) != "id"))
				{
					table_column_list.push_back(itr_item->first);
					property_value_list.push_back("'" + itr_item->second.toString() + "'");
				}
			}

			const std::string command = "insert into " + this->GetTableName() + " (" + this->JoinString(table_column_list, ",") + ") values (" + this->JoinString(property_value_list, ", ") + ");";
			const std::string getIdCommand = "select MAX(id) from " + this->GetTableName() + ";";

			this->DatabaseObject->Open();
			this->DatabaseObject->Execute(command);

			auto q = this->DatabaseObject->Execute(getIdCommand);

			while (q->Next())
			{
				int id = q->Value(0).toInt();
				model->SetProperty("id", id);
			}

			this->DatabaseObject->Close();
		}

		void DeleteExecute(T* item) override
		{
			auto model = static_cast<CAbstractDatabaseModel*>(item);
			assert(model);

			int     id = model->GetProperty("id").toInt();

			std::list<std::string> property_list;

            auto all_property = model->GetAllPropertie();

			for (auto itr_item = all_property.cbegin(); itr_item != all_property.cend(); ++itr_item)
			{
				if (this->ToLower(itr_item->first) != "id")
				{
					auto prop_name = itr_item->first;
					auto prop_value = model->GetProperty(itr_item->first).toString();

					std::string format_fill = std::format("{} = '{}'", prop_name, prop_value);

					//property_list.push_back(propName + " = '" + propValue + "'");
					property_list.push_back(format_fill);
				}
			}

			std::string command = std::format("update {} set {} where id= {};",
				this->TableName,
				this->JoinString(property_list, ","),
				std::to_string(id));

			this->DatabaseObject->Open();
			this->DatabaseObject->Execute(command);
			this->DatabaseObject->Close();
		}

		bool CheckDatabaseTable() override
		{
			// create table
			std::list<std::string> table_columns;
			CAbstractDatabaseModel* new_model = static_cast<CAbstractDatabaseModel*>(new T);

            auto all_property = new_model->GetAllPropertie();

			for (auto itr_item = all_property.cbegin(); itr_item != all_property.cend(); ++itr_item)
			{
				if (this->ToLower(itr_item->first) == "id")
				{
                    table_columns.push_back(itr_item->first + " INTEGER PRIMARY KEY AUTOINCREMENT");
				}
				else
				{
					table_columns.push_back(itr_item->first + " " + GetPropertyDbType(itr_item->second.getType()));
				}
			}

			std::string command = std::format("create table {} ({});", this->TableName, this->JoinString(table_columns, ", "));

			this->DatabaseObject->Open();
			this->DatabaseObject->Execute(command);
			this->DatabaseObject->Close();

			return true;
		}
	};
}
