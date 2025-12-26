#include <vector>
#include <ctime>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Contract.hpp"
#include "utils.hpp"



using namespace std;
using json = nlohmann::json;



// definition of static field only declared in the header file
shared_ptr<Logger> Contract::logger;


Contract::Contract(){};

Contract::Contract(string& _name, float _money, string& _datetime_string)
{
    this->name = _name;
    this->money = _money;
    this->datetime = {};
    istringstream in(_datetime_string);
    in >> get_time(&(this->datetime), date_format.c_str());

    // Check if parsing succeeded, this should never be a problem as _date_string_string should have been already validated when creating the contract object
    if (in.fail()) {
        Contract::logger->logfile << endl << "An error occurred trying to create a contract with datetime string " << _datetime_string << endl;
        throw runtime_error("\nAn error occurred trying to create a contract with datetime string " + _datetime_string  + "\n");
    }
}

void Contract::print()
{
    cout << "Contract name: " << this->name << endl;
    cout << "Date of the deal: " << this->datetime.tm_year + 1900 << ":" << this->datetime.tm_mon  + 1 << ":" << this->datetime.tm_mday << endl;
    cout << "Money: " << this->money << endl;
}



string Contract::get_name()
{
    return this->name;
}

float Contract::get_money()
{
    return this->money;
}


tm Contract::get_datetime()
{
    return datetime;
}

void Contract::set_name(string& new_name)
{
    this->name = new_name;
}

void Contract::set_money(float new_money)
{
    this->money = new_money;
}

void Contract::set_datetime(string& new_datetime_string)
{

    istringstream in(new_datetime_string);
    this->datetime = {}; // Zero it out
    in >> get_time(&(this->datetime), date_format.c_str());

    // Check if parsing succeeded, this should never be a problem as _date_string_string should have been already validated
    if (in.fail()) {
        Contract::logger->logfile << endl << "An error occurred trying to set a contract datetime with datetime string " << new_datetime_string << endl;
        throw runtime_error("\nAn error occurred trying to set a contract datetime with datetime string " + new_datetime_string  + "\n");
    } 

}



bool Contract::operator<=(const string datetime_string) const{

    tm temp_datetime_struct;
    istringstream in(datetime_string);
    in >> get_time(&(temp_datetime_struct), date_format.c_str());

    // Check if parsing succeeded, this should never be a problem as _date_string_string should have been already validated
    if (in.fail()) {
        Contract::logger->logfile << endl << "An error occurred trying to create a datetime struct by datetime string " << datetime_string << endl;
        throw runtime_error("\nAn error occurred trying to create a datetime struct by datetime string " + datetime_string  + "\n");
    }

    // compare year
    if(this->datetime.tm_year > temp_datetime_struct.tm_year) return false;
    else if(this->datetime.tm_year < temp_datetime_struct.tm_year) return true;
    else{     // compare month
        if(this->datetime.tm_mon > temp_datetime_struct.tm_mon) return false;
        if(this->datetime.tm_mon < temp_datetime_struct.tm_mon) return true;
        else{ // compare day 
            if(this->datetime.tm_mday > temp_datetime_struct.tm_mday) return false;
            else return true;
        }
    }
}

bool Contract::operator>=(const string datetime_string) const{

    tm temp_datetime_struct;
    istringstream in(datetime_string);
    in >> get_time(&(temp_datetime_struct), date_format.c_str());

    // Check if parsing succeeded, this should never be a problem as _date_string_string should have been already validated
    if (in.fail()) {
        Contract::logger->logfile << endl << "An error occurred trying to create a datetime struct by datetime string " << datetime_string << endl;
        throw runtime_error("\nAn error occurred trying to create a datetime struct by datetime string " + datetime_string  + "\n");
    }

    // compare year
    if(this->datetime.tm_year < temp_datetime_struct.tm_year) return false;
    else if(this->datetime.tm_year > temp_datetime_struct.tm_year) return true;
    else{     // compare month
        if(this->datetime.tm_mon < temp_datetime_struct.tm_mon) return false;
        if(this->datetime.tm_mon > temp_datetime_struct.tm_mon) return true;
        else{ // compare day 
            if(this->datetime.tm_mday < temp_datetime_struct.tm_mday) return false;
            else return true;
        }
    }
}

void to_json(json& j, const Contract& contract) {
    j = json{
        {"name", contract.name},
        {"money", contract.money},
        {"datetime", format_tm(contract.datetime)}
    };
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CONTRACT RECORD CLASS


// definition of static field only declared in the header file
shared_ptr<Logger> ContractRecord::logger;

void ContractRecord::print()
{

    if(this->contract_record.size()==0)
    {
        cout << endl << "No contracts registered for this costumer yet." << endl;
    }
    else{

        cout << endl << "Contract Record:" << endl;

        for(int i = 0; i < this->contract_record.size(); i++)
        {

            cout << endl << endl << "Contract " << i+1 << ") " << endl << endl;
            this->contract_record[i].print();
            cout << endl << endl;
        }
    }
    
}


ContractRecord::ContractRecord(){}


vector<Contract>& ContractRecord::get_contract_record(){
    return this->contract_record;
}

Contract* ContractRecord::search_contract_duplicate(string contract_name){

    Contract* duplicate_contract = nullptr;
    for(Contract& contract: this->contract_record){
        if(contract.get_name() != contract_name){
            continue;
        } 
        else{
            duplicate_contract = &contract;
            break;
        }
    }
    return duplicate_contract;
}


void ContractRecord::add_contract(string contract_name, float money, string datetime_string)
{

    // check if a contract with the same dat already exists 
    ContractRecord::logger->logfile << "Looking for a potential duplicate of contract with the same name...";
    Contract* potential_duplicate = this->search_contract_duplicate(contract_name);
    ContractRecord::logger->logfile << " Done" << endl;

    if(potential_duplicate != nullptr){
    
        cout << "A contract named " << contract_name << " already exists." << endl;
        ContractRecord::logger->logfile << "Adding contract not executed due to existing duplicate" << endl << SEPARATOR_LINE << endl;
        return;
    }

    // if no duplicate was found proceed
    ContractRecord::logger->logfile << "Adding contract with name " << contract_name << ", money " << money << " and datetime " << datetime_string << "...";
    Contract new_contract(contract_name, money, datetime_string);
    this->contract_record.push_back(new_contract);
    ContractRecord::logger->logfile << " Done"  << endl;
}

void ContractRecord::delete_contract(Contract* contract_to_delete){

    // find the iterator of the object to be deleted, use a lambda function to specify the matching criteria for brevity
    auto iterator = find_if(this->contract_record.begin(), this->contract_record.end(), [contract_to_delete](Contract& obj) { return &obj == contract_to_delete; });
    if (iterator != this->contract_record.end()) {
        this->contract_record.erase(iterator);
        return;
    }
    else{
        ContractRecord::logger->logfile << endl << "An error occurred trying to delete contract " << contract_to_delete->get_name() << endl;
        throw runtime_error("\nAn error occurred trying to delete contract" + contract_to_delete->get_name() + "\n");
    }
}


void to_json(json& j, const ContractRecord& contract_record) {
    j = json{
        {"contract_record", contract_record.contract_record}
    };
}

void from_json(const json& j, ContractRecord& contract_record) {
    for (const auto& item : j.at("contract_record")) {
        std::string name;
        int money;
        std::string datetime;
        item.at("name").get_to(name);
        item.at("money").get_to(money);
        item.at("datetime").get_to(datetime);
        contract_record.add_contract(name, money, datetime);
    }
}