# TINYRadio: Internet Radio for ESC32-Board WT32-SC01-PLUS<br><br>
## Author/Copyright: DrNeurosurg - this is a fork to apply some changes.<br><br>

## ![Screenshot](/images/screen.png)

## ![Screenshot](/images/Screen2.jpg)

## Hardware
**PLEASE NOTE:** TINYRadio will only run on WT32-SC01-PLUS, not on WT32-SC01!
The board contains a mono decoder and amplifier.
For details and external connectors see [WT32-SC01-PLUS Datasheet](https://www.marutsu.co.jp/contents/shop/marutsu/datasheet/khe_WT32-SC01-PLUS.pdf?srsltid=AfmBOoohYPBXUQcbwF3fNsmzJ99yOVGr-nubaThOd_DJB-lqxFCud59W) and [WT32-S3-WROVER](https://en.wireless-tag.com/product-item-17.html).

You will need one 2-pin JST-connector (1.25mm) for the speaker, another one to enable the flashing mode (see below). I found the following set very helpful, especially when I needed more external connectors:
[Connector/Cable Set](https://www.amazon.de/Steckverbinder-vorgecrimpte-kompatibel-PicoBlade-Silikonkabel/dp/B07S18D3RN)

## Software
- Install Visual Studio Code, Platformio and the Espressif IDF extension - there are several online tutorials.
- Download repository as zip file and extract to a project folder
- Open project folder in platformio

## Configuration
The default configuration works with the minimal hardware: just the board and a speaker.
I will add information about stereo and external volume control or station selector later.
- Connect external speaker to the corresponding connector
- Connect pin 6 of the debug connector to GND - on the debug connector, these are pins 6 and 7. **Make sure not to short-circuit the wrong side of the connector, this may destroy your board!!** Pins 6 and 7 are the two pins directly above the reset button:
![Debug Port](images/DebugPort.jpg)
- Press reset to enable flash mode
- Build and upload
- Disconnect pin 6 from GND and power cycle the board
- Enjoy!
