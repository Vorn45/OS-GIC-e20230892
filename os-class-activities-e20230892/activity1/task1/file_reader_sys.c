#include <fcntl.h>
#include <unistd.h>

int main() {
    char buffer[256];
    ssize_t bytesRead;

    // 1. Open "output.txt" for reading using open()
    int fd = open("output.txt", O_RDONLY);
    if (fd < 0) {
        const char *err = "Error reading file\n";
        write(2, err, 19);
        return 1;
    }

    // 2 & 3. Read into buffer and write to stdout (fd 1)
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytesRead);
    }

    // 4. Close the file using close()
    close(fd);

    return 0;
}
