#pragma once
#include "AbstractEFProvider.h"
#include <mutex>

namespace EFprovider
{
	template <class T>
	class CSQLightEFprovider : public CAbstractEFProvider<T>
	{
	public:
		T SingleOrDefault(std::string condition) override;
		T LastItem() override;
		T Find(std::string condition) override;
		std::list<std::list<std::string>> Select(std::string condition) override;
		std::list<T> Where(std::string condition) override;
		std::list<T> ToList() override;
		std::list<T> GetPagedData(int pageIndex, int numberOfItemPerPage, std::string orderByProperty) override;
		int Count() override;
		int GetMaxID() override;

	private:
		std::mutex LocalMutex;
	};
}