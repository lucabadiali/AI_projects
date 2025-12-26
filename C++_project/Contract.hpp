#pragma once 

#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <tuple>
#include "utils.hpp"


using namespace std;

/**
 * @class Contract
 * @brief Represents a single contract with a client.
 *
 * Stores the name, amount, and date of the contract.
 */
class Contract
{
    private:
        string name;
        float money;
        tm datetime;   // to represent datetimes I used the ctime library which provides C-style like structs named tm designed to represent datetimes.

    public:

        // shared pointer to the Logger object
        static shared_ptr<Logger> logger;


        /** Default Public constructor for the Contract class.
         * This is used only when loading data from file.
         */
        Contract();

        /** Public constructor for the Contract class
         * @param _name: the name of the contract
         * @param _money: the amount of money assigned to this contract
         * @param _datetime_string: the string indicating the datetime when the contract was made
         */
        Contract(string& _name, float _money, string& _datetime_string);


        // getters and setters
        string get_name();
        float get_money();
        bool get_valid_datetime();
        tm get_datetime();

        void set_name(string& new_name);
        void set_money(float new_money);
        void set_datetime(string& new_datetime_string);

        /** Prints the contract information */
        void print();

        /** Less and Greater than operators that compare contracts chronologically depending on their datetime field
        */
        bool operator<=(const string datetime_string) const;
        bool operator>=(const string datetime_string) const;


        /** Friend functions to manage data saving and loading through the nlohmann json libray: https://github.com/nlohmann/json/releases/latest/download/json.hpp
        */
        friend void to_json(json& j, const Contract& contract);
        friend void from_json(const json& j, Contract& contract);
};



/**
 * @class ContractRecord
 * @brief Represents the collection of the existing contracts with a specific costumer.
 *
 * Stores the contracts and implements methods to manage the contracts, such as editing, adding or deleting.
 */
class ContractRecord
{   
    private:
        // vector of Contract objects
        vector<Contract> contract_record;

    public:

        /** Default Constructor for the class */ 
        ContractRecord();

        // getters and setters
        vector<Contract>& get_contract_record();


        // shared pointer to the Logger object
        static shared_ptr<Logger> logger;

        /** Prints the available information regarding all the customer's contracts */
        void print();

        /** Adds a new contract to the collection of existing contract objects contract_record
         * @param name: the name of the new contract
         * @param money: the amount of money the new contract is worth
         * @param datetime_string: the date when the contract was signed
        */
        void add_contract(string name, float money, string datetime_string);

        /** Deletes an existing contract
         * @param contract_to_delete: pointer to the Contract object to delete
        */
        void delete_contract(Contract* contract_to_delete);

        /** Checks if a contract with a given name already exists in the costumer's contract record
         * @param contract_name: name of the contract to look for
         * @returns pointer to the duplicate existing contract. If no duplicate is found the pointer is nullptr
        */
        Contract* search_contract_duplicate(string contract_name);


        /** Friend functions to manage data saving and loading through the nlohmann json libray: https://github.com/nlohmann/json/releases/latest/download/json.hpp
        */
        friend void to_json(json& j, const ContractRecord& contract_record);
        friend void from_json(const json& j, ContractRecord& contract_record);

};
