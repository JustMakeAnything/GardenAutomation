# Hydroponics Controller


Wemos D1 mini

``` pinout
                      |_|_|_|_|_|_|_|_|_|
                  # RST        |01       TX #   XXX out
EC sensor         # A0         |03       RX #   XXX in
                  # D0 Wake  16|05 A SCL D1 #   Heizpumpe   I2C
EC Power          # D5 All S 14|04 A SDA D2 #   Pufferpumpe I2C
airpumpe (1)      # D6 All P 12|00   Out D3 #   Flash
Dallas Temp       # D7 All I 13|02   Out D4 #   XXX boot
XXX boot          # D8 All " 15|        GND #
                  # 3.3V       |         5V #
```

```
D1(05)-OK (I2C-SCL)
D2(04)-OK (I2C-SDA)
D3(00)-OK wird für flashen benutzt
D4(02)-OK high beim booten
D5(14)-OK
D6(12)-OK
D7(13)-OK
D8(15)-Pull down out = ok wird beim booten benutzt
```
Effectively the ESP8266 has 1 analog and 5 usable digital io pins. GPIO02 and GPIO15 could be used with restrictions. GPIO16 when no sleep mode is used.