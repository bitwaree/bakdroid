/*-----------(START OF LICENSE NOTICE)-----------*/
/*
 * This file is part of HXO-loader.
 *
 * HXO-loader is licensed under the GNU General Public License v3.0
 * (GPL-3.0). You may copy, modify, and distribute it under the terms of the
 * GPL-3.0. This software is distributed WITHOUT ANY WARRANTY; see the GPL-3.0
 * for more details. You should have received a copy of the GPL-3.0 along
 * with this software. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2024 bitware
*/
/*-----------(END OF LICENSE NOTICE)-----------*/

//utils.c: Provides additional utilities for hxo-loader to work properly

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

#ifdef __bsd__
    #include <sys/sysctl.h>
#endif


void __attribute__((visibility("hidden"))) NameUtils::dircat(char *absolute, char *parent, char *child)
{
    //If child starts with a "/" take it as a absolute directory
    if(child[0] == '/') {
        //treat child as an absolute path
        strcpy(absolute, child);
    }
    else {
        //copy the parent directory first
        strcpy(absolute, parent);
        fixDIR(absolute);           //add a slash (/) is not already exists
        strcat(absolute, child);    //concat child
    }
}

int __attribute__((visibility("hidden"))) FileUtils::dirExists(const char *path) {
    struct stat info;

    // Use stat to get information about the path
    if (stat(path, &info) != 0) {
        // Error in accessing the path (e.g., it doesn't exist)
        return 0;
    } else if (info.st_mode & S_IFDIR) {
        // S_IFDIR bit is set, meaning it's a directory
        return 1;
    } else {
        // The path exists, but it's not a directory
        return 0;
    }
}

int __attribute__((visibility("hidden"))) FileUtils::fileExists(const char *filepath) {

    // Use fopen to create a handle at path
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        // Error in accessing the file (e.g., it doesn't exist)
        return 0;
    } else {
        // The path exists, but it's not a directory
        fclose(fp);
        return 1;
    }
}

int __attribute__((visibility("hidden"))) FileUtils::CopyFile(char *source_file, char *destination_file) {
    FILE *source = fopen(source_file, "rb");
    if (source == NULL) {
        fprintf(stderr, "[!] CopyFile failed: Could not open source file '%s'\n", source_file);
        return 1;
    }

    FILE *destination = fopen(destination_file, "wb");
    if (destination == NULL) {
        fprintf(stderr, "[!] CopyFile failed: Could not open destination file '%s'\n", destination_file);
        fclose(source);
        return 1;
    }

    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        if (fwrite(buffer, 1, bytes_read, destination) != bytes_read) {
            fprintf(stderr, "[!] CopyFile failed: Error writing to destination file '%s'\n", destination_file);
            fclose(source);
            fclose(destination);
            return 1;
        }
    }

    if (ferror(source)) {
        fprintf(stderr, "[!] CopyFile failed: Error reading from source file '%s'\n", source_file);
        fclose(source);
        fclose(destination);
        return 1;
    }

    fclose(source);
    fclose(destination);

    return 0;
}

void __attribute__((visibility("hidden"))) NameUtils::fixDIR(char *Dir)
{
    size_t tmp_length = strlen(Dir);
    if(Dir[tmp_length-1] != '/')
    {
        Dir[tmp_length] = '/';
        Dir[tmp_length+1] = '\0';
    }
}


#include <time.h>

void __attribute__((visibility("hidden"))) UnixSystemUtils::GetCurrentDateTime(int *year, int *month, int *day, int *hours, int *minutes) {
            // Get the current time
        time_t now;
        time(&now);

        // Convert to local time format
        struct tm *local = localtime(&now);

        // Extract date and time components
        *day = local->tm_mday;      // Day of the month (1-31)
        *month = local->tm_mon + 1; // Month (0-11, so add 1)
        *year = local->tm_year + 1900; // Year since 1900

        *hours = local->tm_hour;     // Hours (0-23)
        *minutes = local->tm_min;     // Minutes (0-59)
}

#include <signal.h>
int __attribute__((visibility("hidden"))) UnixSystemUtils::KillProcess(int PID) {
    //print a message
    fprintf(stdout, "[!!!] PID %d will now be killed.\n", PID);

    // Sleep for a few seconds to allow you to see the message

    // Kill the process using its own PID
    //kill(getpid(), SIGTERM);  //pookie   method
    //kill(getpid(), SIGKILL);  //gigachad method

    if (kill(PID, SIGTERM) == -1) {
        perror("Failed to kill the process");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS; // This line will not be reached
}

