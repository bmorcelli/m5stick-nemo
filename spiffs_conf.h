#include <SPIFFS.h>
#define MAX_SPIFFS_VAR 6

// File name
const char *filename = "/nemo.conf";

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
    configFile.println("1,15,100,0,11,1,\n");
    #else
    configFile.println("3,15,100,0,11,1,\n");      
    #endif
    configFile.close();
    Serial.println("file ./nemo.conf created with default config");
    return true;
  } 
  else { return true; }
}

void verifySpiffsPartition() {
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
  if (SPIFFS.exists(filename)) {
    if (SPIFFS.remove(filename)) {      
      Serial.println("File deleted successfully");
      checkOrCreateConfigFile();
    } else {
      Serial.println("Error deleting file");
    }
  } else {
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
  Serial.println(line);
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
  File configFile = SPIFFS.open(filename, FILE_WRITE);
  if (!configFile) {
    Serial.println("Failed to open configuration file for writing");
    return;
  }
  // Open the file and move the pointer to the beginning of the desired variable
  String line = configFile.readStringUntil('\n');
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
  for(i=0; i<MAX_SPIFFS_VAR; i++) {
    if (i==index) { line += value; }
    else { line += var[i]; }
    line += ",";
  }
  line+="\n";
  // Write the new variable to the file
  configFile.print(line);

  configFile.close();
}

