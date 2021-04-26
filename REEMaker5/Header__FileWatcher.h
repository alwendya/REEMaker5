#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>
// Define available file changes
enum class FileStatus { created, modified, erased };

class FileWatcher {
public:
	std::wstring path_to_watch;
	// Time interval at which we check the base folder for changes
	std::chrono::duration<int, std::milli> delay;
	// Keep a record of files from the base directory and their last modification time
	FileWatcher(std::wstring path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{ path_to_watch }, delay{ delay } {
		for (auto& file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
			paths_[file.path().wstring()] = std::filesystem::last_write_time(file);
		}
	}
	// Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
	void start(const std::function<void(std::wstring, FileStatus)>& action) {
		while (running_) {
			// Wait for "delay" milliseconds
			std::this_thread::sleep_for(delay);
			auto it = paths_.begin();
			while (it != paths_.end()) {
				if (!std::filesystem::exists(it->first)) {
					action(it->first, FileStatus::erased);
					it = paths_.erase(it);
				}
				else {
					it++;
				}
			}
			// Check if a file was created or modified
			for (auto& file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
				auto current_file_last_write_time = std::filesystem::last_write_time(file);
				if (!contains(file.path().wstring())) {
					// File creation
					paths_[file.path().wstring()] = current_file_last_write_time;
					action(file.path().wstring(), FileStatus::created);
				}
				else
				{
					// File modification
					if (paths_[file.path().wstring()] != current_file_last_write_time) {
						paths_[file.path().wstring()] = current_file_last_write_time;
						action(file.path().wstring(), FileStatus::modified);

					}

				}

			}

		}

	}
private:
	std::unordered_map<std::wstring, std::filesystem::file_time_type> paths_;
	bool running_ = true;

	// Check if "paths_" contains a given key
	bool contains(const std::wstring& key) {
		auto el = paths_.find(key);
		return el != paths_.end();

	}

};


/*
* Code main
*/

/*
	FileHelper fwFileHelper;
	fwFileHelper.Initialise(L"C:\\tיאפמ\\");
	FileWatcher fw{ fwFileHelper.RetourneCheminCompletLong(), std::chrono::milliseconds(1000) };

	// Start monitoring a folder for changes and (in case of changes)
	// run a user provided lambda function
	fw.start([](std::wstring path_to_watch, FileStatus status) -> void {
		// Process only regular files, all other file types are ignored
		if (!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
			return;
		}

		switch (status) {
		case FileStatus::created:
			OutputDebugStringW(std::wstring(L"File created: " + path_to_watch + L"\n").c_str());
			break;
		case FileStatus::modified:
			OutputDebugStringW(std::wstring(L"File modified: " + path_to_watch + L"\n").c_str());
			break;
		case FileStatus::erased:
			OutputDebugStringW(std::wstring(L"File erased: " + path_to_watch + L"\n").c_str());
			break;
		default:
			std::cout << "Error! Unknown file status.\n";
		}
		});
*/