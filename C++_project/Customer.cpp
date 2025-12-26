
#include <string>
#include <iostream>
#include "Customer.hpp"
#include "utils.hpp"


using namespace std;

// definition of static field only declared in the header file
shared_ptr<Logger> Customer::logger;

Person::Person(){}

Person::Person(string _name, string _surname)
{
    this->name = _name;
    this->surname = _surname;
}

void Person::set_name(string new_name)
{
    this->name = new_name;
}

string Person::get_name()
{
    return this->name;
}

void Person::set_surname(string new_surname)
{
    this->surname = new_surname;
}

string Person::get_surname()
{
    return this->surname;
}


// this is used to sort the customers alphabetically
bool Person::compare_names_alphabetically(Person& p1, Person& p2)
{
    string name1 = to_lowercase(p1.get_name() + " " + p1.get_surname());
    string name2 = to_lowercase(p2.get_name() + " " + p2.get_surname()); 

    return name1 < name2;
}


Customer::Customer(){};

Customer::Customer(string _name, string _surname)
    : Person(_name, _surname)
{}


ContractRecord& Customer::get_contract_record(){
    return this->contract_record;
}


void Customer::print_complete_information()
{
    Customer::logger->logfile << "Printing complete information for customer " << name << " " << surname << "...";
    cout << endl << "Name: " << name << " " << endl << endl << "Surname: " <<  surname << endl  << endl;
    this->get_contract_record().print();
    Customer::logger->logfile << " Done" << endl;

}

void Customer::print_id()
{
    cout << name << " " << surname << endl;
}


void to_json(json& j, const Customer& customer) {
    j = json{
        {"name", customer.name},
        {"surname", customer.surname},
        {"contract_record", customer.contract_record}
    };
}

void from_json(const json& j, Customer& customer) {
    j.at("name").get_to(customer.name);
    j.at("surname").get_to(customer.surname);
    j.at("contract_record").get_to(customer.contract_record);
}
