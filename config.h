#define BANNER_STR "\n\n\n" \
		"+-+-+-+-+-+-+-+-+\n" \
		"|B|a|k|D|r|o|i|d|\n" \
		"+-+-+-+-+-+-+-+-+\n" \
		"               -v%d.%d.%d\n\n"

#define VER_CODE_X 0
#define VER_CODE_Y 0
#define VER_CODE_Z 1

#define VER_CODE (VER_CODE_X << 8) | (VER_CODE_Y << 16) | (VER_CODE_Z << 24)

#define _MAX_PATH_LEN 4096           //Set maximum absolute path length
#define _MAX_FILE_LEN 512            //Set maximum file name length
#define _MAX_COMMAND_LEN 6144

#define RELATIVE_BACKUP_PATH "../"

#define TAR_EXCLUDE_LIST "--exclude=\"*HXO*\" --exclude=\"*hxo*\" --exclude=\"*BAKDROID*\" --exclude=\"*bakdroid*\""

