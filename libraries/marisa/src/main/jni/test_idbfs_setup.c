#include <stdio.h>
#include <emscripten.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

const char* SECRET = "no more secrets";

void EMSCRIPTEN_KEEPALIVE success()
{
    printf ("success() called\n");
}

void EMSCRIPTEN_KEEPALIVE setupTest() {

    int fd;

    struct stat st;

    // stat() returns -1 on error - this generally means the file
    // doesn't exist. 
    if (stat("/working1/empty.txt", &st) != 0) { 
        if (errno != ENOENT) {
            printf("Unexpected error running stat() on empty.txt: %s\n", strerror(errno));
        }
    } else {
        printf("stat() on empty.txt came back with 0\n");
    }

    fd = open("/working1/empty.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        // Couldn't open the file for read/write/create. Something is
        // horribly wrong.
        printf("Couldn't create the empty.txt file: %s\n", strerror(errno));
    } else {
        printf("empty.txt created!\n");

        if (close(fd) != 0) {
            printf("Couldn't delete the empty.txt file: %s\n", strerror(errno));
        } else {
            printf("empty.txt closed!\n");
        }
    }

    // a file whose contents are just 'az'
    if (stat("/working1/waka.txt", &st) == 0) { 
        // success! 
        printf("stat(waka.txt) came back with 0!\n");
    } else {
        printf("Error stat('waka.txt')\n");
        if (errno != ENOENT) {
            printf("Unexpected error running stat() on empty.txt: %s\n", strerror(errno));
        }
    }

    fd = open("/working1/waka.txt", O_RDWR | O_CREAT, 0666);
    if (fd != -1) {
        printf("Created waka.txt properly.\n");
        // Success!
        if (write(fd, "az", 2) == -1) {
            printf("Error writing 'az' into waka.txt: %s\n", strerror(errno));
        }
        if (close(fd) != 0) {
            printf("Error closing waka.txt: %s\n", strerror(errno));
        }
    } else {
        printf("Unable to create waka.txt: %s\n", strerror(errno));
    }

    // a file whose contents are random-ish string set by the test_browser.py file
    if (stat("/working1/moar.txt", &st) == 0) { 
        printf("moar.txt stat()==0!\n");

        // success!
        if (unlink("/working1/moar.txt") == 0) {
            printf("moar.txt deleted!\n");
        } else {
            printf("Error deleting moar.txt\n");
        }
    } else {
        if (errno != ENOENT) {
            printf("Error stat() on moar.txt: %s\n", strerror(errno));
        }
    }
    fd = open("/working1/moar.txt", O_RDWR | O_CREAT, 0666);
    if (fd != -1) {
        printf("Successfully created moar.txt!\n");
        // success ! 
        if (write(fd, SECRET, strlen(SECRET)) != strlen(SECRET)) {
            printf("Error writing SECRET into moar.txt: %s\n", strerror(errno));
        }
        if (close(fd) != 0) {
            printf("Error closing moar.txt: %s\n", strerror(errno));
        }
    } else {
        // Error creating moar.txt
        printf("Error creating moar.txt: %s\n", strerror(errno));
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
                ccall('setupTest', 'v');
                });
          );

    emscripten_exit_with_live_runtime();

    return 0;
}
