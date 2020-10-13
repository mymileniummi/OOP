#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;
class ini_file
{
private:
	map<string, map<string, string>> info;
public:
	ini_file() = default;
	void add_section_variables(const string& section_name_, const string& param, const string& value) {
		info[section_name_].emplace(param, value);
	}
	void show() {
		for (auto cur = info.begin(); cur != info.end(); cur++)
		{
			cout << cur->first << "\n";
			for (auto cur_var = info[cur->first].begin(); cur_var != info[cur->first].end(); cur_var++)
			{
				cout << (*cur_var).first << ": ";
				cout << (*cur_var).second << "\n";
			}
		}
	}
	string get_value_string(string section_name_, string variable_) {
		regex string_("([\\w-./]+)");
		auto it = info[section_name_].find(variable_);
		if (it != info[section_name_].end()) {
			return info[section_name_][variable_];
		}
		else
			throw(exception("No such section or variable"));
	}
	int get_value_int(string section_name_, string variable_) {
		string value = get_value_string(section_name_, variable_);
		regex int_("([0-9]+)");
		if (regex_match(value.c_str(), int_)) {
			return atoi(value.c_str());
		}
		else {
			throw(exception("Types does not match"));
		}
	}
	double get_value_double(string section_name_, string variable_) {
		regex double_("([0-9]{1,})""(\\.)""([0-9]{1,})");
		string value = get_value_string(section_name_, variable_);
		if (regex_match(value.c_str(), double_)) {
			return atof(value.c_str());
		}
		else {
			throw(exception("Types does not match"));
		}
	}
};
void file_init(string file_name, ini_file& dictionary) {
	regex ini_file("([\\w]+)""\\.""(ini)");
	//проверка формата файла
		if  (! regex_match(file_name, ini_file)) {
			throw exception("That is not .ini file");
		}
	ifstream fin(file_name);
	//проверка на ошибку файловой сиситемы
	if (fin.fail()) {
		throw exception("Fail to read ini file");
	}

	string str;
	cmatch result;
	int last_section = -1;
	string str_value;
	vector<string> section_names;
	//регулярные выражения 
	regex section("(\\[)" "([\\w]+)""(\\])");
	regex parametr("[a-zA-Z]+");
	regex value("(=)""([\\s]*)""([\\w-./]+)");

	while (!fin.eof())
	{
		fin >> str;
		//провека если комментарий 
		if ((str == ";") || (str[0] == ';')) {
			getline(fin, str_value);
		}
		// проверка на секцию
		else if (regex_match(str.c_str(), section)) {
			last_section++;
			section_names.push_back(str);
		}
		// проверка на переменную со значением
		else if (regex_match(str.c_str(), parametr) && (last_section != -1)) {
			getline(fin, str_value);
			if (regex_search(str_value.c_str(), result, value)) {
				str_value = result[3];
				dictionary.add_section_variables(section_names[last_section], str, str_value);
			}
		}

	}
}


int main() {
	ini_file example_ini;
	try {
		file_init("file.ini", example_ini);
		example_ini.show();
		cout << example_ini.get_value_string("[COMMON]", "DiskCachePath") << "\n" ;
		cout << example_ini.get_value_int("[NCMD]", "EnableChannelControl") << "\n";
		cout << example_ini.get_value_double("[NCMD]", "SampleRate");
	} 
	catch(exception &ex) {
		cout << ex.what();
	}
	
	return 0;
}
