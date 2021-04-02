#include "json_parser.hpp"

using namespace std;


bool contains(string line, char c){
    return line.find(c) != string::npos;
}

string read_file_single_str(string path){
    ifstream input;
    input.open("config.json");
    std::string content( (std::istreambuf_iterator<char>(input) ), 
                (std::istreambuf_iterator<char>()));
    content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
    input.close();
    return content;
}

int read_intValue(string full_file, int &pos, int& pos2){
    pos = full_file.find(':', pos2 + 1);
    pos2 = full_file.find(",", pos + 1);
    string value = full_file.substr(pos+1, pos2 - pos - 1);
    cout<<"val: "<<value<<endl;
    return stoi(value);
}

User read_user_data(string user_data){
    User s;
    int pos = 0;
    int pos2 = -1;
    while(pos != -1 || pos != user_data.length()){
		pos = user_data.find('\"', pos2 + 1);
        pos2 = user_data.find("\"", pos + 1);
        if (pos == -1 || pos2 == -1) break;
        string key = user_data.substr(pos+1, pos2 - pos - 1);
        pos = user_data.find('\"', pos2 + 1);
        pos2 = user_data.find("\"", pos + 1);
        string value = user_data.substr(pos+1, pos2 - pos - 1);
        if (key == "user"){
            s.user = value;
        }else if (key == "password"){
            s.password = value;
        }else if (key == "admin"){
            s.admin = value == "true"? true : false;
        }else if (key == "size"){
            s.size = stoi(value);
        }
        if (pos == -1 || pos2 == -1) break;
	}
    return s;
}

vector<User> read_users (string full_file, int &pos, int& pos2){
    pos = full_file.find('[', pos2 + 1);
    pos2 = full_file.find("]", pos + 1);
    string value = full_file.substr(pos+1, pos2 - pos - 1);
    int npos = 0;
    int npos2 = -1;
    vector<User> users;
    while (npos != -1 || npos != value.length()){
        npos = value.find('{', npos2 + 1);
        npos2 = value.find("}", npos + 1);
        if (npos == -1 || npos2 == -1) break;
        string user_data = value.substr(npos+1, npos2 - npos - 1);
        users.push_back(read_user_data(user_data));
    }
    return users;
}

vector<string> read_files (string full_file, int &pos, int& pos2){
    pos = full_file.find('[', pos2 + 1);
    pos2 = full_file.find("]", pos + 1);
    string value = full_file.substr(pos+1, pos2 - pos - 1);
    int npos = 0;
    int npos2 = -1;
    vector<string> files;
    while (npos != -1 || npos != value.length()){
        npos = value.find('\"', npos2 + 1);
        npos2 = value.find("\"", npos + 1);
        if (npos == -1 || npos2 == -1) break;
        string file = value.substr(npos+1, npos2 - npos - 1);
        files.push_back(file);
    }
    return files;
}

int parse(string path, int &commandChannelPort, int &dataChannelPort, vector<User> &users, vector<string> &files){
    string full_file = read_file_single_str(path);
    int pos = 0;
    int pos2 = -1;
    while(pos != -1 || pos != full_file.length()){
		pos = full_file.find('\"', pos2 + 1);
        pos2 = full_file.find("\"", pos + 1);
        if (pos == -1 || pos2 == -1) break;
        string key = full_file.substr(pos+1, pos2 - pos - 1);
        if (key == "commandChannelPort"){
            commandChannelPort = read_intValue(full_file, pos, pos2);
        }else if (key == "dataChannelPort"){
            dataChannelPort = read_intValue(full_file, pos, pos2);
        }else if (key == "users"){
            users = read_users(full_file, pos, pos2);
        }else if (key == "files"){
            files = read_files(full_file, pos, pos2);
        }
        if (pos == -1 || pos2 == -1) break;
	}
    return 1;
}

// Note: for testing:

// int main(){
//     int command, data;
//     vector<string> files;
//     vector<User> users;
//     parse("config.json", command, data, users, files);
//     cout<<"command: " << command << endl << "data: " << data << endl;
//     cout<<"users: " <<endl;
//     for (int i = 0 ; i < users.size() ; i++){
//         User temp = users[i];
//         cout<< "user: " << temp.user << " pass: " <<temp.password << " admin: " << temp.admin << " data: "<< temp.size << endl;
//     }
//     cout<<"files: ";
//     for (int i = 0 ; i < files.size() ; i++){
//         cout<< files[i] << ", ";
//     }
//     cout<<endl;
// }