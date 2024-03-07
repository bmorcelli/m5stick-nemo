#include "FS.h"
#include "SPIFFS.h"
#include <iostream>
#include <sstream>
/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true
#define MAX_SPIFFS_VAR 6

// File name
const char *filename = "/nemo.conf";

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
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

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

bool deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
       return true;
    } else {
        Serial.println("- delete failed");
       return false;
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
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
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
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


// Function to check if the file exists and create it with default values if not
bool checkOrCreateConfigFile() {
  if (!SPIFFS.exists(filename)) {    
    File configFile = SPIFFS.open(filename, FILE_WRITE);
    if (!configFile) {
      Serial.println("Failed to open configuration file for writing");
      return false;
    }
    //configFile.println("rotation,dimTime,brightness,TvBG_region,FGColor,BGColor"); 
    #if defined(CARDPUTER)
    writeFile(SPIFFS, filename, "1,15,100,0,11,1,\n");
    #else
    writeFile(SPIFFS, filename, "3,15,100,0,11,1,\n");  
    #endif
    configFile.close();
    Serial.println("file ./nemo.conf created with default config");
    return true;
  } 
  else { return true; }
}

void verifySpiffsPartition() {
  listDir(SPIFFS,"/",0);
  readFile(SPIFFS, filename);
  if(!checkOrCreateConfigFile) {
    if (SPIFFS.format()) {
      Serial.println("SPIFFS formatted successfully");
      checkOrCreateConfigFile();
    } else {
      Serial.println("Error formatting SPIFFS");
    }
  } 
}

void resetConfigFile() {
    if (deleteFile(SPIFFS,filename)) {      
      Serial.println("File deleted successfully");
      checkOrCreateConfigFile();
    } else {
      Serial.println("Error deleting file");
      checkOrCreateConfigFile();
    }
}

// Function to read the content of the file and assign it to corresponding variables
uint16_t readConfigFile(int index) {
  File configFile = SPIFFS.open(filename, FILE_READ);
  if (!configFile) {
    Serial.println("Failed to open configuration file for reading");
    return 0;
  }
  String line = configFile.readStringUntil('\n');
  String tempvar;
  configFile.close();
  uint16_t var[MAX_SPIFFS_VAR];
  int i=0;
  int commaIndex;
  for(i=0; i<MAX_SPIFFS_VAR; i++) {
    commaIndex = line.indexOf(',');
    tempvar = line.substring(0, commaIndex);
    var[i]=tempvar.toInt();
    line = line.substring(commaIndex + 1);
  }
  return var[index];
}

// Function to write the variable to the configuration file
void writeVariableToFile(int index, uint16_t value) {
  File configFile = SPIFFS.open(filename, FILE_READ);
  if (!configFile) {
    Serial.println("Failed to open configuration file for reading");
    return;
  }
  // Open the file and move the pointer to the beginning of the desired variable
  String line = configFile.readStringUntil('\n');
  configFile.close();
   
  String tempvar;
  uint16_t var[MAX_SPIFFS_VAR];
  int i=0;
  int commaIndex;

  for(i=0; i<MAX_SPIFFS_VAR; i++) {
    commaIndex = line.indexOf(',');
    tempvar = line.substring(0, commaIndex);
    var[i]=tempvar.toInt();
    line = line.substring(commaIndex + 1);
  }

  line=""; // reset line

  std::ostringstream oss;
  std::string str_value;
  for(i=0; i<MAX_SPIFFS_VAR; i++) {
    if (i==index) { 
       oss << value;
       str_value = oss.str();
       line += String(str_value.c_str()); 
    }
    else { 
       oss << var[i];
       str_value = oss.str();
       line += String(str_value.c_str()); 
    }
    line += ",";
  }
  line+="\n";


  configFile = SPIFFS.open(filename, FILE_WRITE);
  if (!configFile) {
    Serial.println("Failed to open configuration file for writing");
    return;
  }
  Serial.println("Depois de sobrescrever" + line);
  // Write the new variable to the file
  configFile.println(line);

}

