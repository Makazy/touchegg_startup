/**
 * Script de lancement de touchegg pour Ubuntu
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

// Permission for config directory
#define TOUCHEGG_DIR_PERMISSIONS 0775

// Config directory path relative to home
const char TOUCHEGG_DIR_PATH[] = ".config/touchegg/";
// Application name
const char touchegg[] = "touchegg";
// Config file name
const char TOUCHEGG_CONF_PATH[] = "touchegg.conf";
// Idle time to reduce config check while inactive
const int idle_time = 60;

// Start/Restart touchegg
void startup(const char*);
// Monitor modification to config and restart touchegg
void monitoring(const char*, const char*);

/**
 * Utils
 */
char* getHomeDirectory();
char* getConfigDirectory() ;
int createDirectoryIfdoesnotExist(const char *);
int isProcessRuninng(const char *);
