# WFMU RP2040 Pimoroni Internet Radio

this is the Arduino code that powers my home internet radio which streams WFMU

BOM:
- [raspberry pi pico w](https://shop.pimoroni.com/products/raspberry-pi-pico-w) - the main board
- [pimoroni pico audio pack](https://shop.pimoroni.com/products/pico-audio-pack) - i2s audio board

software / library requirements:
- arduino ide
- [arduino-pico](https://arduino-pico.readthedocs.io/en/latest/) -- see the [installation instructions](https://learn.adafruit.com/rp2040-arduino-with-the-earlephilhower-core/installing-the-earlephilhower-core) or [here](https://arduino-pico.readthedocs.io/en/latest/install.html#installing-via-arduino-boards-manager)
- [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio/) -- despite its name (i.e. esp8266, not rp2040), most of the audio/streaming code comes from this, with modifications/hacky overrides in this repo
