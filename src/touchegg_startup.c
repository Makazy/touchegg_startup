#include "touchegg_startup.h"



int main(int argc, char const *argv[])
{
    /**
     *  Init
     */
    
    // Configuration directory path
    char *configDir = getConfigDirectory();

    /**
     * Starting up touchegg
     */
    startup(configDir);

    /**
     * Monitoring configureation changes
     */
    monitoring(configDir, configDir);

    /**
     * Cleaning
     */
    free(configDir);

    return 0;
}

void startup(const char *logdir) {
    printf("Touchegg startup...\n");

    // Create log directory if needed
    if(createDirectoryIfdoesnotExist(logdir)) {
        perror("Error while creating log directory");
    }
    printf("Log directory at '%s'.\n", logdir);


    // Kill curently running instance
    if (isProcessRuninng(touchegg))
    {
        system("kill `pidof touchegg` >/dev/null 2>&1");
    }

    // Starting touchegg
    int cmdlength = strlen(logdir)*2+strlen("touchegg 1>stdout.log 2>stderr.log &")+1;
    char *command = malloc(cmdlength);
    snprintf(command, cmdlength, "%s%s%s%s%s", "touchegg 1>", logdir, "stdout.log 2>", logdir, "stderr.log &");

    printf("executing command '%s'.\n", command);
    system(command);

    free(command);
    printf("Done.\n");
}

void monitoring(const char *configDir, const char *logDir) {
    // Config file path
    char *configPath = malloc(strlen(configDir) + strlen(TOUCHEGG_CONF_PATH) + 2); // Not forgeting additionnal '/' size
    strcpy(configPath, configDir);
    strcat(configPath, TOUCHEGG_CONF_PATH);
    printf("Config file at '%s'.\n", configPath);
    // Last config change
    struct stat st = {0};
    stat(configDir, &st);
    time_t last_config_time = st.st_mtime;

    // Counter use to reduce check amount when config is not actively edited
    int idle_counter = 60;
    while (1)
    {
        if (idle_counter >= idle_time) {
            sleep(6);
        } else {
            sleep(1);
            idle_counter++;
        }

        // Check for changes
        struct stat st = {0};
        stat(configDir, &st);
        if (st.st_mtime != last_config_time) { // If modification time is different then change happened
            // On changes restart touchegg
            startup(logDir);
            printf("Detected configuration changes, restart of touchegg.\n");
            last_config_time = st.st_mtime;
            idle_counter = 0;
        }
    }
}

char* getHomeDirectory() {
    char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return homedir;
}

char* getConfigDirectory() {
    const char *homedir = getHomeDirectory();
    char *logdir = malloc(1024);
    snprintf(logdir, 1024, "%s%s%s", homedir, "/", TOUCHEGG_DIR_PATH);
    return logdir;
}

int createDirectoryIfdoesnotExist(const char *path) {
    struct stat st = {0};

    if (stat(path, &st) == -1) {
        printf("creating directory %s.\n", path);
        return mkdir(path, TOUCHEGG_DIR_PERMISSIONS);
    }
    return 0;
}

int isProcessRuninng(const char *processName) {
    char *command = malloc(strlen("pidof  >/dev/null 2>&1") + strlen(processName) + 1);
    strcpy(command, "pidof ");
    strcat(command, processName);
    strcat(command, " >/dev/null 2>&1");
    int result = system(command);

    if (result < 0) {
        perror("Error while trying to check if the process is running.");
        return -1;
    } else {
        printf("%d process %s running.\n", result+1%2, processName);
        return !result;
    }
}
