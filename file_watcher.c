#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WATCH_DIR "/data/local/tmp/"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1))

// Recursive deletion function
void recursive_delete(const char *path) {
    struct stat path_stat;
    if (lstat(path, &path_stat) == -1) return; // Skip if cannot stat

    if (S_ISDIR(path_stat.st_mode)) {
        // Directory: recurse into it
        DIR *dir = opendir(path);
        if (!dir) return;

        struct dirent *entry;
        char full_entry_path[PATH_MAX];
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            snprintf(full_entry_path, sizeof(full_entry_path), "%s/%s", path, entry->d_name);
            recursive_delete(full_entry_path);
        }
        closedir(dir);
        rmdir(path); // Remove the now-empty directory
    } else {
        // File or symlink: unlink
        unlink(path);
    }
}

// Function to handle new file/folder/symlink creation
void handle_new_item(const char *name) {
    if (strcmp(name, "ccminer") == 0) return; // Skip ccminer

    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s%s", WATCH_DIR, name);
    recursive_delete(fullpath); // Delete recursively
}

// Function to clean existing contents on startup
void clean_existing_contents() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(WATCH_DIR);
    if (!dir) exit(EXIT_FAILURE); // Exit silently if can't open

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        handle_new_item(entry->d_name); // Apply deletion logic
    }

    closedir(dir);
}

int main() {
    int inotify_fd, watch_fd;
    char buf[BUF_LEN];
    ssize_t num_read;
    struct inotify_event *event;

    // Initial cleanup
    clean_existing_contents();

    // Set up inotify
    inotify_fd = inotify_init1(IN_NONBLOCK);
    if (inotify_fd == -1) exit(EXIT_FAILURE); // Silent fail

    watch_fd = inotify_add_watch(inotify_fd, WATCH_DIR, IN_CREATE | IN_MOVED_TO);
    if (watch_fd == -1) exit(EXIT_FAILURE); // Silent fail

    // Event loop
    while (1) {
        num_read = read(inotify_fd, buf, BUF_LEN);
        if (num_read > 0) {
            for (char *ptr = buf; ptr < buf + num_read; ptr += EVENT_SIZE + ((struct inotify_event *) ptr)->len) {
                event = (struct inotify_event *) ptr;
                if (event->len && (event->mask & (IN_CREATE | IN_MOVED_TO))) {
                    handle_new_item(event->name);
                }
            }
        }
        usleep(50000); // 50ms polling
    }

    close(inotify_fd);
    return 0;
}
