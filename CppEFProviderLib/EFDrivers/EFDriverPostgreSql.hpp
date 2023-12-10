
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
	};
}