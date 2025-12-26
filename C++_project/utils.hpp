#pragma once 

#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <filesystem>
#include "json.hpp"



using json = nlohmann::json;
using namespace std;

/////////////////////////////////////////////////////////////////////
// inline definitions of global constants used by utility methods

inline const string date_format = "%Y:%m:%d";
inline const string SEPARATOR_LINE = "===============================================================";
inline const string cancel_string = "q";
inline const string error_msg = "Invalid input. Please try again.\n";
inline const string invalid_alphabetical_string_message = "You must enter strictly alphabetical strings. Try again.";
inline const string invalid_datetime_string_message = string("Invalid datetime string. The datetime string must be in the format ") + date_format + " Try again.";
inline const string invalid_money_message = "You must insert a positive value" ;
inline const vector<string> yes_no_possible_answers = {"y", "n"};


/**
 * @class Logger
 * @brief Implements a logger that keeps track of the actions executed by the CRM application
 */
class Logger{

    public:
        ofstream logfile;

        Logger(){}

        Logger(string logfile_name){
            this->logfile.open(logfile_name, std::ios::out);
            if (!(this->logfile)) {
                    throw std::runtime_error("Failed to open logging file: " + logfile_name);
            }
        }
};


/////////////////////////////////////////////////////////////////////
/////// Utility methods (I used inline functions to avoid creating an additional source file



/** Utility function to turn a string into lowercase only characters 
 * @param s: string to edit
 * @returns: a lowercased version of the input string
*/
inline string to_lowercase(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

/** Utility function to check if a given input string is made of strictly alphabetical characters
 * @param s: string to check
 * @returns: boolean value
*/
inline bool validate_only_alphabetical_string(string& s)
{
    for(char c: s)
    {
        if(!isalpha(c)){
            return false;
        }
    }
    return true;
}

/** Utility function to check if a given inpout character is a space character, used in validating user's input strings
 * @param ch: unsigned character to check, unsigned is used to avoid potential ambiguities with signed characters
 * @returns: boolean value
 * */
inline bool check_if_space_character(unsigned char ch)
{
    return !isspace(ch);
}


/** Utility function to trim the leading whitespace characters of an input string
 * @param ch: string whose potential leading whitespace characters are to be removed (in place) 
*/
inline void left_trim_string(string& s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), check_if_space_character));
}

/** Utility function to trim the trailing whitespace characters of an input string
 * @param ch: string whose potential trailing whitespace characters are to be removed (in place) 
*/
inline void right_trim_string(string& s) {
    s.erase(find_if(s.rbegin(), s.rend(), check_if_space_character).base(), s.end());
}

/** Utility function to remove the leading and trailing whitespaces from a string
 * @param s: string to trim
*/
inline void trim_string(string& s) {
    left_trim_string(s);
    right_trim_string(s);
}

/** Utility function to validate an input string as a good input datetime string that can be used to instantiate a meaningful tm struct from the ctime library
 * @param input_string: datetime string to be validated
 * @returns boolean value
*/
inline bool validate_datetime_string(string input_string)
{

    tm datetime_struct;
    // create a string stream as required by get_time
    istringstream in(input_string);

    // Parse into sys_days to check the validity of the input string. NB: get_time expects a C style string as second argument so we need to convert the string
    in >> get_time(&datetime_struct, date_format.c_str());

    // Check if parsing succeeded AND the whole input was consumed
    if (in.fail() || !in.eof()) {
        return false;
    }

    // check that the time fields make sense
    bool valid_year = datetime_struct.tm_year + 1900 >= 1900;
    bool valid_month = (datetime_struct.tm_mon +1  >= 1) && (datetime_struct.tm_mon +1 <= 12);
    bool valid_day = (datetime_struct.tm_mday >= 1 && datetime_struct.tm_mday <= 31);

    if(!(valid_year)){
        // cout << "Year not valid. ";
        return false;
    }
    else if(!(valid_month)){
        // cout << "Month not valid. ";
        return false;
    }
    else if(!(valid_day)){
        // cout << "Day not valid. ";
        return false;
    }
    else{
        return true;
    }
}


/** Utility template that takes care of reading and parsing a whole line of user input (numbers and strings in practice), and stores the element in a vector for later use.
 * @param user_input: vector of generic type T, meant to store the user's inputs
 * @param prompt: message to print to the user when asking for input
 * @returns boolean value indicating whether the user wants to cancel the operation for which input is being asked. True means cancel, False go on.
*/
template<typename T>
inline bool ask_user_input(vector<T>& user_input, string& prompt)
{
    string user_input_line;
    T user_input_element;


    while(true){

        user_input.clear();


        cout << prompt << endl;
        if (!getline(cin, user_input_line)){ // check if the input stream is in a good state after reading input from it

            if (cin.eof()) { // if EOF was detected return
                cerr << "\n[INFO] End of input detected.\n";
                exit(1);// signal EOF or input error
            }

            cin.clear(); // recover from other input stream errors by clearing
            cout << error_msg << endl;
            continue;
        }

        trim_string(user_input_line);
        // check for cancel condition before further parsing. If cancel condition is satisfied return immediately
        if(to_lowercase(user_input_line) == cancel_string){
            return true;
        }


        stringstream ss(user_input_line); // load the user's input line into a stream string for later access to the values

        // load the strings contained in the string stream into the vector
        while (ss >> user_input_element) {
            user_input.push_back(user_input_element);
        }

        // Additional check: test if the entire string stream was correctly parsed
        if (ss.fail() && !ss.eof()) {
            cerr << error_msg << endl;
            continue;
        }
        else{
            break;
        }
    }

    return false;
}


/** Utility function to read and validate user's answer to yes/no questions
 * @param prompt: message to print to the user when asking for input
 * @param logger: shared pointer to the Logger class object so that logging can be executed during the method's execution
 * @returns boolean value indicating whether the user said yes or no
*/
inline bool read_user_answer(string prompt, shared_ptr<Logger> logger){
   
    vector<string> user_inputs;
    string answer;
    while(true){
    
        logger->logfile << "Asking user yes/no question... " << endl;

        user_inputs.clear();
        ask_user_input(user_inputs, prompt);
        logger->logfile << "Validating user answer... "; 

        // the user must type just one word
        if(user_inputs.size()!=1){
            cout << "You can only type one word" << endl;
            logger->logfile << "Not valid" << endl;
            continue;
        }

        // make it lowercase to add a bit of flexibility
        answer = to_lowercase(user_inputs[0]);

        // the user must type only one of the 2 possible answers
        if((answer!=yes_no_possible_answers[0])&&(answer)!=yes_no_possible_answers[1]){
            cout << "Invalid answer. Try again." << endl;
            logger->logfile << "Not valid" << endl;
        }
        logger->logfile << "Validated" << endl;
        break;
    }

    if(answer==yes_no_possible_answers[0]){
        return true;
    }
    else{
        return false;
    }
}

/** Utility function to read and validate user's choice in a menu
 * @param user_choice: variable to store the user choice
 * @param number_menu_possible_actions: limits the number of possible and valid user choices
 * @param prompt: message to print to the user when asking for input
 * @param logger: shared pointer to the Logger class object so that logging can be executed during the method's execution
 * @param exit_option: boolean flag to indicate that the user can indicate exit the current menu by typing -1 (needed only when selecting the result of a search, otherwise menus typically have
 * an action corresponding to exit)
*/
inline void read_user_menu_choice(int& user_choice, int number_menu_possible_actions, string prompt, shared_ptr<Logger> logger, bool exit_option = false){
    vector<int> user_inputs;
    while(true){
        user_inputs.clear();
        ask_user_input(user_inputs, prompt);

        // the user must type just one number
        logger->logfile << "Validating number of user input values... ";
        if(user_inputs.size()!=1){
            cout << endl << "You can only choose one action" << endl;
            logger->logfile << "Not valid" << endl;
            continue;
        }
        logger->logfile << "Validated" << endl;

        user_choice = user_inputs[0];

        if(exit_option){
            if(user_choice == -1){
                return;
            }
        }

        logger->logfile << "Validating user menu choice... ";
        if((user_choice < 0) or (user_choice > number_menu_possible_actions)){
            cout << endl << "Invalid input. The possible choices are :";
            for(int i = 0; i < number_menu_possible_actions; i++){
                cout << " " << i+1;
            }
            cout << endl;
            logger->logfile << "Not valid" << endl;
            continue;
        }
        logger->logfile << "Validated" << endl;
        break;
    }
}



/** Utility function to read and validate user's input, specifically for float values
 * @param user_input: float value to store the user's input
 * @param prompt: message to print to the user when asking for input
 * @param logger: shared pointer to the Logger class object so that logging can be executed during the method's execution
 * @param positive_number_check: check if entered number is positive (needed for contracts)
 * @returns boolean value indicating whether the user wants to cancel the operation for which input is being asked. True means cancel, False go on.
*/
inline bool read_user_input(float& user_input, string& prompt, shared_ptr<Logger> logger, bool positive_number_check = false){
    vector<float> user_inputs;
    
    while(true){
        if(ask_user_input(user_inputs, prompt)){ // the user wants to cancel the current operation
            cout << endl << "Operation Cancelled." << endl << SEPARATOR_LINE  << endl;
            logger->logfile << "Operation cancelled by the user" << endl << SEPARATOR_LINE << endl;
            return true; 
        }

        // the user must insert exactly one value
        logger->logfile << "Validating number of user input values... ";
        if(user_inputs.size() != 1){
            cout << "You must enter exactly 1 input value. Try again." << endl;
            logger->logfile << "Not valid." << endl;
            continue;
        }
        logger->logfile << "Validated." << endl;
        
        user_input = user_inputs[0];

        // perform the non-negative check if required
        if(positive_number_check){ 
            logger->logfile << "Validating non-negative user input number... " << endl;
            if(user_input < 0){
                cout << invalid_money_message;
                logger->logfile << "Not valid." << endl;
                continue;
            }
            logger->logfile << "Validated." << endl;
        }
        break;
    }
    return false;
}



/** Utility function to read and validate user's inputs, specifically for strings
 * @param user_inputs: vector of strings to store the user's input
 * @param prompt: message to print to the user when asking for input
 * @param logger: shared pointer to the Logger class object so that logging can be executed during the method's execution
 * @param positive_number_check: check if entered number is positive (needed for contracts)
 * @param alphabetical_check: boolean flag to indicate whether the user input strings should be checked for being strictly alphabetical (needed for Customers' names)
 * @param accepted_number_of_inputs: by defaul is -1 to indicate ignore, otherwise specifies how many words the user must type for a correct interaction with the application
 * @param datetime_check: boolean flag to indicate whether the input string should be checked as valid to build a meaningful datetime struct (used for contracts)
 * @returns boolean value indicating whether the user wants to cancel the operation for which input is being asked. True means cancel, False go on.
*/
inline bool read_user_input(vector<string>& user_inputs, string& prompt, shared_ptr<Logger> logger, bool alphabetical_check = false, int accepted_number_of_inputs = -1, bool datetime_check = false){
    while(true){
        if(ask_user_input(user_inputs, prompt)){ // the user wants to cancel the current operation
            cout << endl << "Operation Cancelled." << endl << SEPARATOR_LINE  << endl;
            logger->logfile << "Operation cancelled by the user" << endl << SEPARATOR_LINE << endl;
            return true; 
        }

        // check the number of words
        if(accepted_number_of_inputs > 0){
            logger->logfile << "Validating number of user input values... ";
            if(user_inputs.size() != accepted_number_of_inputs){
                cout << "You must enter exactly" << accepted_number_of_inputs << "input elements. Try again." << endl;
                logger->logfile << "Not valid." << endl;
                continue;
            }
            logger->logfile << "Validated." << endl;
        }

        // alphabetical check
        if(alphabetical_check){
            logger->logfile << "Validating strictly alphabetical user input strings... ";
            for(string user_string: user_inputs){
                if(!(validate_only_alphabetical_string(user_string))){
                    cout << invalid_alphabetical_string_message << endl;
                    logger->logfile << "Not valid." << endl;
                    continue;
                }
            }
            logger->logfile << "Validated." << endl;
    
        }

        //datetime check
        if(datetime_check){
            logger->logfile << "Validating user input datetime string... " << endl;
            if(!(validate_datetime_string(user_inputs[0]))){
                cout << invalid_datetime_string_message << endl;
                logger->logfile << "Not valid." << endl;
                continue;
            }
            logger->logfile << "Validated." << endl;
        }
        break;
    }
    return false;
}


/** Utility function to check that an input string corresponds to a valid filesystem path. Used in data loading/saving processes
 * @param file_path: string indicating the path to check
 * @returns boolean value indicating whether the input string is a valid path or not
*/
inline bool validate_path(string& file_path) {
    try {
        filesystem::path p(file_path);
        return true; 
    } catch (const filesystem::filesystem_error& e) {
        cout << "Invalid path: " << e.what() << "\n";
        return false;
    }
}


/** Utility function to convert a ctime struct into a string
 * @param t: struct from the ctime library to represent datetimes
 * @returns string representing the information containing in the tm struct
*/
inline string format_tm(const tm& t) {
    ostringstream oss;
    oss << std::put_time(&t, "%Y:%m:%d "); 
    return oss.str();  
}



