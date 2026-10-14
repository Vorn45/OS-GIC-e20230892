#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

int main() {
    char buffer[512];

    DIR *dir = opendir(".");
    if (dir == NULL) {
        const char *err = "Error opening directory\n";
        write(2, err, strlen(err));
        return 1;
    }

    const char *header = "Filename                       Size (bytes)\n------------------------------ ----------\n";
    write(1, header, strlen(header));

    struct dirent *entry;
    struct stat fileStat;

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &fileStat) == 0) {
            int len = snprintf(buffer, sizeof(buffer), "%-30s %10ld\n", entry->d_name, (long)fileStat.st_size);
            if (len > 0) {
                write(1, buffer, len);
            }
        }
    }

    closedir(dir);
    return 0;
}
