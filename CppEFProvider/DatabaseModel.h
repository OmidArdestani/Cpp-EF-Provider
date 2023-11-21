#pragma once

#include <map>
#include <stdexcept>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include "PropertyVariant.h"

class CAbstractDatabaseModel;



struct RelationshipItem
{
	std::string ForeignKey = "";
	CAbstractDatabaseModel* RelModel = nullptr;
};


#define REGISTER_PROPERTY(type, name, setter, getter) \
    public: type name; \
    private: struct name ## PropertyRegistrar { \
			name ## PropertyRegistrar(CAbstractDatabaseModel* p){\
				p->RegisterProperty(#name, \
						new std::function<void(CVariant)>(std::bind(setter, p, std::placeholders::_1)),\
						new std::function<CVariant()>(std::bind(getter, p))); \
			}\
    } name ## Registrar;


class CAbstractDatabaseModel
{
	REGISTER_PROPERTY(std::string, Omid, &CAbstractDatabaseModel::SetOmid, &CAbstractDatabaseModel::GetOmid)

public:
	virtual int GetId() = 0;


	void SetOmid(CVariant value)
	{
		Omid = value.toType<std::string>();
	}

	CVariant GetOmid()
	{
		return Omid;
	}

private:

	//std::map<std::string, CVariant> GetAllPropertie()
	//{
	//	return this->PropertyMap;
	//}

	CVariant GetPropertie(std::string key)
	{
		auto prop = this->PropertyMap.find(key);
		if (prop != this->PropertyMap.end())
			return (*prop->second.Getter)();
		else
			return CVariant();
	}

	std::list<RelationshipItem> RelationshipList;
	void AddRelationship(RelationshipItem foreignKey)
	{
		RelationshipList.push_back(foreignKey);
	}

	template<class T>
	T* GetRelationship(std::string foreignKey)
	{
		for each (RelationshipItem rel in RelationshipList)
		{
			if (rel.ForeignKey == foreignKey)
			{
				return static_cast<T*>(rel.RelModel);
			}
		}
		// if not found any rel, create alternative relationship
		RelationshipItem new_rel;
		new_rel.RelModel = new T();
		new_rel.ForeignKey = foreignKey;
		RelationshipList.push_back(new_rel);

		return static_cast<T*>(new_rel.RelModel);
	}

	void SetProperty(std::string property_name, CVariant var)
	{
		auto find_prop = PropertyMap.find(property_name);
		if (find_prop != PropertyMap.end())
		{
			(*find_prop->second.Setter)(var);
		}
		else
		{
			std::function<void(CVariant)>* f_setter = new std::function<void(CVariant)>([](CVariant value) {});
			std::function<CVariant()>* f_getter = new std::function<CVariant()>([]() {return CVariant(); });
			RegisterProperty(property_name, f_setter, f_getter);
		}
	}

	void RegisterProperty(std::string property_name, std::function<void(CVariant)>* setter, std::function<CVariant()>* getter)
	{
		PropContainer pc(setter, getter);
		PropertyMap.insert_or_assign(property_name, pc);
	}

private:
	struct PropContainer
	{
		PropContainer() { }
		PropContainer(std::function<void(CVariant)>* setter, std::function<CVariant()>* getter)
		{
			this->Setter = setter;
			this->Getter = getter;
		}
		std::function<void(CVariant)>* Setter;
		std::function<CVariant()>* Getter;
	};
	std::map<std::string, PropContainer> PropertyMap;
};
