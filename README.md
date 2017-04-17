# gr8-i2s
Repository for enabling I2S on a NTC GR8. Currently not working, WiP.

### Current issues:
* Can't get 24 bit output. (`Format s24le is not supported by hardware, trying default.`)
* Signal differs from known good I2S source (DIR9001) and currently cannot produce audio on my testing. Differences are:
  * MCLK phase is inverted
  * BCLK is exactly 32fS and 16 bits of stereo data (32 bits) can only fit. But..
    * Many DAC's (actually all of I checked) requires one additional SCK clock period after LRCK edge change, before sending data.
    * Because BCLK is 32fS, our data is "packed" and in that "required" additional SCK, data line actually has the LSB from previous channel's data!
    * This "LSB sagging" is actually part of I2S specification: https://www.sparkfun.com/datasheets/BreakoutBoards/I2SBUS.pdf
    * But DAC's seemingly not support it and needs that extra SCK space.
    * Will need to change I2S BCLK to at least 64fS (pretty standart on other devices).

Related discussions:  
https://bbs.nextthing.co/t/i2s-on-the-chip-pro/15571  
https://groups.google.com/forum/#!topic/linux-sunxi/6Hl2n1L5ccs
