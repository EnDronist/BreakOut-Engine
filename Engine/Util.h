#pragma once
#include <string>
//#include <clocale>
//#include <locale>
#include <vector>
#include <codecvt>
#include <fstream>
std::wstring strtowstr(const std::string &str);
std::string wstrtostr(const std::wstring &wstr);
std::string fgetstr(std::string filename, int line);
const std::string fgetcstr(std::string filename, int line);