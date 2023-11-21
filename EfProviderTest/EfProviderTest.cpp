// EfProviderTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\CppEFProvider\EFProviderDrives.h"

class MCountry :public CAbstractDatabaseModel
{
public:
	MCountry() : CAbstractDatabaseModel()
	{
		using std::placeholders::_1;
		RegisterProperty<std::string>(std::string("Name"));
		RegisterProperty<std::string>("Display");
	}
	void SetName(std::string name) { Name = name; }
	std::string GetName() { return Name; }
	void SetDisplay(std::string display) { Display = display; }
	std::string GetDisplay() { return Display; }

private:
	std::string Name;
	std::string Display;
};

int main()
{
	using namespace EFprovider;
	auto db_provider = new CSQLightEFprovider<MCountry>("MyDBFile.db");

	MCountry my_country;
	my_country.SetName("Malaysia");
	my_country.SetDisplay("Malaysia In East Asia");
	
	db_provider->Append(&my_country);
	db_provider->SaveChanges();

	std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
