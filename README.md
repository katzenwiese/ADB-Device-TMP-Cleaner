# Android ADB Device TMP Cleaner (Inotify-based)

This program is a simple TMP directory cleaner for Android devices connected via ADB. It uses the `inotify` mechanism to monitor the `/data/local/tmp/` directory and recursively deletes any new files, folders, or symlinks created or moved into this directory.

## 🔍 Features
- Designed to protect Android TV boxes and other Android devices exposed to the internet via ADB from malicious attackers.
- Continuously monitors the `/data/local/tmp/` directory and deletes any unauthorized files, folders, or scripts.
- Can prevent attackers from dropping malicious binaries or scripts onto compromised or exposed devices.
- Uses `inotify` for efficient monitoring without constant polling.
- Designed to run continuously in the background.
- Monitors the `/data/local/tmp/` directory for file, directory, and symlink creation or movement.
- Automatically deletes detected items recursively.
- Uses `inotify` for efficient monitoring without constant polling.
- Designed to run continuously in the background.

## 📁 Files
- `file_watcher.c` - The source code of the cleaner.
- `tmp_watcher32` - Precompiled static binary for 32-bit ARM devices.
- `tmp_watcher64` - Precompiled static binary for 64-bit ARM devices.

## 🚀 Compilation
Compile the program for 32-bit ARM:
```sh
gcc -o tmp_watcher32 file_watcher.c -Wall
```

Compile the program for 64-bit ARM:
```sh
gcc -o tmp_watcher64 file_watcher.c -Wall
```

Compile the program using:
```sh
gcc -o file_watcher file_watcher.c -Wall
```

## 📲 Usage with ADB
### Pushing to Android Devices
```sh
adb push tmp_watcher32 /data/local/tmp/  # For 32-bit ARM devices
adb push tmp_watcher64 /data/local/tmp/  # For 64-bit ARM devices
adb shell chmod +x /data/local/tmp/tmp_watcher32
adb shell chmod +x /data/local/tmp/tmp_watcher64
```

```sh
adb push file_watcher /data/local/tmp/
adb shell chmod +x /data/local/tmp/file_watcher
adb shell /data/local/tmp/file_watcher
```

### Running in the Background
To run the cleaner as a background process:
```sh
adb shell nohup /data/local/tmp/tmp_watcher32 &  # For 32-bit ARM devices
adb shell nohup /data/local/tmp/tmp_watcher64 &  # For 64-bit ARM devices
```
Or using `&` directly:
```sh
adb shell /data/local/tmp/tmp_watcher32 &  # For 32-bit ARM devices
adb shell /data/local/tmp/tmp_watcher64 &  # For 64-bit ARM devices
```

To run the cleaner as a background process:
```sh
adb shell nohup /data/local/tmp/file_watcher &
```
Or using `&` directly:
```sh
adb shell /data/local/tmp/file_watcher &
```

### Checking if it's running
```sh
adb shell ps | grep tmp_watcher32  # For 32-bit ARM devices
adb shell ps | grep tmp_watcher64  # For 64-bit ARM devices
```

```sh
adb shell ps | grep file_watcher
```

### Stopping the process
```sh
adb shell kill -9 <PID>
```

## 📌 Adding Exceptions
By default, the program will delete everything within `/data/local/tmp/`. To **exclude** certain files or folders, you can modify the `handle_new_item()` function in the source code.

### Example Exclusion Code:
```c
// Function to handle new file/folder/symlink creation
void handle_new_item(const char *name) {
    if (strcmp(name, "example_file") == 0 || strcmp(name, "example_folder") == 0) return; // Skip specified items

    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s%s", WATCH_DIR, name);
    recursive_delete(fullpath); // Delete recursively
}
```

In this modification, the program will **skip** deleting any file or folder named `example_file` or `example_folder`. Simply add more conditions to the `if` statement to exclude other files or directories.

## 📄 License
This project is released under the MIT License.

