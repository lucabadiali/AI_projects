#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "utils.hpp"
#include "Customer.hpp"


using namespace std;



/**
 * @class CRM
 * @brief Implements a Customer Relationship Management system. 
 *
 * Stores and manages customers and their records.
 */
class CRM{
    private:
        // customers are stored in a vector of Customer objects
        vector<Customer> customer_record;

        // shared pointer to the Logger object
        shared_ptr<Logger> logger;

        // handle the CLI menu options
        int customer_menu_possible_actions;
        int main_menu_possible_actions;
        int edit_customer_menu_possible_actions;
        int search_contract_menu_possible_actions;
        int contract_menu_possible_actions;
        int edit_contract_menu_possible_actions;

    public:

        // default contructor, used in loading data from file
        CRM();

        /** Public constructor for the CRM class
         * @param logfile_path: the name/path to the logging file
        */
        CRM(string logfile_path);


         /** Getter for the collection of customer objects */
        vector<Customer>& get_customer_record();


        //////////////////////////////////////////////////////////////////
        // methods implementing menu interfaces

        /** Shows the main menu interface */
        void main_menu();

        /** Shows the customer menu interface */
        void customer_menu(Customer* customer);

        /** Shows the edit customer menu interface */
        void edit_customer_menu(Customer* customer);

        /** Shows the contract menu interface */
        void contract_menu(Customer* customer);

        /** Shows the edit contract menu interface */
        void edit_contract_menu(Customer* customer, Contract* contract);

        /** Shows the search contract menu interface */
        void search_contract_menu(Customer* customer);


        //////////////////////////////////////////////////////////////////
        // methods involving user interactions

        /** Allows the user to add a new constumer */
        void add_customer_CLI();

        /** Allows the user to search a customer among the already registered ones */
        void search_customer_CLI();


        /** Allows the user to add a new constumer
         * @param customer: pointer to the Customer object to which new contracts can be added
        */
        void add_contract_CLI(Customer* customer);

        /** Allows the user to search among contracts belonging to a specific customer
         * @param customer: pointer to the Customer whose contracts can be searched
        */
        void search_contract_CLI(Customer* customer);

        /** Allows the user to search among contracts belonging to a specific customer by their names
         * @param customer: pointer to the Customer whose contracts can be searched
        */
        void search_contract_by_name_CLI(Customer* customer);

        /** Allows the user to search among contracts belonging to a specific customer by their datetime
         * @param customer: pointer to the Customer whose contracts can be searched
        */
        void search_contract_by_datetime_CLI(Customer* customer);

        /** Allows the user to search among contracts belonging to a specific customer by their amount of money
         * @param customer: pointer to the Customer whose contracts can be searched
        */
        void search_contract_by_money_CLI(Customer* customer);

        /** Allows the user to edit the name of an existing contract
         * @param contract: pointer to the Contract object whose information can be edited
        */
        void edit_contract_name_CLI(Contract* contract);

        /** Allows the user to edit the datetime of an existing contract
         * @param contract: pointer to the Contract object whose information can be edited
        */
        void edit_contract_datetime_CLI(Contract* contract);

        /** Allows the user to edit the money for an existing contract
         * @param contract: pointer to the Contract object whose information can be edited
        */
        void edit_contract_money_CLI(Contract* contract);

        /** Edits the name or surname of a given customer
         * @param customer: pointer to the Customer object whose id field should be edited
         * @param id_field: string indicating whether the name or surname should be edited
         */
        void edit_customer_id_CLI(Customer* customer, string id_field);


        /** Allows the user to load customers' data from a file */
        void load_CLI();

        /** Allows the user to save customers' data into a file */
        void save_CLI();



        //////////////////////////////////////////////////////////////////
        ///// lower level actions on customers or customers' data that typically don't involve user interactions  
        /// (the only exception being the select_customer that may involve user interaction.)


        /** Adds a new customer to the customer list 
         * @param name: the name of the customer
         * @param surname: the surname of the customer
         * 
         * if duplicates are created by adding the new customer the 
         * process is stopped and a message is printed to the user
        */
        void add_customer(string name, string surname);


        /** Delete an existing customer
         * @param customer: pointer to the customer to delete
        */
        void delete_customer(Customer* customer);

        /**
         * Prints the list of all the current customers. 
         * Before printing, the list of customers is sorted alphabetically 
         */
        void print_customer_list();


        /** Retrieves all the existing customers that are a match for a fuzzy search by name and/or surname
         * @param user_input_string: vector of strings that can include the name and/or the surname of the customer to look for
         * @returns: a vector of pointers to the customers matching the fuzzy search
         */
        vector<Customer*> search_customer_matches(vector<string> user_input_strings);
    

        /** Tries to retrieve a customer after a fuzzy search by name and/or surname.
         * @param user_input_strings: vector of strings that can include the name and/or the surname of the customer to look for
         * @param CLI_mode: boolean variable to indicate if messages should be printed to screen. By default it is true, false is set
         * only when calling this function during data loading from file.
         * @return : a pointer to the customer retrieved by the search if an exact match was found,
         * or selected by the user if an exact match was not found. The pointer can be null
         * if the user cancels the operation of if no potentially matching customers were found
         * in the first place.
         */       
        Customer* select_customer(vector<string> user_input_strings, bool CLI_mode=true);

        /**
         * After a contract search which did not find an exact match, this function 
         * allows the user to select one of the potential matches that were found.
         * @param contracts: potential matching contracts that were found by searching
         * @return : a pointer to the contract by the user selected. The pointer can be null
         * if the user cancels the operation of if no potentially matching contracts were found
         * in the first place.
         */
        Contract* select_contract(vector<Contract*> contracts);

        /**
        * Sorts the customer list alphabetically
        */
        void sort_alphabetically();

        /**
         * allows the user to save customer data to a given file
         * @param file_path: path for the file to where data should be dumped
         * @param j: json object to manage json data serialization, see https://github.com/nlohmann/json/releases/latest/download/json.hpp
         */
        void save(string file_path, json& j);

        /**
         * allows the user to load customer data from a given file
         * @param file_path: path for the file from where data should be loaded
         * @param j: json object to manage json data deserialization, see https://github.com/nlohmann/json/releases/latest/download/json.hpp
         */
        void load(string file_path, json& j);

        /** Friend functions to manage data saving and loading through the nlohmann json libray: https://github.com/nlohmann/json/releases/latest/download/json.hpp
        */
        friend void to_json(json& j, const CRM& crm);
        friend void from_json(const json& j, CRM& crm);
};



