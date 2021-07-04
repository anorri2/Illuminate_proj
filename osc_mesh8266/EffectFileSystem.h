#ifndef EFFECTFILESYSTEM_H
#define EFFECTFILESYSTEM_H

#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>

/* You only need to format LITTLEFS the first time you run a
   test or else use the LITTLEFS plugin to create a partition
   https://github.com/lorol/arduino-esp32littlefs-plugin */

#define FORMAT_LITTLEFS_IF_FAILED true

int getFilesInDirectory(String* file_array, String directory) {
  //  char* dir = directory.c_str();
  //String* file_list;
  Serial.printf("Listing directory: %s\r\n", directory.c_str());

  File root = LittleFS.open(directory.c_str());
  if (!root) {
    Serial.println("- failed to open directory");
    return 0;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return 0;
  }

  File file = root.openNextFile();
  int file_counter = 0;
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());

    } else {
      String fname = file.name();
      file_array[file_counter] = fname;
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }

    file = root.openNextFile();
    file_counter++;
  }
  return file_counter;
}

int getNumFilesInDirectory(String directory) {

  String* file_list;
  Serial.printf("Listing directory: %s\r\n", directory.c_str());

  File root = LittleFS.open(directory);
  if (!root) {
    Serial.println("- failed to open directory");
    return 0;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return 0;
  }

  File file = root.openNextFile();
  int file_counter = 0;
  while (file) {
    file = root.openNextFile();
    file_counter++;
  }
  return file_counter;
}


void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

//char* getFileContents(fs::FS &fs,String filename){
//  Serial.printf("Reading file: %s\r\n", filename.c_str());
//
//  File file = fs.open(filename.c_str());
//  if (!file || file.isDirectory()) {
//    Serial.println("- failed to open file for reading");
//    return NULL;
//  }
//
//  Serial.println("- read from file:");
//    char buff[1000]; buff[999] = 0;
//  while (!feof(&file)) {
//    fgets(buff, 63, file);
//    Serial.printf("%s\n", buff);
//  }
////  while (file.available()) {
////    Serial.write(file.read());
////  }
//
//  file.close();
//  return buff;
//}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("- file renamed");
  } else {
    Serial.println("- rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}

// eq_spiFFS-like write and delete file, better use #define CONFIG_LittleFS_eq_spiFFS_COMPAT 1

void writeFile2(fs::FS &fs, const char * path, const char * message) {
  if (!fs.exists(path)) {
    if (strchr(path, '/')) {
      Serial.printf("Create missing folders of: %s\r\n", path);
      char *pathStr = strdup(path);
      if (pathStr) {
        char *ptr = strchr(pathStr, '/');
        while (ptr) {
          *ptr = 0;
          fs.mkdir(pathStr);
          *ptr = '/';
          ptr = strchr(ptr + 1, '/');
        }
      }
      free(pathStr);
    }
  }

  Serial.printf("Writing file to: %s\r\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void deleteFile2(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file and empty folders on path: %s\r\n", path);

  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }

  char *pathStr = strdup(path);
  if (pathStr) {
    char *ptr = strrchr(pathStr, '/');
    if (ptr) {
      Serial.printf("Removing all empty folders on path: %s\r\n", path);
    }
    while (ptr) {
      *ptr = 0;
      fs.rmdir(pathStr);
      ptr = strrchr(pathStr, '/');
    }
    free(pathStr);
  }
}

void testFileIO(fs::FS &fs, const char * path) {
  Serial.printf("Testing file I/O with %s\r\n", path);

  static uint8_t buf[512];
  size_t len = 0;
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }

  size_t i;
  Serial.print("- writing" );
  uint32_t start = millis();
  for (i = 0; i < 2048; i++) {
    if ((i & 0x001F) == 0x001F) {
      Serial.print(".");
    }
    file.write(buf, 512);
  }
  Serial.println("");
  uint32_t end = millis() - start;
  Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
  file.close();

  file = fs.open(path);
  start = millis();
  end = start;
  i = 0;
  if (file && !file.isDirectory()) {
    len = file.size();
    size_t flen = len;
    start = millis();
    Serial.print("- reading" );
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      if ((i++ & 0x001F) == 0x001F) {
        Serial.print(".");
      }
      len -= toRead;
    }
    Serial.println("");
    end = millis() - start;
    Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
    file.close();
  } else {
    Serial.println("- failed to open file for reading");
  }
}

void initFileSystem() {

  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  //    Serial.println( "eq_spiFFS-like write file to new path and delete it w/folders" );
  //    writeFile2(LITTLEFS, "/new1/new2/new3/hello3.txt", "Hello3");
  //    listDir(LITTLEFS, "/", 3);
  //    deleteFile2(LITTLEFS, "/new1/new2/new3/hello3.txt");
  //
  //    listDir(LITTLEFS, "/data", 3);
  //  createDir(LITTLEFS, "/mydir");
  //  writeFile(LITTLEFS, "/mydir/hello2.txt", "Hello2");
  //  listDir(LITTLEFS, "/", 1);
  //  deleteFile(LITTLEFS, "/mydir/hello2.txt");
  //  removeDir(LITTLEFS, "/mydir");
  //  listDir(LITTLEFS, "/", 1);
  //    writeFile(LITTLEFS, "/hello.txt", "Hello ");
  //    appendFile(LITTLEFS, "/hello.txt", "World!\r\n");
  //    readFile(LITTLEFS, "/hello.txt");
  //    renameFile(LITTLEFS, "/hello.txt", "/foo.txt");
  //    readFile(LITTLEFS, "/foo.txt");
  //    deleteFile(LITTLEFS, "/foo.txt");
  //    testFileIO(LITTLEFS, "/test.txt");
  //    deleteFile(LITTLEFS, "/test.txt");
  //
  //    Serial.println( "Test complete" );

  //  Serial.printf("ENTER COMMAND\r\n M=makedir w=writefile L=list D=delete r=read\r\n");
}

void manageFileSystem() {
  while (!Serial.available());
  if (Serial.available() > 0) {

    char comm = Serial.read();
    Serial.println("Enter file or directory");
    while (!Serial.available());
    String name = Serial.readString();

    switch (comm) {
      case 'm': {
          createDir(LittleFS, name.c_str());
          break;
        }
      case 'w': {
          Serial.println("Enter text to write");
          while (!Serial.available());
          String input = Serial.readString();
          writeFile(LittleFS, name.c_str(), input.c_str());
          break;
        }
      case 'l': {
          Serial.println("Enter num levels");
          while (!Serial.available());
          int i = Serial.parseInt();
          listDir(LittleFS, name.c_str(), i);
          break;
        }
      case 'd': {
          deleteFile(LittleFS, name.c_str());
          break;
        }
      case 'r': {
          readFile(LittleFS, name.c_str());
          break;
        }
    }
  }
  delay(2000);
}
int checkForFile(String filename, String directory) {
  String filestrs[64];
  String filepath = directory + filename + ".json";
  int num_files = getFilesInDirectory(filestrs, directory);
  if (num_files <= 0) {
    return -1;
  }
  else {
    for (int i = 0; i < num_files; i++) {
      Serial.print("Comparing ");
      Serial.print(filepath);
      Serial.print(" : ");
      Serial.println(filestrs[i]);
      if (strcmp(filestrs[i].c_str(), filepath.c_str())==0) {
        Serial.println("Match Found");
        return 1;
      }
    }
    return 0;
  }
}
#endif
