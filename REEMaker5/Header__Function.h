#pragma once
/*
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
* ░▒▓       _    _      _                   ______               _   _					▓▒░
* ░▒▓      | |  | |    | |                 |  ____|             | | (_)					▓▒░
* ░▒▓      | |__| | ___| |_ __   ___ _ __  | |__ ___  _ __   ___| |_ _  ___  _ __		▓▒░
* ░▒▓      |  __  |/ _ \ | '_ \ / _ \ '__| |  __/ _ \| '_ \ / __| __| |/ _ \| '_ \		▓▒░
* ░▒▓      | |  | |  __/ | |_) |  __/ |    | | | (_) | | | | (__| |_| | (_) | | | |		▓▒░
* ░▒▓      |_|  |_|\___|_| .__/ \___|_|    |_|  \___/|_| |_|\___|\__|_|\___/|_| |_|		▓▒░
* ░▒▓				     | |															▓▒░
* ░▒▓     			     |_|															▓▒░
* ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
*  REEMaker 5 __ Grégory WENTZEL (c) 2021
*/

class SpinLock {
	std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
	void lock() {
		while (locked.test_and_set(std::memory_order_acquire)) { ; }
	}
	void unlock() {
		locked.clear(std::memory_order_release);
	}
};
SpinLock sLock;
size_t NombreCPU()
{
	SYSTEM_INFO systemInfo;
	GetNativeSystemInfo(&systemInfo);
	if (systemInfo.dwNumberOfProcessors == NULL)
		return 2;
	else
		return systemInfo.dwNumberOfProcessors;
}

size_t wsplit(const std::wstring& txt, std::vector<std::wstring>& strs, wchar_t ch)
{
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::wstring::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

size_t split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}
std::wstring wGenerate(int max_length) {
	std::wstring possible_characters = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_int_distribution<> dist((int)0, (int)(possible_characters.size() - 1));
	std::wstring ret = L"";
	for (int i = 0; i < max_length; i++) {
		int random_index = dist(engine); //get index between 0 and possible_characters.size()-1
		ret += possible_characters[random_index];
	}
	return ret;
}
std::string sGenerate(int max_length) {
	std::string possible_characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_int_distribution<> dist((int)0, (int)(possible_characters.size() - 1));
	std::string ret = "";
	for (int i = 0; i < max_length; i++) {
		int random_index = dist(engine); //get index between 0 and possible_characters.size()-1
		ret += possible_characters[random_index];
	}
	return ret;
}
