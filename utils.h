namespace FileUtils{
	int __attribute__((visibility("hidden"))) dirExists(const char *path);
	int __attribute__((visibility("hidden"))) fileExists(const char *filepath);
	int __attribute__((visibility("hidden"))) CopyFile(char *source_file, char *destination_file);
}

namespace NameUtils{
	void __attribute__((visibility("hidden"))) dircat(char *absolute, char *parent, char *child);
	void __attribute__((visibility("hidden"))) fixDIR(char *Dir);
}

namespace UnixSystemUtils{
	void __attribute__((visibility("hidden"))) GetCurrentDateTime(int *_year, int *_month, int *_day, int *_hour, int *_minutes);
	int __attribute__((visibility("hidden"))) KillProcess(int PID);
}
