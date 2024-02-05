# TEMP_CONTROL
   This product is about cooling the vaccines with a battery powered electrical device
   
## TFT Display
   Do the 
   ![TFT display doc image](https://github.com/sarath522/phloton/blob/master/1676460248817.JPEG)
   1. below 4 degree Celcius show in Green colour
   2. Above 4 degree Celcius show in Red Colour
   3. Battery when below 20% show in Yellow colour
   4. Ambient Temperature in orange colour 
   5. show top and bottom temperature value in display
   6. battery bar and bluetooth symbol show in top right corner
   7. Only the Avg temp (Middle & Bottom of the Chamber temp) & Amb temp will be 
      shown in the display
### process for TFT display
     so here we are used ESP32 wrover kit and 2.0 TFT display with ILI9225 driver used
     
     Connections
      define TFT_RST 26  // IO 26
      define TFT_RS  25  // IO 25
      define TFT_CLK 14  // HSPI-SCK
      define TFT_SDI 13  // HSPI-MOSI
      define TFT_CS  15  // HSPI-SS0
      define TFT_LED 0   
      
     Library used SPI, TFT_22_ILI9225 and FreeSans24pt7b
      SPI - we are communicating through spi so we are using spi protocol.
      TFT_22_ILI9225 - TFT display contains ili9225 driver so dwld and used this header file
      FreeSans24pt7b - In ili9225 driver header contains only Two font(Terminal12x16,Terminal6x8) so we are added extra header to used bigger font.
      
   
## Temperature sensor
   Take Middle & Bottom of the Chamber temp and average it, display in tft and display the ambient temp value
## PWM
    Pulse Width Modulation is used to control the constant current circuit 
## BMS
   1. show battery value in bar in display 
   2. 75-100 = full bar
   3. 50-75 = three bar
   4. 25-50 = two bar
   5. 7-25 = one bar
   6. below 7 show battery low and off display  
## RTC
   show the date and time in top left corner
## BLE
   Need to transfer the voltage, current, battery level, Temp Data (Avg & Atm Temp)
## SD Card
 - Every one Minute Avg Temp data to be stored in internal memory (SD card)
 * Avg temp & Timing data to be stored in SD card at lid opening & closing Condition 
  
 2 - 7 °C – Blue Led
 
 Above 7 °C – Red Led
 
 
