C++ project Luca Badiali

===============================================================
Project structure:

- Contract.hpp: interface for the Contract and ContractRecord classes;
- Contract.cpp: source code for the Contract and ContractRecord classes;
- Costumer.hpp: interface for the Person and Costumer classes;
- Costumer.cpp: source code for the Person and Costumer classes;
- CRM.hpp: interfacer for the CRM class;
- CRM: source code for the CRM class;
- json.hpp: external library file, available at [nlohmann/json](https://github.com/nlohmann/json), for handling json loading and dumping of costum classes;
- utils.hpp: header file containing utility functions and logger class Logger

Project classes:

Logger – Manages logging of user actions during application execution.
Person – Base class containing identity fields (e.g., name and surname).
Customer – Inherits from Person, represents a customer with associated contracts.
Contract – Represents a single contract, including name, datetime, and amount.
ContractRecord – Manages a collection of contracts for a given customer.
CRM – Main class managing the overall system, containing all customers.

===============================================================
Logging:

I tried to implement a logging system that keeps track of the actions performed by the application in response to the user's inputs.
All the main classes involved in the management system have a shared_pointer to the same Logger object which allows logging to a unique file.
Classes that can be instantiated multiple times (e.g., Customer, Contract, ContractRecord) have a static shared pointer to the logger to avoid duplicating log streams.

The name of the logging file can be set in the main.cpp file

===============================================================
Functionalities:

Costumer

    - Add Customer: 
        - The user can add new customers. Upon creation the new customer's name and surname must be entered. These must be strictly alphabetical words;
        - Duplicate names are not allowed; if a match is found, the new customer is not added.


    - Search Customer: 
        - Users can perform fuzzy searches using one or two keywords (name and/or surname).
        - Matching is case-insensitive and based on substring presence;
        - Exact matches immediately open the customer menu; otherwise, the user can select from potential matches.

    - Edit Customer information: 
        After selecting a customer via the search functionality, the Costumer menu opens by which the user:
        - Edit the name and/or surname of the customer;
        - Delete the customer;
        - Open the contract menu for the customer, which allows for further actions, including editing of the contracts registered for the same customer.

Contracts

After accessing the contract menu for a given customer, it is possible to:

    - Add a new Contract: 
        - The user inputs a name, datetime, and monetary value;
        - If a contract with the same name exists for the customer, it won't be added.
    
    - Search a Contract by: 
        - Name (fuzzy match)
        - Datetime (by range)
        - Monetary value (by range)
        - Matching is fuzzy by default; user can choose from potential results.



    - Edit Contract Information:
        After selecting a contract via the contract search functionality, the user can:
        - Edit the various fields of the contract (name, datetime, money);
        - Delete the contract;


Data Saving and Loading

Via the main menu interface the user can:
    - Save the current data to a user-specified json file;
    - Load the content of a user-specified json file created before. Data is appended, not overwritten. If a customer is to be loaded with the same
    name and surname as an existing customer, the user is asked if the existing customer should be overwritten.

The external json library, see [nlohmann/json](https://github.com/nlohmann/json), manages data dumping and loading in a json format. This library
requires that methods from_json and to_json are defined for each costum Class defined in the program. 
For example, if class A contains an object of class B, then the from_json method from class A will automatically call the same method for class B, and so on recursively if needed.  

===============================================================
Compilation

To compile and run the project on a MAC laptop, run the following command:

clang++ -std=c++20 utils.hpp Customer.cpp CRM.cpp Contract.cpp main.cpp; if [ $? -eq 0 ]; then  ./a.out  ;  fi

A valid data.json that can be loaded is provided to make the application manual testing easier.

The application was manually tested. Unit Testing would be a great addition, but was not added due to time constraints.
===============================================================

