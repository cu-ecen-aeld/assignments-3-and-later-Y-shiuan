#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
    openlog("writer", LOG_PID|LOG_CONS, LOG_USER);

    if (argc != 3) {
        syslog(LOG_ERR, "Error: Two arguments required: <file_path> <text_string>");
        closelog();
        return 1;
    }

    char *writefile = argv[1];
    char *writestr = argv[2];
    FILE *file = fopen(writefile, "w");

    fputs(writestr, file);
    syslog(LOG_DEBUG, "Writing '%s' to '%s'", writestr, writefile);

    fclose(file);
    closelog();

    return 0;
}
