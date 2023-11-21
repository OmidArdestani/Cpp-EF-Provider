#include "EFProviderDrives.h"

template<class T>
inline T CSQLightEFprovider<T>::SingleOrDefault(std::string condition)
{
	std::list<T*> temp = Where(condition);

	if (temp.size() > 0)
		return temp[0];
	else
		return new T();
}

template<class T>
T CSQLightEFprovider<T>::LastItem()
{
	return T();
}

template<class T>
T CSQLightEFprovider<T>::Find(std::string condition)
{
	return T();
}

template<class T>
std::list<std::list<std::string>> CSQLightEFprovider<T>::Select(std::string condition)
{
	return std::list<std::list<std::string>>();
}

template<class T>
std::list<T> CSQLightEFprovider<T>::Where(std::string condition)
{
	LocalMutex.lock();
	auto command = "select * from " + TableName + " where " + condition + ";";
	//while (!this->DatabaseObject->Open()) { QThread::msleep(500); }
	auto q = DatabaseObject->Execute(command);
	
	std::list<T*> model_temp_list;

	while (q.next())
	{
		auto model_temp = static_cast<CAbstractDatabaseModel*>(new T);
		for (int i = 1; i < temp->metaObject()->propertyCount(); i++)
		{
			model_temp->setProperty(temp->metaObject()->property(i).name(), q.value(temp->metaObject()->property(i).name()));
		}
		model_temp_list.push_back(static_cast<T*>(model_temp));
	}

	this->DatabaseObject->Close();
	for each(T * item in model_temp_list)
	{
		UpdtateWithRelationships(item);
	}
}

template<class T>
std::list<T> CSQLightEFprovider<T>::ToList()
{
	return std::list<T>();
}

template<class T>
std::list<T> CSQLightEFprovider<T>::GetPagedData(int pageIndex, int numberOfItemPerPage, std::string orderByProperty)
{
	return std::list<T>();
}

template<class T>
int CSQLightEFprovider<T>::Count()
{
	return 0;
}

template<class T>
int CSQLightEFprovider<T>::GetMaxID()
{
	return 0;
}
