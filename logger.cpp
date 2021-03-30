#include "logger.hpp"

using namespace std;

std::string trim(std::string s){
    s.erase(0, s.find_first_not_of(WHITESPACE));
    s.erase(s.find_last_not_of(WHITESPACE) + 1);
    return s;
}

int log(string message, string type="NOTIFICATION"){
    ofstream file;
    file.open(FILENAME, std::ios_base::app | std::ios_base::in);
    time_t now = time(0);
    string time = ctime(&now);
    message = trim(message);
    time = trim(time);
    string logged_message = time + " [" + type + "]: " + message;  
    cout<<message<< endl;
    file<<logged_message<<endl;
    file.close();
    return 1;
}


//How to use the logger:
// int main(){
//     log("server_starting...", "START");
//     log("listning on port 8080");
//     log("connected to user with address of 123.13.83.1");
//     log("sent file to user");
// }