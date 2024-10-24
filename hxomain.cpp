#include "config.h"
#include "backup.h"
#include "utils.h"
#include <iostream>

//Cpp function declarations...


extern "C" {
#include "hxo-user.h"                            //For hxo API definitions
#include <string.h>
size_t _init_hxo(struct HXOParam *hxoParameter)  //HXO entrypoint
{
    //Show template
    fprintf(stdout, BANNER_STR, (VER_CODE >> 8) & 0xFF, (VER_CODE >> 16) & 0xFF, (VER_CODE >> 24) & 0xFF);

    //Check for the hxo version
    if (hxo_cmpversion(hxoParameter->hxo_version, "1.2.0") == -1) {
        fprintf(stderr, "ERROR: HXO-loader version 1.2.0+ needed!\n"); 
        return -1;
    }


    char *APP_ID = (char*) strdup(hxoParameter->baseName);
    char *modulePath = (char*) strdup(hxoParameter->modulePath);
    
    int STAT_VAL = initBackup(APP_ID, modulePath);

    free(APP_ID);    free(modulePath);
    return STAT_VAL;         //Successful Initialization :)
}
}

//Rest of the C++ code
