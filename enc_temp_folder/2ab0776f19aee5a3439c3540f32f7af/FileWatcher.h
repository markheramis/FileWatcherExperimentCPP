#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <filesystem>
using std::string;
using std::function;
enum class FileStatus { created, modified, erased };

class FileWatcher {
	// ...
public:
	string watched_path;
	// Time interval at which we check the base folder for changes
	std::chrono::duration<int, std::milli> delay;

	FileWatcher(string watched_path, std::chrono::duration<int, std::milli> delay) : watched_path{ watched_path }, delay{ delay } {
		for (auto& file : std::filesystem::recursive_directory_iterator(watched_path)) {
			paths_[file.path().string()] = std::filesystem::last_write_time(file);
		}
	}

	// Monitor "watched_path" for changes and in case of a change execute the user supplied "action" function
	void start(const function<void(string, FileStatus)>& action) {
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
			for (auto& file : std::filesystem::recursive_directory_iterator(watched_path)) {
				auto current_file_last_write_time = std::filesystem::last_write_time(file);
				string file_path = file.path().string();

				if (!contains(file_path)) {
					// File creation
					paths_[file_path] = current_file_last_write_time;
					action(file_path, FileStatus::created);

				} else {
					// File modification
					if (paths_[file_path] != current_file_last_write_time) {
						paths_[file_path] = current_file_last_write_time;
						action(file_path, FileStatus::modified);
					}
				}
			}
		}
	}
private:
	std::unordered_map<string, std::filesystem::file_time_type> paths_;
	bool running_ = true;
	/*
	 * Check if "paths_" contains a given key
	 * if your compiler supports C++20 use paths.contains(key) instead of this function
	 */
	bool contains(const std::string& key) {
		auto el = paths_.find(key);
		return el != paths_.end();
	}
};