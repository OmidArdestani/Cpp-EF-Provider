#pragma once
#include "../AbstractEFProvider.hpp"
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
	class CAbstractCommonDatabaseCommand : public CAbstractEFProvider<T>
	{
	public:
		CAbstractCommonDatabaseCommand() : CAbstractEFProvider<T>()
		{
		}

	public:
		T* SingleOrDefault(std::string condition) override
		{
			std::list<T*> temp = this->Where(condition);

			if (temp.size() > 0)
				return temp.front();
			else
				return new T();
		}

		T* LastItem() override
		{
			int topIndex = this->GetMaxID();
			T* temp_model = Find(topIndex);

			this->UpdateWithRelationships(temp_model);
			return temp_model;
		}

		T* Find(int id) override
		{
			T* temp = SingleOrDefault("id='" + std::to_string(id) + "'");

			return temp;
		}

		std::list<T*> Top(int number) override
		{
			std::list<T*> temp_list;
			int        topIndex = this->GetMaxID();
			int        lowIndex = topIndex - number;

			for (int i = topIndex; i > lowIndex; i--)
			{
				auto temp = Find(i);

				this->UpdateWithRelationships(temp);
				temp_list.push_back(temp);
			}
			return temp_list;
		}

	protected:
		std::mutex LocalMutex;
	};
}
