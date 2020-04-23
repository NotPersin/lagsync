#pragma once

#include "../menu/c_menu.h"
#include <vector>

template< typename T >
class ConfigItem
{
	std::string name;
	T* value;
public:
	ConfigItem(std::string name, T* value)
	{
		this->name = name;
		this->value = value;
	}
};

template< typename T >
class ConfigValue
{
public:
	ConfigValue(std::string name_, T* value_)
	{
		name = name_;
		value = value_;
	}

	std::string name;
	T* value;
};

class C_Config
{
protected:
	std::vector< ConfigValue< int >* > ints;
	std::vector< ConfigValue< bool >* > bools;
	std::vector< ConfigValue< float >* > floats;
	std::vector< ConfigValue< c_vector3d >* > vector;

private:
	void SetupValue(int&, int, std::string);
	void SetupValue(bool&, bool, std::string);
	void SetupValue(float&, float, std::string);
	void SetupValue(c_vector3d&, c_vector3d, std::string);
	//void SetupValue(ColorV2&, float, std::string);
	void SetupValue(bool*, bool, int, std::string);
public:
	C_Config()
	{
		Setup();
	}

	void Setup();

	void Save(int idx);
	void Load(int idx);
};

extern C_Config* g_Config;
