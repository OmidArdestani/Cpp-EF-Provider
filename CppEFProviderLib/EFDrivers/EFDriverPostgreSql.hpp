
#include "../AbstractEFProvider.hpp"
#include <mutex>
#include <string>
#include <map>
#include <list>
#include <format>
#include <assert.h>
#include <format>

namespace EFProvider
{
	template <class T>
	class CEFDriverPostgreSql : public CAbstractEFProvider<T>
	{
	public:
		CEFDriverPostgreSql() :CAbstractEFProvider<T>()
		{

		}

	public:
		virtual T* SingleOrDefault(std::string condition) { return new T(); }
		virtual T* LastItem() { return new T(); }
		virtual T* Find(int id) { return new T(); }
		virtual std::list<std::list<std::string>> Select(std::string property_name, std::string condition) { return std::list<std::list<std::string>>(); }
		virtual std::list<T*> Where(std::string condition) { return std::list<T*>(); }
		virtual std::list<T*> ToList() { return std::list<T*>(); }
		virtual std::list<T*> Top(int number) { return std::list<T*>(); }
		virtual std::list<T*> GetPagedData(int pageIndex, int number_of_item_per_page, std::string orderByProperty) { return std::list<T*>(); }
		virtual int Count() { return -1; }
		virtual int GetMaxID() { return -1; }
	};
}