# TINYRadio: Experimental Branch<br><br>
## Upgrade to Arduino Core 3.1.1 / ESP IDF 5.3.0<br><br>

### These Changes were made to upgrade to more recent versions:

### Change 1: Switch to current version of schreibfaul1/ESP32-audioI2S

File platformio:  
	;https://github.com/schreibfaul1/ESP32-audioI2S.git#d9a2421  
	https://github.com/schreibfaul1/ESP32-audioI2S.git

Result: error: 'm_i2s_chan_cfg' was not declared in this scope (and many more similar errors)  
Reason: m_i2s_chan_cfg and other variables are not included anymore in current versions of ESP-IDF  
Solution: change 2

### Change 2: Switch to current ESP-IDF / Arduino platform

File platformio.ini:  
;platform = espressif32@6.6.0  
platform = espressif32

Result: include/defines.h:17:4: error: #error This code is intended to run only on the ESP8266/ESP32 boards ! Please check your Tools->Board setting.  
Reason: unknown  
Solution: Change 3

### Change 3: Correct expression for error condition

File /include/defines.h:  
/* SparkOfCode: Line 16 replaced due to error with current ESP IDF, first line returns true for some reason.  
//#if !( ESP8266 || ESP32)  
#if !( defined(ESP8266) || defined(ESP32) )  

Result: .pio/libdeps/WT32SC01PLUS/ESP Async WebServer/src/WebAuthentication.cpp:81:5: error: 'mbedtls_md5_starts_ret' was not declared in this scope; did you mean 'mbedtls_md5_starts'?  
Reason: changed interface of Arduino Core 3.x  
Workaround: Change 4

### Change 4: Switch to improved version of ESP Async WebServer by me-no-dev
File platformio:  
;https://github.com/khoih-prog/ESPAsyncWebServer  
https://github.com/me-no-dev/ESPAsyncWebServer

Result: src/main.cpp:240:45: error: invalid conversion from 'const AsyncWebParameter*' to 'AsyncWebParameter*' [-fpermissive]  
Reason: unknown  
Solution: Change 5

### Change 5: add "const" to declaration
File main.cpp:240  
**const** AsyncWebParameter* p = request->getParam(i);

Result: Compiles without errors, but link error:  
C:\Projects\TINYRadio9-main/lib/ESPAsync_WiFiManager_Lite/src/ESPAsync_WiFiManager_Lite.h:715:(.text._ZN25ESPAsync_WiFiManager_Lite5beginEPKc[_ZN25ESPAsync_WiFiManager_Lite5beginEPKc]+0x119): undefined reference to `getChipID()'  
Reason: unknown  
Workaround: Change 6

### Change 6: replace Chip ID by fixed string (seems to work but side effects are unknown!)
Line 715f:  
//        String _hostname = "ESP_" + String(ESP_getChipId(), HEX);  
        String _hostname = "ESP32S3";  
Line 3047f:  
//        String chipID = String(ESP_getChipId(), HEX);  
        String chipID = "ESP32S3";

### Result: Successful build.

