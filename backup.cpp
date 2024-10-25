#include <iostream>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "INIReader.h"
#include "backup.h"
#include "utils.h"


//for extra directory scan
#include <sys/types.h>
#include <dirent.h>


class DroidBackup
{
public:
	char privateDataPath[512];
	char sharedDataPath[512];
	char sharedOBBPath[512];
	char sharedMediaPath[512];

	char INIConfigFile[512];
	char backupFile[1024];

	char tarCommand[128];

	DroidBackup(char *APP_ID, char *modulePath);
	bool CheckTar();			//Check if tar command works
	//finds the appropiate rw backup file
	int SetupBackupFile(char *APP_ID, char *modulePath, int task, char algo);

};

class INIConfig {
public:
	bool configExists;	//if ini file exists
	int taskstat;      //Backup or Restore???
						//= 0 Nothing, Don't do anything
						//= 1 Backup
						//= 2 Restore


	bool excludeData;	//if data path is excluded
	bool excludeOBB;	//if OBB path is excluded
	bool excludeMedia;	//if Media path is excluded

	char Algorithm;     //For compression ???
						//= 'z' -> gzip (default)
						//= 'J' -> xz
						//= 'j' -> bzip2

	INIConfig(char* INIFile);	//constructor
	int Disable(char* INIFile);		//Disable/set task=0 to avoid accidental next run
};

int __attribute__((visibility("hidden"))) SelfKiller();

int initBackup(char *APP_ID, char *modulePath)
{
	printf("[...] Initializing bakdroid...\n");

	//SetupPaths
	class DroidBackup backup(APP_ID, modulePath);
	//Parse ini config
	class INIConfig config(backup.INIConfigFile);
	

	if(!config.configExists || !config.taskstat) {
		//If file doesn't exist
		printf("[!] Config not found or, no task specified in config ...\n");
		return 1;
	}
	if(!backup.CheckTar()) {
		//If file doesn't exist
		fprintf(stderr, "[X] %s command failed ...\n", backup.tarCommand);
		return -1;
	}

	//Continue
	char *command_run = (char*) malloc(_MAX_COMMAND_LEN);
	char *backupFile = (char*) malloc(1024);
	int tarExitValue = 1;

	//Setup backup file path
	NameUtils::dircat(backupFile, modulePath, (char*)RELATIVE_BACKUP_PATH);


	if (config.taskstat == 1)
	{
		//perform backup
		backup.SetupBackupFile(APP_ID, modulePath, config.taskstat, config.Algorithm);	//Setup the output backup file
		NameUtils::dircat(backupFile, backupFile, backup.backupFile);
		//setup exclude
		if (config.excludeData == 1)
		{backup.sharedDataPath[0] = (char) 0;    backup.privateDataPath[0] = (char) 0;}
		if (config.excludeMedia == 1)
			backup.sharedMediaPath[0] = (char) 0;
		if (config.excludeData == 1)
			backup.sharedOBBPath[0] = (char) 0;

        sprintf(command_run, "%s %s -c%cf %s %s %s %s %s", backup.tarCommand, TAR_EXCLUDE_LIST,
        	config.Algorithm, backupFile,
         	backup.privateDataPath, backup.sharedDataPath, backup.sharedOBBPath, backup.sharedMediaPath);
        //disable to avoid accdental next run
        //config.Disable(backup.INIConfigFile);

	} else if (config.taskstat == 2) {
		//perform backup restoration
		//find files
		switch (backup.SetupBackupFile(APP_ID, modulePath, config.taskstat, config.Algorithm))
		{
			case 0:
				fprintf(stderr, "[X] No Backup File found! can't restore backup\n");
				free(command_run);
				free(backupFile);
				return -1;
				break;
			case 2:
			    fprintf(stdout, "[X] Multiple Backup File found! Selecting: \"%s\"\n", backup.backupFile);
			    break;
			case 1:
				printf("[*] Backup file found: \"%s\"\n", backup.backupFile);
				break;
		}
		//Get the full path
		NameUtils::dircat(backupFile, backupFile, backup.backupFile);

        sprintf(command_run, "%s -xf %s -C /", backup.tarCommand, backupFile);
        //disable to avoid accdental next run
        config.Disable(backup.INIConfigFile);
	}

	//debug
    fprintf(stdout, "\n[*] TAR COMMAND = %s\n", command_run);
    fprintf(stdout, "\n\n:---------------(PROCEEDING TO RUN TAR)---------------:\n");
    tarExitValue = system(command_run);
    fprintf(stdout, "\n:---------------(END OF TAR COMMAND, EXIT: %d)---------------:\n\n", tarExitValue);

	free(command_run);
	free(backupFile);
	
//	if(tarExitValue == 0)
//	{
		printf("[+] The task has been completed. The app will now exit by itself!\n");
		SelfKiller();	//kill itself if tar succeeds
//	}

	return 0;
}

DroidBackup::DroidBackup(char *APP_ID, char *modulePath)
{
	//get the /data/data/<APP_ID>
	NameUtils::dircat(this->privateDataPath, "/data/data/", APP_ID);
	//get the /storage/emulated/0/Android/data/<APP_ID>
	NameUtils::dircat(this->sharedDataPath, "/storage/emulated/0/Android/data/", APP_ID);
	//get the /storage/emulated/0/Android/obb/<APP_ID>
	NameUtils::dircat(this->sharedOBBPath, "/storage/emulated/0/Android/obb/", APP_ID);
	//get the /storage/emulated/0/Android/obb/<APP_ID>
	NameUtils::dircat(this->sharedMediaPath, "/storage/emulated/0/Android/media/", APP_ID);


	NameUtils::dircat(this->INIConfigFile, modulePath, "bakdroid.ini");
	//Initialize other stuffs
	NameUtils::dircat(this->tarCommand, "/system/bin/", "tar");
}

bool DroidBackup::CheckTar()
{
	char tarCommand_testrun[256];
	strcpy(tarCommand_testrun, this->tarCommand);
	strcat(tarCommand_testrun, " --version");	

	FILE *testrun = popen(tarCommand_testrun, "r");
	if(!testrun) {
		//tar command fail
		return false;
	} else {
		pclose(testrun);
		return true;
	}
}

int DroidBackup::SetupBackupFile(char *APP_ID, char *modulePath, int task, char algo) {
	//for backup ..
	if(task == 1)
	{
		//Get the date and time
		int year, month, day, hours, minutes;
    	UnixSystemUtils::GetCurrentDateTime(&year, &month, &day, &hours, &minutes);
    	//set the file extention
    	char file_extention[8];
    	switch(algo) {
    		case 'z':
    			strcpy(file_extention, "gz"); break;
    		case 'j':
    			strcpy(file_extention, "bz2"); break;
    		case 'J':
    			strcpy(file_extention, "xz"); break;
    		}

		sprintf(this->backupFile, "BAKDROID_%s_%04d%02d%02d_%02d%02d.tar.%s", APP_ID, year, month, day, hours, minutes, file_extention);
	}
	else if (task == 2) {
		//scan for files...
		int count = 0;
		char dir_path[1024];
		NameUtils::dircat(dir_path, modulePath, RELATIVE_BACKUP_PATH);


		struct dirent *entry;
    	DIR *dp = opendir(dir_path);

    	if (dp == NULL) {
        	perror("opendir");
        	return 0;
    	}

    	while ((entry = readdir(dp)) != NULL) {
        	// Check if the file name starts with "BAKDROID" and ends with "tar.*"
        	if (strncmp(entry->d_name, "BAKDROID", 8) == 0) {
            	// Check if it ends with "tar" followed by any extension
            	char *dot = strrchr(entry->d_name, '.');
            	if (dot != NULL && strncmp(dot - 3, "tar", 3) == 0) {
                	strcpy(this->backupFile, entry->d_name);
                	count++;
            	}
        	}
    	}
	
    	closedir(dp);

    	//if no file found...
    	if (count < 1)
    	{
    		return 0;  //no file found
    	}
    	else if (count > 1)
    	{
    		return 2;  //more than one file found
    	}
	}
	return 1;		   //one file found/destination set ..
}

INIConfig::INIConfig(char *INIFile) {
	
	//Check if ini file exists in the first place
	this->configExists = FileUtils::fileExists(INIFile);
	if(!configExists) {
		//if config don't exist
		return;
	}

	//parse inifile
	INIReader reader(INIFile);

	this->taskstat = reader.GetInteger("bakdroid", "task", 0);
	if (taskstat != 0 && taskstat != 1 && taskstat != 2) {
		fprintf(stderr, "[!] Invalid task parameter: %d\n", (int) reader.GetInteger("bakdroid", "task", 0));
		this->taskstat = 0;
	}

	this->excludeData = reader.GetInteger("config", "excludeData", 0);
	this->excludeOBB = reader.GetInteger("config", "excludeOBB", 0);
	this->excludeMedia = reader.GetInteger("config", "excludeMedia", 0);

	if ("gzip" == reader.Get("options", "algorithm", "gzip") ||
		"gz" == reader.Get("options", "algorithm", "gzip")) {
		this->Algorithm = 'z';  //gzip = 1 
	} else if ("xz" == reader.Get("options", "algorithm", "gzip")) {
		this->Algorithm = 'J';  //xz = 1
	} else if ("bzip2" == reader.Get("options", "algorithm", "gzip") ||
		"bz2" == reader.Get("options", "algorithm", "gzip")) {
		this->Algorithm = 'j';  //bzip2 = 1
	} else {
		fprintf(stderr, "[!] Invalid compression algorithm passed: %s, selecting default compression: %s\n",
		reader.Get("options", "algorithm", "gzip").c_str(), "gzip");
		this->Algorithm = 'z';
	}

	return;
}

int INIConfig::Disable(char *INIFile)
{
	size_t MAX_INIBUFFER_LEN = 8*1024;
	char *buffer = (char*) malloc(MAX_INIBUFFER_LEN);  //assume the max length of 8KB
	memset(buffer, 0, MAX_INIBUFFER_LEN);
	FILE *inifile = fopen(INIFile, "r");
	if(!inifile)
	{
		free(buffer);
		return 0;
	}
	//read the data
	size_t bytesRead = fread(buffer, 1, MAX_INIBUFFER_LEN, inifile);
	fclose(inifile);

	char *task_pos = strstr(buffer, "task=");
    if (task_pos != NULL) {
        // Move the pointer to the first digit after "status="
        task_pos += strlen("task=");
        
        // Check if the next character is a digit
        if (isdigit(*task_pos)) {
            // Find the end of the digit sequence
            char *digit_start = task_pos;
            while (isdigit(*task_pos)) {
                task_pos++;
            }
            
            // Replace the digit(s) with '0'
            // We can replace the first digit and then null-terminate the rest
            *digit_start = '0';
            if (task_pos != digit_start + 1) {
                // If there are more digits, null-terminate the string after the first '0'
                strcpy(digit_start + 1, task_pos);
            }
        }
    }

    //write the modified data back
    inifile = fopen(INIFile, "w");
   	if(!inifile)
	{
		free(buffer);
		return 0;
	}
	fwrite(buffer, 1, strlen(buffer), inifile);
	free(buffer);
	return 0;
}

int __attribute__((visibility("hidden"))) SelfKiller()
{
	fflush(stderr);    fflush(stdout);    //flush buffers before exit.

	if(UnixSystemUtils::KillProcess(getpid()) == EXIT_FAILURE) {
		return 0;
	} else {
		return 1;
	}
}
