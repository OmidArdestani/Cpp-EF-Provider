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

#define GET_CLASS_NAME() ( _Function_class_)
#define REGISTER_PROPERTY(type, name, model_name, setter, getter) \
    public: type name; \
    private: struct name ## PropertyRegistrar { \
			name ## PropertyRegistrar(model_name* p){\
				p->RegisterProperty(#name, \
						new std::function<void(CVariant)>(std::bind(setter, p, std::placeholders::_1)),\
						new std::function<CVariant()>(std::bind(getter, p))); \
			}\
    }; std::unique_ptr<name ## PropertyRegistrar> name ## Registrar = std::make_unique<name ## PropertyRegistrar>(this);


class CAbstractDatabaseModel
{
public:
	std::map<std::string, CVariant> GetAllPropertie()
	{
		std::map<std::string, CVariant> all_prop;
		for (auto item = PropertyMap.cbegin(); item != PropertyMap.cend(); ++item)
		{
            auto prop_value = (*item->second->Getter)();
            all_prop.insert_or_assign(item->first, prop_value);
		}
		return all_prop;
	}

	int PropertyCount()
	{
		return 0;
	}

	CVariant GetProperty(std::string key)
	{
		auto prop = this->PropertyMap.find(key);
		if (prop != this->PropertyMap.end())
			return (*prop->second->Getter)();
		else
			return CVariant();
	}

	void AddRelationship(RelationshipItem foreignKey)
	{
		RelationshipList.push_back(foreignKey);
	}

	template<class T>
	T* GetRelationship(std::string foreignKey)
	{
		for (auto rel = RelationshipList.cbegin(); rel != RelationshipList.cend(); ++rel)
		{
			if (rel->ForeignKey == foreignKey)
			{
				return static_cast<T*>(rel->RelModel);
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
			(*find_prop->second->Setter)(var);
		}
		else
		{

			//TODO need to complete 
			std::function<void(CVariant)>* f_setter = new std::function<void(CVariant)>([](CVariant value) {});
			std::function<CVariant()>* f_getter = new std::function<CVariant()>([]() {return CVariant(); });
			RegisterProperty(property_name, f_setter, f_getter);
		}
	}

	void RegisterProperty(std::string property_name, std::function<void(CVariant)>* setter, std::function<CVariant()>* getter)
	{
        PropertyMap.insert_or_assign(property_name, std::make_unique<PropContainer>(setter, getter));
	}

private:
	struct PropContainer
	{
		PropContainer() {}
        ~PropContainer() {delete Setter; delete Getter;}
		PropContainer(std::function<void(CVariant)>* setter, std::function<CVariant()>* getter)
		{
			this->Setter = setter;
			this->Getter = getter;
		}
		std::function<void(CVariant)>* Setter = nullptr;
		std::function<CVariant()>* Getter = nullptr;
	};
	std::map<std::string, std::unique_ptr<PropContainer>> PropertyMap;
	std::list<RelationshipItem> RelationshipList;

    REGISTER_PROPERTY(int, id, CAbstractDatabaseModel, &CAbstractDatabaseModel::SetId, &CAbstractDatabaseModel::GetId)
public:
    virtual CVariant GetId() { return this->id; }
    virtual void SetId(CVariant id) { this->id = id.toType<int>(); }
};
