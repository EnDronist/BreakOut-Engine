#pragma once
#include "Util.h"
std::wstring strtowstr(const std::string &str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.from_bytes(str);
	return std::wstring(str.begin(), str.end());
}
std::string wstrtostr(const std::wstring &wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.to_bytes(wstr);
	return std::string(wstr.begin(), wstr.end());
}
std::string fgetstr(std::string filename, int line) {
	std::ifstream fin(filename);
	std::string text;
	if (fin.is_open()) {
		int i = 0;
		do {
			i++;
			getline(fin, text);
			if (i == line) break;
			else text.clear();
		} while (!fin.eof());
	}
	return text;
}
const std::string fgetcstr(std::string filename, int line) {
	std::ifstream fin(filename);
	std::string text;
	if (fin.is_open()) {
		int i = 0;
		do {
			i++;
			getline(fin, text);
			if (i == line) break;
			else text.clear();
		} while (!fin.eof());
	}
	return text;
}