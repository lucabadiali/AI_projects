#pragma once 

#include <string>
#include <iostream>
#include "Contract.hpp"


using namespace std;


/**
 * @class Person
 * @brief Represents a person
 *
 * Stores basic id information about the person
 */
class Person
{
    protected:
        string name;
        string surname;
    public:
        Person();
        Person(string, string);

        // getters and setters
        void set_name(string);
        string get_name();
        void set_surname(string);
        string get_surname();

        /** Compares people's names alphabetically */
        static bool compare_names_alphabetically(Person&, Person&);

};


/**
 * @class Customer
 * @brief Represents a customer
 *
 * This class is used to change the Customer id information and to manage the customer's contracts via the contract_record field.
 */
class Customer: public Person
{
    private:
        ContractRecord contract_record;

    public:

        // shared pointer to the Logger object
        static shared_ptr<Logger> logger;   
        
        // empty constructor
        Customer();

        /** Public constructor for the Customer class
         * @param _name: the name of the customer
         * @param _surname: the surname of the customer
        */
        Customer(string _name, string _surname);

        /** Getter for the collection of contract objects */
        ContractRecord& get_contract_record();


        /** Prints the id information regarding the customer */
        void print_id();

        /** Prints all the available information regarding the customer */
        void print_complete_information();

        /** Friend functions to manage data saving and loading through the nlohmann json libray: https://github.com/nlohmann/json/releases/latest/download/json.hpp
        */
        friend void to_json(json& j, const Customer& customer);
        friend void from_json(const json& j, Customer& customer);

};







