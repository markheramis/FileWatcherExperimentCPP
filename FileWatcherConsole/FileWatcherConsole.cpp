#include <iostream>
#include "FileWatcher.h"

int main()
{
	// Create a FileWatcher instance that will check the current folder for changes every 5 seconds
	FileWatcher fw{ "C:\\Users\\Mark\\Documents\\dicom-watch", std::chrono::milliseconds(5000) };
	// Start monitoring a folder for changes and (in case of changes)
	// run a user provided lambda function
	fw.start([](
		std::string watched_path, 
		FileStatus status
	) -> void {
		// Process only regular files, all other file types are ignored
		if (!std::filesystem::is_regular_file(std::filesystem::path(watched_path)) && status != FileStatus::erased) {
			return;
		}
		switch (status) {
			case FileStatus::created:
				std::cout << "File created: " << watched_path << '\n';
				break;
			case FileStatus::modified:
				std::cout << "File modified: " << watched_path << '\n';
				break;
			case FileStatus::erased:
				std::cout << "File erased: " << watched_path << '\n';
				break;
			default:
				std::cout << "Error! Unknown file status.\n";

		}
		});
	return 0;
}
