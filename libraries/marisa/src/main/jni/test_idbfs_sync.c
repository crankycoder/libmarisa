#include <stdio.h>
#include <emscripten.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int result = 1;


const char* SECRET = "no more secrets";

void REPORT_RESULT() {
    printf("Final result: %d\n", result);
}

void EMSCRIPTEN_KEEPALIVE success()
{
    REPORT_RESULT();
}

void EMSCRIPTEN_KEEPALIVE test() {

    int fd;

    // does the empty file exist?
    fd = open("/working1/empty.txt", O_RDONLY);
    if (fd == -1) {
        printf("Error opening empty.txt for read : %s\n", strerror(errno));
    } else {
        printf("Success! Opened empty.txt for read.\n");
        if (close(fd) != 0) {
            printf("Error closing empty.txt: %s\n", strerror(errno));
        }
    }

    if (unlink("/working1/empty.txt") != 0) {
        result = -14000 - errno;
    }

    // does the 'az' file exist, and does it contain 'az'?
    fd = open("/working1/waka.txt", O_RDONLY);
    printf("Not FIRST waka.txt Errno: %d ENOENT=%d\n", errno, ENOENT);
    if (fd == -1) {
        result = -15000 - errno;
    }
    else
    {
        char bf[4];
        int bytes_read = read(fd,&bf[0],sizeof(bf));
        if (bytes_read != 2) {
            result = -16000;
        }
        else if ((bf[0] != 'a') || (bf[1] != 'z')) {
            result = -17000;
        }
        if (close(fd) != 0) {
            result = -18000 - errno;
        }
        /*
        if (unlink("/working1/waka.txt") != 0) {
            result = -19000 - errno;
        }
        */
    }

    // does the random-ish file exist and does it contain SECRET?
    fd = open("/working1/moar.txt", O_RDONLY);
    printf("Not FIRST moar.txt Errno: %d ENOENT=%d\n", errno, ENOENT);
    if (fd == -1) {
        printf("The file moar.txt doesn't exist!\n");
        result = -20000 - errno;
    }
    else
    {
        char bf[256];
        int bytes_read = read(fd,&bf[0],sizeof(bf));
        if (bytes_read != strlen(SECRET)) {
            result = -21000;
        }
        else
        {
            bf[strlen(SECRET)] = 0;
            if (strcmp(bf,SECRET) != 0) {
                result = -22000;
            }
        }
        if (close(fd) != 0) {
            result = -23000 - errno;
        }
        /*
        if (unlink("/working1/moar.txt") != 0) {
            result = -24000 - errno;
        }
        */
    }

    // sync from memory state to persisted and then
    // run 'success'
    EM_ASM(
            FS.syncfs(function (err) {
                assert(!err);
                ccall('success', 'v');
                });
          );

}

int main() {

    EM_ASM(
            FS.mkdir('/working1');
            FS.mount(IDBFS, {}, '/working1');

            // sync from persisted state into memory and then
            // run the 'test' function
            FS.syncfs(true, function (err) {
                assert(!err);
                ccall('test', 'v');
                });
          );

    emscripten_exit_with_live_runtime();

    return 0;
}
