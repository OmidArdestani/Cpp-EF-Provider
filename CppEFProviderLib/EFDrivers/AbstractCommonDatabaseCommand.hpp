#pragma once
/*
MIT License

Copyright (c) 2023 Omid Ardestani

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
