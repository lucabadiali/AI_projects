#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include "utils.hpp"
#include "Customer.hpp"
#include "CRM.hpp"



void from_json(const json& j, Customer& customer);



using namespace std;
using json = nlohmann::json; // json library https://github.com/nlohmann/json/releases/latest/download/json.hpp

CRM::CRM(){}

CRM::CRM(string logfile_path){

    this->logger = make_shared<Logger>(logfile_path);


    Customer::logger = this->logger;
    ContractRecord::logger = this->logger;
    Contract::logger = this->logger;

    this->customer_menu_possible_actions = 5;
    this->main_menu_possible_actions = 6;
    this->edit_customer_menu_possible_actions = 3;
    this->search_contract_menu_possible_actions = 4;
    this->contract_menu_possible_actions = 4;
    this->edit_contract_menu_possible_actions = 5;

    // call the main menu upon creation
    this->main_menu();
}


vector<Customer>& CRM::get_customer_record(){
    return this->customer_record;
}


void CRM::sort_alphabetically()
{
    (this->logger)->logfile << "Sorting customer list...";
    sort(this->customer_record.begin(), customer_record.end(), Person::compare_names_alphabetically);
    (this->logger)->logfile << "Done" << endl << SEPARATOR_LINE << endl;
}

void CRM::add_customer_CLI()
{

    (this->logger)->logfile << "Adding new customer process started... " << endl;

    ////////////////////////////////////////////////
    // handle CLI Interaction
    string name, surname;
    vector<string> user_input_strings;
    string prompt= "Enter the name and surname of the new customer (only alphabetical characters) separated by a space character. Type 'q' to cancel the operation." ;
    bool cancel_operation;

    (this->logger)->logfile << "Asking the user to enter input strings for name and surname... " << endl;
    // ask the user to enter name and surname, exactly two strictly alphabetical strings
    cancel_operation = read_user_input(user_input_strings, prompt, this->logger, true, 2);
    if(cancel_operation){
        return;
    }
    (this->logger)->logfile << "Input strings for name and surname read from user. " << endl;

    ////////////////////////////////////////////////
    // add the customer after parsing user input

    name = user_input_strings[0];
    surname = user_input_strings[1];
    
    this->add_customer(name, surname);
    (this->logger)->logfile << "Adding new customer process completed." << endl << SEPARATOR_LINE << endl;

}



void CRM::delete_customer(Customer* customer)
{        
    // find the iterator of the object to be deleted, use a lambda function for brevity
    auto iterator = find_if(this->customer_record.begin(), this->customer_record.end(), [customer](Customer& obj) { return &obj == customer; });
    if (iterator != this->customer_record.end()) {
        this->customer_record.erase(iterator);
        (this->logger)->logfile << " Done" << endl << SEPARATOR_LINE << endl;
        return;
    }
    else{
        (this->logger)->logfile << endl << "An error occurred trying to delete customer " << customer->get_name() << " " << customer->get_surname() << endl;
        throw runtime_error("\nAn error occurred trying to delete customer " + customer->get_name() + " " + customer->get_surname() + "\n");
    }
}





void CRM::add_customer(string name, string surname)
{
    (this->logger)->logfile << "Adding customer " << name << " " << surname << "..." << endl;

    // check if a customer with the same name already exists 
    (this->logger)->logfile << "Looking for potential duplicates of " << name << " " << surname << "...";
    vector<string> new_customer_id = {name, surname};
    vector<Customer*> potential_matches = this->search_customer_matches(new_customer_id);

    (this->logger)->logfile << " Done" << endl;

    if(potential_matches.size()!=0)
    {
        for(Customer* customer: potential_matches)
        {
            if((customer->get_name() == name) and (customer->get_surname() == surname)){
                cout << "A customer named " << name << " " << surname << " already exists." << endl;
                (this->logger)->logfile << "Adding customer not executed due to existing duplicate" << endl << SEPARATOR_LINE << endl;
                return;
            }
        }
    }

    // add the customer to the customer list if no duplicate exists
    this->customer_record.push_back(Customer(name, surname));
    (this->logger)->logfile << "Customer " << name << " " << surname << " Added." << endl;
}



// I tried to implement a fuzzy search functionality: the user can enter either one or two keywords. When entering only one keyword, that can be either the name or the surname. 
// A given customer is considered a potential match for the query if at least one of the user input words is a (case-insensitive) substring of the contact's name or surname.
vector<Customer*> CRM::search_customer_matches(vector<string> user_input_strings)
{
    vector<Customer*> potential_matches;

    string customer_name;
    string customer_surname;


    for(Customer& customer: this->customer_record){

        // make everyhting lowercase to enhance flexibility
        customer_name = to_lowercase(customer.get_name());
        customer_surname = to_lowercase(customer.get_surname());

        for(string word: user_input_strings){
            word = to_lowercase(word);
            if(customer_name.find(word) != string::npos){
                    potential_matches.push_back(&customer);
                    break;
            }
            else if(customer_surname.find(word) != string::npos){
                    potential_matches.push_back(&customer);
                    break;
            }
        }
    }
    
    return potential_matches;
}

/**
 * Prints the list of all the current customers. 
 * Before printing, the list of customers is sorted alphabetically 
*/
void CRM::print_customer_list()
{
    (this->logger)->logfile << "Printing customer list..." << endl;
    cout << SEPARATOR_LINE << endl;
    if(this->customer_record.size() == 0){
        cout << "No customer registered yet!" << endl;
        return;
    }

    string name, surname;
    cout << "Customer list:" << endl << endl;
    for(int i  = 0; i < customer_record.size(); i++)
    {

        Customer customer=this->customer_record[i];
        name = customer.get_name();
        surname = customer.get_surname();

        cout << i+1 << ") ";
        (this->logger)->logfile << "Printing id information for customer " << name << " " << surname << "...";
        customer_record[i].print_id();
        (this->logger)->logfile << " Done" << endl;
        cout << endl;
    }
    (this->logger)->logfile << "Customer list printed." << endl << SEPARATOR_LINE << endl;
    cout << SEPARATOR_LINE;
}


void CRM::search_customer_CLI()
{

    (this->logger)->logfile << "Search customer process started... " << endl;
    Customer* selected_customer;

    string prompt = "Enter the name and/or of the customer (only alphabetical characters). Type 'q' to cancel the operation.\n" ;
    vector<string> user_input_strings;
    bool cancel_operation;


    ////////////////////////////////////////////////
    // handle CLI Interaction

    while(true){
        (this->logger)->logfile << "Asking the user to enter input strings for name and/or surname. " << endl;
        cancel_operation = read_user_input(user_input_strings, prompt, this->logger, true);
        (this->logger)->logfile << "Input strings for name and/or surname read from user. " << endl;
            
        if(cancel_operation){
            return;
        }

        (this->logger)->logfile << "Validating number of user input words... ";
        // check that the user has entered at most two words
        if(user_input_strings.size() > 2){
            cout << "You can type at most two words. Try again." << endl;
            (this->logger)->logfile << "Not valid" << endl;
            continue;
        }
        (this->logger)->logfile << "Validated" << endl;
        break;

    }


    ////////////////////////////////////////////////
    // After reading and parsing user inputs, use the search keyword to look for matching customers

    selected_customer = this->select_customer(user_input_strings);


    (this->logger)->logfile << "Search customer process completed." << endl << SEPARATOR_LINE << endl;

    ////////////////////////////////////////////////
    // open the selected customer menu if a matching customer is found or if a customer is selected among potential mathing customers

    if(selected_customer == nullptr){
        return;
    }
    else{
        this->customer_menu(selected_customer);
        return;
    }

}




Customer* CRM::select_customer(vector<string> user_input_strings, bool CLI_mode)
{

    ////////////////////////////////////////////////////////////
    /// build a potential customer name by using the user input strings
    string potential_customer_name = "";
    for(string word: user_input_strings){
        potential_customer_name += word;
        potential_customer_name += " ";
    }

    trim_string(potential_customer_name);


    ////////////////////////////////////////////////////////////
    /// fuzzy search for matches
    vector<Customer*> potential_matches = this->search_customer_matches(user_input_strings);


    if(potential_matches.size()==0) // no match found
    {
        if((CLI_mode)){
            cout << endl << "No match was found for " << potential_customer_name << endl << SEPARATOR_LINE << endl;
        }
        return nullptr;

    }
    else{

        if(user_input_strings.size()==2){   // if the user entered both name and surname we check for a perfect match
            string name = user_input_strings[0];
            string surname = user_input_strings[1];

            for(Customer* potential_match: potential_matches){
                        if((potential_match->get_name() == name) and (potential_match->get_surname() == surname))  // if a perfect match is found (both name and surname correspond)
                        {
                            return potential_match;
                        }
            }  
        }

        // if the function is being called when loading data from a file, we only need to verify that an existing customer with the same name was not already present. So, at this point,
        // we can directly retun a null pointer to indicate that no duplicate was found.
        if(!(CLI_mode)){     
            return nullptr;
        }
        

        // if an exact match was not found but at least a potential match was found, give the user the chance to select one of the potential matches
        cout << endl << "No exact match was found. Did you mean one of these customers?" << endl;
        Customer customer;
        for(int i =0; i < potential_matches.size(); i++)
        {
            customer = *(potential_matches[i]);
            cout << i+1 << ") " << (customer).get_name() << " " << (customer).get_surname() << endl;
        }

        int user_choice;
        string prompt = "Type the number corresponding to a customer to select that customer. Insert -1 to cancel the operation.";

        read_user_menu_choice(user_choice, potential_matches.size(), prompt, this->logger, true);

        // if the user entered -1 return a null pointer
        if(user_choice == -1){
                     cout << endl << "Operation Cancelled." << endl << SEPARATOR_LINE  << endl;
                    (this->logger)->logfile << "Operation cancelled by the user" << endl << SEPARATOR_LINE << endl;
                    return nullptr;
        }
        
        return potential_matches[user_choice-1];
    } 
}






void CRM::edit_customer_id_CLI(Customer* customer, string id_field){
    

    (this->logger)->logfile << "Process for editing " << id_field <<  " for customer" << customer->get_name() << " " << customer->get_surname() << "started ..." << endl;

    string new_value;
    string prompt_id_field = "Enter the new " + id_field + " for the customer (only alphabetical characters). Type 'q' to cancel the operation." ;
    bool cancel_operation;
    vector<string> user_input_strings;


    ////////////////////////////////////////////////
    // handle CLI Interaction

    (this->logger)->logfile << "Asking the user to enter string..." << endl;
    cancel_operation = read_user_input(user_input_strings, prompt_id_field, this->logger, true, 1);
    if(cancel_operation){
        return;
    }
    (this->logger)->logfile << "Input string read from user" << endl;

    new_value = user_input_strings[0];

    ////////////////////////////////////////////////
    // After reading and parsing user input, edit coustomer's field
    if(id_field == "name"){
            customer->set_name(new_value);
    }
    else if(id_field == "surname"){
            customer->set_surname(new_value);
    }
    else{
        throw runtime_error("\nSomething went wrong in setting a new id field for customer " + customer->get_name() + " " + customer->get_surname() + "\n");
        (this->logger)->logfile << "Something went wrong in setting a new id field for customer " + customer->get_name() + " " + customer->get_surname() << endl;

    }

    (this->logger)->logfile << "Process for editing " << id_field <<  " for customer" << customer->get_name() << " " << customer->get_surname() << "completed ..." << endl;

}





void CRM::add_contract_CLI(Customer* customer)
{
    (this->logger)->logfile << "Adding contract process started..." << endl;

    //handle CLI Interaction
    string contract_name="";
    string datetime_string="";

    vector<string> user_input_strings;
    vector<int> user_input_ints;

    string user_input_string;
    float money = -1;
    bool cancel_condition = false;

    string name_prompt = "Enter the name for the new contract. Type 'q' to cancel the operation." ;
    string datetime_prompt= "Enter the date when the contract was signed off in the format " + date_format + " . Type 'q' to cancel the operation.";
    string money_prompt = "Enter a non-negative number for the amount of money the new contract is worth. Type 'q' to cancel the operation." ;
    

    ////////////////////////////////////////////////
    // handle CLI Interaction

    // read contract name from user

    (this->logger)->logfile << "Asking the user to enter input strings... ";
    cancel_condition = ask_user_input(user_input_strings, name_prompt);
    if(cancel_condition){
        return;
    }
    (this->logger)->logfile << "Done" << endl;


    // combining back the parsed strings into a single string
    for(string input_string: user_input_strings){
        contract_name += (input_string + " ");
    }

    // clear vector before proceeding
    user_input_strings.clear();
    
    // read contract datetime from user


    cancel_condition = read_user_input(user_input_strings, datetime_prompt, this->logger, false, 1, true);
    if(cancel_condition){
        return;
    }

    datetime_string = user_input_strings[0];

    // read contract money from user

    cancel_condition = read_user_input(money, money_prompt, this->logger, true);
    if(cancel_condition){
        return;
    }

    ////////////////////////////////////////////////
    // After reading and parsing user input, add the new contract
    
    customer->get_contract_record().add_contract(contract_name, money, datetime_string);


    cout << SEPARATOR_LINE << endl;
    (this->logger)->logfile << "Adding contract process completed." << endl << SEPARATOR_LINE << endl;


}


void CRM::main_menu()
{
    (this->logger)->logfile << "Opening Main Menu" << endl << SEPARATOR_LINE << endl;

    int user_choice;
    bool exit_menu = false;
    string prompt =   format("Enter your choice (1–{}): ", this->main_menu_possible_actions);


    std::string menu_message = R"(
==================== InsuraPro CRM Console ====================

Please choose an option below:

1: Add a new customer
2: View all customers
3: Search for a customer by name and/or surname
4: Save all customer data to file
5: Load customer data from file
6: Exit the application


===============================================================
)";

    while(!(exit_menu)){

        cout << menu_message << endl;

        read_user_menu_choice(user_choice, this->main_menu_possible_actions, prompt, this->logger);
    

        switch(user_choice){
                case 1:
                    this->add_customer_CLI();
                    break;
                case 2:
                    this->print_customer_list();
                    break;
                case 3:
                    this->search_customer_CLI();
                    break;
                case 4:
                    this->save_CLI();
                    break;
                case 5:
                    this->load_CLI();
                    break;
                case 6:
                    cout << endl << "Closing application..." << endl << SEPARATOR_LINE << endl;
                    exit_menu = true;

                    break;
            } 
        }
    (this->logger)->logfile << "Closing Application" << endl << SEPARATOR_LINE << endl;
}




void CRM::customer_menu(Customer* customer)
{
    (this->logger)->logfile << "Opening Customer Menu" << endl << SEPARATOR_LINE << endl;

    int user_choice = 0;
    string menu_message;
    bool exit_menu = false;
    string prompt =   format("Enter your choice (1–{}): ", this->customer_menu_possible_actions);

    while(!(exit_menu)){  
    

            menu_message = R"(
==================== Customer Menu ====================

Customer: )" + customer->get_name() + " " + customer->get_surname() + "\n" + R"(
Please choose an option below:

1: Print the customer's information
2: Edit the customer's id information
3: Open the contract menu for this customer
4: Delete the customer
5: Go back to main menu

===============================================================

)";

        cout << menu_message << endl;

        read_user_menu_choice(user_choice, this->customer_menu_possible_actions, prompt, this->logger);

        switch(user_choice){
            case 1:
                customer->print_complete_information();
                (this->logger)->logfile << SEPARATOR_LINE << endl;
                break;
            case 2:
                this->edit_customer_menu(customer);
                break;
            case 3:
                this->contract_menu(customer);
                break;
            case 4:
                (this->logger)->logfile << "Deleting customer " << customer->get_name() << " " << customer->get_surname() << "...";
                this->delete_customer(customer);
                exit_menu = true;
                break;
            case 5:
                exit_menu = true;
                break;
        }
    }
    (this->logger)->logfile << "Opening Main Menu" << endl << SEPARATOR_LINE << endl;
}


void CRM::edit_customer_menu(Customer* customer)
{

    (this->logger)->logfile << "Opening Edit Customer Menu" << endl << SEPARATOR_LINE << endl;

    int user_choice;
    bool exit_menu = false;
    string menu_message;
    string prompt = format("Enter your choice (1–{}): ", this->edit_customer_menu_possible_actions);
    

    while(!(exit_menu))
    {        
        menu_message = R"(
==================== Edit Customer Menu ====================

Customer: )" + customer->get_name() + " " + customer->get_surname() + "\n" + R"(
Please choose an option below:

1: Edit the customer's name
2: Edit the customer's surname
3: Go back to the previous menu

===============================================================

)";
        cout << menu_message << endl;
        read_user_menu_choice(user_choice, this->edit_customer_menu_possible_actions, prompt, this->logger);


        switch(user_choice){
            case 1:
                this->edit_customer_id_CLI(customer, "name");
                break;
            case 2:
                this->edit_customer_id_CLI(customer, "surname");
                break;
            case 3:
                exit_menu = true;
                break;
        }
    }
    (this->logger)->logfile << "Opening Customer Menu" << endl << SEPARATOR_LINE << endl;
}






void CRM::contract_menu(Customer* customer){
    (this->logger)->logfile << "Opening Contract Menu for customer " << customer->get_name() << " " << customer->get_surname() << endl << SEPARATOR_LINE << endl;

    int user_choice = 0;
    string menu_message;
    bool exit_menu = false;
    string prompt =   format("Enter your choice (1–{}): ", this->contract_menu_possible_actions);
    menu_message = R"(
==================== Contract Menu ====================

Customer: )" + customer->get_name() + " " + customer->get_surname() + "\n" + R"(
Please choose an option below:

1: Add a new contract
2: Print customer's contract record
3: Search existing contract
4: Go back to the previous menu

===============================================================

)";
    while(!(exit_menu)){
        cout << menu_message;
        read_user_menu_choice(user_choice, this->contract_menu_possible_actions, prompt, this->logger);


        switch(user_choice){
            case 1:
                this->add_contract_CLI(customer);
                break;
            case 2:
                (this->logger)->logfile << "Printing contract record for customer " << customer->get_name() << " " << customer->get_surname() << "..." << endl;
                customer->get_contract_record().print();
                (this->logger)->logfile << "Done";
                break;
            case 3:
                this->search_contract_menu(customer);
                break;
            case 4:
                exit_menu = true;
                break;
        }
    }
    (this->logger)->logfile << "Opening Customer Menu" << endl << SEPARATOR_LINE << endl;
}


void CRM::search_contract_menu(Customer* customer)
{

    (this->logger)->logfile << "Opening Search Customer Menu" << endl << SEPARATOR_LINE << endl;

    int user_choice;
    bool exit_menu = false;
    string menu_message;
    string prompt =   format("Enter your choice (1–{}): ", this->search_contract_menu_possible_actions);
    menu_message = R"(
==================== Search Contract Menu ====================

Customer: )" + customer->get_name() + " " + customer->get_surname() + "\n" + R"(
Please choose an option below:

1: Search contract by name
2: Search contract by datetime
3: Search contract by money
4: Go back to previous menu

===============================================================

)";    
    cout << menu_message;
    read_user_menu_choice(user_choice, this->search_contract_menu_possible_actions, prompt, this->logger);

    switch(user_choice){
        case 1:
            this->search_contract_by_name_CLI(customer);
            break;
        case 2:
            this->search_contract_by_datetime_CLI(customer);
            break;
        case 3:
            this->search_contract_by_money_CLI(customer);
            break;
        case 4:
            exit_menu = true;
            break;
    }
    
    (this->logger)->logfile << "Opening Customer Menu" << endl << SEPARATOR_LINE << endl;

}

void CRM::search_contract_by_name_CLI(Customer* customer){
    
    (this->logger)->logfile << "Searching for contract by name process started..." << endl;

    string initial_prompt = "Type a (sub)string which is contained in the contract's name. Type 'q' to cancel the operation.";
    vector<string> user_input_strings;
    string user_input_string;
    bool cancel_condition = false;
    vector<Contract*> matching_contracts;
    Contract* selected_contract;


    ////////////////////////////////////////////////
    // handle CLI Interaction
    (this->logger)->logfile << "Asking the user to enter string...";
    cancel_condition = read_user_input(user_input_strings, initial_prompt, this->logger);
    if(cancel_condition){
        return;
    }
    (this->logger)->logfile << " Done" << endl;

    // combining back the parsed strings into a single string in lowercase
    for(string parsed_string: user_input_strings){
        user_input_string += (to_lowercase(parsed_string) + " ");
    }

    trim_string(user_input_string);


    ////////////////////////////////////////////////
    // find matching contracts

    for(Contract& contract: customer->get_contract_record().get_contract_record()){

        if(to_lowercase(contract.get_name()).find(user_input_string) != string::npos){


            matching_contracts.push_back(&contract);
        }
    }

    ////////////////////////////////////////////////
    // let user choose among matching contracts
    selected_contract = select_contract(matching_contracts);

    cout << endl << SEPARATOR_LINE << endl;
    (this->logger)->logfile << "Searching for contract by name process completed..." << endl << SEPARATOR_LINE << endl;

    if(selected_contract == nullptr){

        return;
    }
    else{
        this->edit_contract_menu(customer, selected_contract);
    }

}

void CRM::search_contract_by_datetime_CLI(Customer* customer){

    (this->logger)->logfile << "Searching for contract by date process started...";

    string initial_prompt = "You need to specify a time period within which the contract datetime falls.";
    string start_datetime_prompt= "Enter a start date in the format " + date_format + " . Type 'q' to cancel the operation.";
    string end_datetime_prompt= "Enter a final date in the format " + date_format + " . Type 'q' to cancel the operation.";
    
    
    vector<string> user_input_strings;
    string start_datetime_string;
    string end_datetime_string;
    bool cancel_condition = false;
    vector<Contract*> matching_contracts;
    Contract* selected_contract;

    cout << initial_prompt << endl;

    ////////////////////////////////////////////////
    // handle CLI Interaction

    (this->logger)->logfile << "Asking the user to enter start datetime string..." << endl;
    cancel_condition = read_user_input(user_input_strings, start_datetime_prompt, this->logger, false, 1, true);
    if(cancel_condition){
            return;
    }
    (this->logger)->logfile << "Start datetime string read from user" << endl;

    start_datetime_string = user_input_strings[0];
    user_input_strings.clear();

    

    (this->logger)->logfile << "Asking the user to enter final datetime string...";
    read_user_input(user_input_strings, end_datetime_prompt, this->logger, false, 1, true);
    if(cancel_condition){
            return;
    }
    (this->logger)->logfile << "End datetime string read from user" << endl;
    end_datetime_string = user_input_strings[0];


    ////////////////////////////////////////////////
    // find matching contracts
    for(Contract& contract: customer->get_contract_record().get_contract_record()){

        if((contract >= start_datetime_string) && (contract <= end_datetime_string)){
            matching_contracts.push_back(&contract);
        }
    }


    ////////////////////////////////////////////////
    // let user choose among matching contracts

    selected_contract = select_contract(matching_contracts);


    (this->logger)->logfile << "Searching for contract by date process completed." << endl << SEPARATOR_LINE << endl;
    cout << endl << SEPARATOR_LINE << endl;


    if(selected_contract == nullptr){
        return;
    }
    else{
        this->edit_contract_menu(customer, selected_contract);
    }


}

Contract* CRM::select_contract(vector<Contract*> matching_contracts){

    int user_choice=0;

    if(matching_contracts.size() == 0){
        cout << endl << endl << "No existing contract matches your query." << endl;
        return nullptr;
    }
    else{
        cout << "The following contracts match your query." << endl;
        string prompt =  "Enter the corresponding number to select a specific contract.  Enter -1 to cancel the operation.";
        for(int i = 0; i < matching_contracts.size(); i++){
            cout << i +1 << ") ";
            matching_contracts[i]->print();
            cout << endl;
        }

        ////////////////////////////////////////////////
        // handle CLI interactions to let the user choose among existing matches

        read_user_menu_choice(user_choice, matching_contracts.size(), prompt, this->logger, true);
        if(user_choice == -1){ // cancel operation
                     cout << endl << "Operation Cancelled." << endl << SEPARATOR_LINE  << endl;
                    (this->logger)->logfile << "Operation cancelled by the user" << endl << SEPARATOR_LINE << endl;
                    return nullptr;
        }

    }

    // return selected contract
    return matching_contracts[user_choice-1];

}


void CRM::search_contract_by_money_CLI(Customer* customer){

    (this->logger)->logfile << "Searching for contract by money process started...";

    string initial_prompt = "You need to specify a money range within which the contract datetime falls.";
    string lower_money_prompt= "Enter a lower bound for the amount of money (only positive numbers). Type 'q' to cancel the operation.";
    string upper_money_prompt= "Enter an upper bound for the amount of money (only positive numbers). Type 'q' to cancel the operation.";
    vector<int> user_input_ints;

    float lower_money, upper_money;

    bool cancel_condition = false;
    vector<Contract*> matching_contracts;
    Contract* selected_contract;

      ////////////////////////////////////////////////
    // handle CLI interaction

    cout << initial_prompt << endl;
    (this->logger)->logfile << "Asking the user to enter a positive number...";
    cancel_condition = read_user_input(lower_money, lower_money_prompt, this->logger, true);
    if(cancel_condition){
            return;
    }
    (this->logger)->logfile << " Done" << endl;


    (this->logger)->logfile << "Asking the user to enter a positive number...";
    cancel_condition = read_user_input(upper_money, upper_money_prompt, this->logger, true);
    if(cancel_condition){
            return;
    }
    (this->logger)->logfile << " Done" << endl;


    ////////////////////////////////////////////////
    // find matching contracts

    for(Contract& contract: customer->get_contract_record().get_contract_record()){

        if((contract.get_money() >= lower_money) && (contract.get_money() <= upper_money)){
            matching_contracts.push_back(&contract);
        }
    }

    ////////////////////////////////////////////////
    // let the user choose among the potentialòy matching contracts

    selected_contract = select_contract(matching_contracts);
    if(selected_contract == nullptr){
        return;
    }
    else{
        this->edit_contract_menu(customer, selected_contract);
    }

    cout << endl << SEPARATOR_LINE << endl;
    (this->logger)->logfile << "Searching for contract by money process completed." << endl << SEPARATOR_LINE << endl;

}


void CRM::edit_contract_menu(Customer* customer, Contract* contract){
(this->logger)->logfile << "Opening Edit Contract Menu" << endl << SEPARATOR_LINE << endl;

    int user_choice;
    bool exit_menu = false;
    string prompt = format("Enter your choice (1–{}): ", this->edit_contract_menu_possible_actions);

    string menu_message = R"(
==================== Edit Contract Menu ====================

Please choose an option below:

1: Edit contract's name
2: Edit contract's datetime
3: Edit contract's money
4: Delete contract
5: Go back to the previous menu

===============================================================

)";

    while(!(exit_menu)){
        
        cout << menu_message;
        read_user_menu_choice(user_choice, this->edit_contract_menu_possible_actions, prompt, this->logger);
    
        switch(user_choice){
                case 1:
                    this->edit_contract_name_CLI(contract);
                    break;
                case 2:
                    this->edit_contract_datetime_CLI(contract);
                    break;
                case 3:
                    this->edit_contract_money_CLI(contract);
                    break;
                case 4:
                    (this->logger)->logfile << "Deleting contract...";
                    customer->get_contract_record().delete_contract(contract);
                    exit_menu = true;
                    (this->logger)->logfile << " Done";
                    break;
                case 5:
                    exit_menu = true;
                    break;
            } 
        }
    (this->logger)->logfile << "Opening Contract Menu" << endl << SEPARATOR_LINE << endl;
    return;
}




void CRM::edit_contract_name_CLI(Contract* contract){

    (this->logger)->logfile << "Edit contract name process started..." << endl;

    string prompt = "Type the new name for this contract: "; 
    bool cancel_condition = false;

    vector<string> user_input_strings;
    string new_name;


    (this->logger)->logfile << "Asking the user to enter input string..." << endl;

    ////////////////////////////////////////////////
    // handle CLI Interaction

    cancel_condition = read_user_input(user_input_strings, prompt, this->logger);
    if(cancel_condition){
            return;
    }
    (this->logger)->logfile << "User input string read." << endl;

    // combining back the parsed strings into a single string
    for(string input_string: user_input_strings){
        new_name += (input_string + " ");
    }

    ////////////////////////////////////////////////
    // edit contract name
    (this->logger)->logfile << "Setting new contract name..." << endl;
    contract->set_name(new_name);
    (this->logger)->logfile << " Done." << endl;

    (this->logger)->logfile << "Edit contract name process completed." << endl << SEPARATOR_LINE << endl;

}



void CRM::edit_contract_datetime_CLI(Contract* contract){

    (this->logger)->logfile << "Edit contract datetime process started..." << endl;

    string prompt = "Type the new datetime for this contract: "; 
    bool cancel_condition = false;

    vector<string> user_input_strings;
    string new_datetime;


    (this->logger)->logfile << "Asking the user to enter input datetime string ..." << endl;

    ////////////////////////////////////////////////
    // handle CLI Interaction
    cancel_condition = read_user_input(user_input_strings, prompt, this->logger, false, 1, true);
    if(cancel_condition){
            return;
    }
    (this->logger)->logfile << " Done" << endl;

    new_datetime = user_input_strings[0];

    ////////////////////////////////////////////////
    // edit contract datetime
    (this->logger)->logfile << "Setting new contract datetime...";
    contract->set_datetime(new_datetime);
    (this->logger)->logfile << " Done." << endl;

    (this->logger)->logfile << "Edit contract datetime process completed." << endl << SEPARATOR_LINE << endl;

}



void CRM::edit_contract_money_CLI(Contract* contract){

    (this->logger)->logfile << "Edit contract money process started..." << endl;

    string prompt = "Type the new amount of money for this contract: "; 
    bool cancel_condition = false;
    float new_money = -1;

    (this->logger)->logfile << "Asking the user to enter positive number ..." << endl;

    ////////////////////////////////////////////////
    // handle CLI Interaction
    cancel_condition = read_user_input(new_money, prompt, this->logger, true);
    if(cancel_condition){
            return;
    }
    (this->logger)->logfile << " Done" << endl;


    ////////////////////////////////////////////////
    // edit contract money
    (this->logger)->logfile << "Setting new contract money..." << endl;
    contract->set_money(new_money);
    (this->logger)->logfile << " Done." << endl;

    (this->logger)->logfile << "Edit contract money process completed." << endl << SEPARATOR_LINE << endl;


}





void CRM::load_CLI(){
    (this->logger)->logfile << "Loading data from file process started..." << endl;
    string prompt = "Type the path (relative or absolute) for the file where to load the data from. This must be a single string with no whitespaces. Type 'q' to cancel the operation.";
    bool cancel_condition = false;
    
    string file_path;
    vector<string> user_input_strings;


    ////////////////////////////////////////////////
    // handle CLI Interaction

    while(true){
        (this->logger)->logfile << "Asking the user to enter file path string..." << endl;
        cancel_condition = read_user_input(user_input_strings, prompt, this->logger, false, 1);
        (this->logger)->logfile << "User's file path string read." << endl;


        if(cancel_condition){
            return;
        }

        file_path = user_input_strings[0];

        (this->logger)->logfile << "Validating user input file path...";
         if(!(validate_path(file_path))){
            cout << "The path entered is not a valid a path. Try again." << endl;
            (this->logger)->logfile << " Not valid.";
            continue;
        }
        (this->logger)->logfile << " Validated." << endl;
        break;
    }
    
    ////////////////////////////////////////////////
    // load data
    (this->logger)->logfile << "Deserializing data process started..." << endl;
    json j;
    this->load(file_path, j);
    (this->logger)->logfile << "Deserializing data process completed" << endl;
    (this->logger)->logfile << "Loading data from file process completed." << endl << SEPARATOR_LINE << endl;


}

void CRM::save_CLI(){

    (this->logger)->logfile << "Saving data to file process started..." << endl;
    

    string prompt = "Type the path (relative or absolute) for the file where to save the data. This must be a single string with no whitespaces. Type 'q' to cancel the operation.";
    bool cancel_condition = false;
    
    string file_path;
    vector<string> user_input_strings;


     ////////////////////////////////////////////////
    // handle CLI Interaction

    while(true){
        (this->logger)->logfile << "Asking the user to enter file path string..." << endl;
        cancel_condition = read_user_input(user_input_strings, prompt, this->logger, false, 1);
        (this->logger)->logfile << "User's file path string read." << endl;


        if(cancel_condition){
            return;
        }

        file_path = user_input_strings[0];

        (this->logger)->logfile << "Validating user input file path...";
         if(!(validate_path(file_path))){
            cout << "The path entered is not a valid a path. Try again." << endl;
            (this->logger)->logfile << " Not valid.";
            continue;
        }
        (this->logger)->logfile << " Validated." << endl;
        break;
    }


     ////////////////////////////////////////////////
    // dump data

    (this->logger)->logfile << "Serializing data...";
    json j = *this;
    (this->logger)->logfile << " Done" << endl;

     (this->logger)->logfile << "Writing data to file...";
    this->save(file_path, j);
    (this->logger)->logfile << " Done" << endl;

    (this->logger)->logfile << "Saving data to file process completed." << endl << SEPARATOR_LINE << endl;
    return;
}


void CRM::save(string file_path, json& j){
    ofstream outFile(file_path);
    if (!outFile) {
        throw std::runtime_error("Could not open file: " + file_path);
        (this->logger)->logfile << "Could not open file: " << file_path << endl;
    }

    outFile << j.dump(4);  // 4 spaces for indent
    outFile.close();
    return;
}



void CRM::load(string file_path, json& j){
    ifstream input_file(file_path);
    if (!input_file) {
        throw std::runtime_error("Could not load data from file: " + file_path);
        (this->logger)->logfile << "Could not load data from file: " << file_path << endl;

    }
    input_file >> j;
    from_json(j, *this);
}



void to_json(json& j, const CRM& crm) {
    j = json{{"customer_record", crm.customer_record}};
}

void from_json(const json& j, CRM& crm) {

    // first load the data in a temporary CRM object
    CRM temp_crm;
    j.at("customer_record").get_to(temp_crm.customer_record);
    Customer* customer_duplicate = nullptr;
    vector<string> id, user_inpus;
    string name, surname, answer;
    string prompt; 

    // add customers from the temporary CRM object one by one manually, checking if there are duplicates with the currently loaded data
    for(Customer& customer: temp_crm.customer_record){
        name = customer.get_name();
        surname = customer.get_surname();
        prompt = string("Customer ") + name + string(" ") + surname + string(" already exists. Do you want to overwrite it? Type 'y' for yes and 'n' for no." );
        id = {name, surname};

        (*(crm.logger)).logfile << "Searching for potential duplicates of customer " << name << " " << surname <<  "...";
        customer_duplicate = crm.select_customer(id, false); // this checks that there is not already an existing customer with the same name and surname
        (*(crm.logger)).logfile << " Done." << endl;

        if(customer_duplicate == nullptr) // no duplicate is found, free to proceed with adding the new customer
        {   
            crm.customer_record.push_back(customer); 
        }
        else{    // if a duplicate is found, let the user decide if to overwrite ot not
            if(read_user_answer(prompt, crm.logger)){
                crm.delete_customer(customer_duplicate);
                crm.customer_record.push_back(customer); 
            }
        }
    }
}