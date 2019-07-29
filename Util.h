#pragma once
#include <string>
//#include <clocale>
//#include <locale>
#include <vector>
#include <set>
#include <codecvt>
#include <fstream>
class Util {
public:
	static std::wstring strtowstr(const std::string &str) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
		return converterX.from_bytes(str);
		return std::wstring(str.begin(), str.end());
	}
	static std::string wstrtostr(const std::wstring &wstr) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
		return converterX.to_bytes(wstr);
		return std::string(wstr.begin(), wstr.end());
	}
	static std::string fgetstr(std::string filename, int line) {
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
	static const std::string fgetcstr(std::string filename, int line) {
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
	template<class T>
	static T* findptr(const std::vector<T*> &vector, const T *object) {
		for (T *element : vector) if (element == object) return element;
		return nullptr;
		/*typename std::vector<T*>::const_iterator iterator;
		iterator = std::find(vector.begin(), vector.end(), object);
		if (iterator != vector.end())
		return vector.at(std::distance(vector.begin(), iterator));
		else return nullptr;*/
	}
	template<class T>
	static T* findvalue(const std::vector<T*> &container, const T value) {
		for (T *element : container) if (element != nullptr && *element == value) return element;
		return nullptr;
	}
	template<class T>
	static void removeptr(std::vector<T*> &container, const T *object) {
		for (size_t i = 0; i < container.size(); i++) if (container[i] == object) {
			container.erase(container.begin() + i);
			i--;
		}
	}
	template<class T>
	static void deleteptr(std::vector<T*> &container, const T *object) {
		bool is_deleted = false;
		for (size_t i = 0; i < container.size(); i++) if (container[i] == object) {
			if (!is_deleted) { delete object; is_deleted = true; }
			container.erase(container.begin() + i);
			i--;
		}
	}
	template<class T>
	static bool havenullptr(const std::set<T*> &container) {
		for (T *element : container) if (element == nullptr) return true;
		return false;
	}
	template<class T>
	static bool havenullptr(const std::vector<T*>& container) {
		for (T *element : container) if (element == nullptr) return true;
		return false;
	}
	template<class T>
	static void removenullptr(std::set<T*> &container) {
		for (T *element : container) if (element == nullptr) {
			container.erase(element);
			return;
		}
	}
	template<class T>
	static void removenullptr(std::vector<T*> &container) {
		for (size_t i = 0; i < container.size(); i++) if (container[i] == nullptr) {
			container.erase(container.begin() + i);
			i--;
		}
	}
};