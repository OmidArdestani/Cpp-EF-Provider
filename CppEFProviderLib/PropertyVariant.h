#include <iostream>
#include <string>
#include <type_traits>
#include <stdexcept>

class CVariant {

public:
	enum class Type {
		INT,
		DOUBLE,
		STRING
	};

private:
	Type type_;
	union Value {
		int intValue;
		double doubleValue;
		std::string* stringValue;

		Value() : stringValue(nullptr) {}
		~Value()
		{
			if (stringValue) {
				//				delete stringValue;
			}
		}
	} value;

public:
	CVariant() : type_(Type::INT) {
		value.intValue = 0;
	}

	// Constructor template for generic types
	template <typename T, typename = std::enable_if_t<std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, std::string>>>
	CVariant(T value) : type_(getType<T>()) {
		assignValue(value);
	}

	// Copy constructor for generic types
	template <typename T>
	CVariant(const CVariant& other, std::enable_if_t<std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, std::string>>* = nullptr) : type_(other.type_) {
		copyValue<T>(other);
	}

	// Assignment operator for generic types
	template <typename T>
	CVariant& operator=(T value) {
		clearValue();
		type_ = getType<T>();
		assignValue(value);
		return *this;
	}

	~CVariant() {
		clearValue();
	}

	Type getType() const {
		return type_;
	}

	// Type-safe conversion functions
	template <typename T>
	T toType() const {
		if constexpr (std::is_same_v<T, int>) {
			return getIntValue();
		}
		else if constexpr (std::is_same_v<T, double>) {
			return getDoubleValue();
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			return getStringValue();
		}
		else {
			// Handle unsupported type
			throw std::runtime_error("Unsupported type");
		}
	}

	int toInt() const
	{
		return this->toType<int>();
	}

	double toDouble() const
	{
		return this->toType<double>();
	}

	std::string toString() const
	{
		return this->toType<std::string>();
	}

	template <typename T>
	T operator = (CVariant value)
	{
		switch (value.getType())
		{
		case Type::DOUBLE:return value.getDoubleValue();
		case Type::INT:return value.getIntValue();
		case Type::STRING:return value.getStringValue();
		default:return T();
		}
	}


private:
	template <typename T>
	std::string ToString()const
	{
		switch (this->getType())
		{
		case Type::DOUBLE:return std::to_string(this->getDoubleValue());
		case Type::INT:return std::to_string(this->getIntValue());
		case Type::STRING:return this->getStringValue();
		default:return T();
		}
	}

	template <typename T>
	static Type getType() 
	{
		if constexpr (std::is_same_v<T, int>) 
			return Type::INT;
		else if constexpr (std::is_same_v<T, double>)
			return Type::DOUBLE;
		else if constexpr (std::is_same_v<T, std::string>)
			return Type::STRING;
		else
			throw std::runtime_error("Unsupported type");// Handle unsupported type
	}

	void clearValue() 
	{
		if (type_ == Type::STRING) 
		{
			//			delete value.stringValue;
		}
	}

	template <typename T>
	void assignValue(T newValue) 
	{
		if constexpr (std::is_same_v<T, int>)
			value.intValue = newValue;
		else if constexpr (std::is_same_v<T, double>)
			value.doubleValue = newValue;
		else if constexpr (std::is_same_v<T, std::string>)
			value.stringValue = new std::string(newValue);
		else
			throw std::runtime_error("Unsupported type");// Handle unsupported type
	}

	template <typename T>
	void copyValue(const CVariant& other)
	{
		if constexpr (std::is_same_v<T, int>)
			value.intValue = other.value.intValue;
		else if constexpr (std::is_same_v<T, double>)
			value.doubleValue = other.value.doubleValue;
		else if constexpr (std::is_same_v<T, std::string>)
			value.stringValue = new std::string(*other.value.stringValue);
		else
			throw std::runtime_error("Unsupported type");// Handle unsupported type
	}

	int getIntValue() const
	{
		switch (type_)
		{
		case Type::INT:
			return value.intValue;
		case Type::DOUBLE:
			return (int)value.doubleValue;
		case Type::STRING:
			return std::stoi(*value.stringValue);
		}
	}

	double getDoubleValue() const
	{
		switch (type_)
		{
		case Type::INT:
			return (double)value.intValue;
		case Type::DOUBLE:
			return value.doubleValue;
		case Type::STRING:
			return (double)std::stof(*value.stringValue);
		}
	}

	std::string getStringValue() const
	{
		switch (type_)
		{
		case Type::INT:
			return std::to_string(value.intValue);
		case Type::DOUBLE:
			return std::to_string(value.doubleValue);
		case Type::STRING:
			return *value.stringValue;
		}
	}
};

