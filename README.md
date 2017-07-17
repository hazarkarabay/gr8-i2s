# gr8-i2s
Repository for enabling I2S on a NTC GR8. ~~Working with issues noted below.~~ Probably is not working again, refer to #1, PR's welcome.

## Steps to play something on a PCM5102A I2S DAC

[![A video!](https://img.youtube.com/vi/JWs2peTOwLI/0.jpg)](https://www.youtube.com/watch?v=JWs2peTOwLI)

Note: This is how I do it. It is likely a good example of how to *not* do things. If you have a better way, send a pull request. 
 
* Get a Linux machine, we're compiling a kernel. I used Ubuntu on a VM. If you go with VM route, allocate a healthy amount of CPU and RAM to your VM (4 core/16G is what I used).
* I'm using [GadgetOS](https://github.com/NextThingCo/gadget-buildroot) from NTC. Follow their instructions until the`Make gadgetos` step. Do not execute the `scripts/build-gadget make -s` .
* You must have a `gadget-buildroot` directory from previous step. Find where is it located. On next steps, replace `[gadget-buildroot]`  with your real, absolute path. (Example: `/home/USERNAME/gadget-buildroot`)
* Create a folder named `temp` in your `[gadget-buildroot]`
* Copy following files from this repo on it:
	* ntc-gr8-crumb-modded.dts
	* Kconfig
	* Makefile
	* https://github.com/torvalds/linux/blob/master/sound/soc/codecs/pcm5102a.c
* On a shell, cd to your  `[gadget-buildroot]` dir and run these:
	* Run `scripts/build-gadget make nconfig`
		* Make the following changes:
			* Browse to `Kernel -> Device Tree Source file name`
				* Change it from `ntc-gr8-crumb` to `ntc-gr8-crumb-modded`
			* Browse to `Target packages -> Audio and video`
				* Select following in `alsa-utils`
					* `alsaconf`
					* `alsactl`
					* `alsamixer`
					* `amixer`
					* `aplay/arecord`
					* `speaker-test`
				* Select `mplayer`
		* Press `F6` then `Enter` then `F9` for saving and exiting.
	* Run `scripts/build-gadget make linux-nconfig`
		* Press  `F9` to exit. Yes, don't change anything and exit. We triggered an extraction of required files because we're going to modify them.
	* Run `sudo docker run --rm --env BR2_DL_DIR=/opt/dlcache/ --volume=gadget-build-dlcache:/opt/dlcache/ --volume=gadget-build-output:/opt/output --volume=[gadget-buildroot]/output/images:/opt/output/images --volume=[gadget-buildroot]/:/opt/gadget-os-proto --name=gadget-build-task -w="/opt/gadget-os-proto" -it gadget-build-container`
		* Run these:
			* `cp /opt/gadget-os-proto/temp/ntc-gr8-crumb-modded.dts /opt/output/build/linux-00f3979422c26ed1e25b11307c982cff5476e172/arch/arm/boot/dts`
			* `cp /opt/gadget-os-proto/temp/pcm5102a.c /opt/output/build/linux-00f3979422c26ed1e25b11307c982cff5476e172/sound/soc/codecs/`
			* `cp /opt/gadget-os-proto/temp/Makefile /opt/output/build/linux-00f3979422c26ed1e25b11307c982cff5476e172/sound/soc/codecs/`
			* `cp /opt/gadget-os-proto/temp/Kconfig /opt/output/build/linux-00f3979422c26ed1e25b11307c982cff5476e172/sound/soc/codecs/`
			* `cp /opt/gadget-os-proto/temp/sun4i-i2s.c /opt/output/build/linux-00f3979422c26ed1e25b11307c982cff5476e172/sound/soc/sunxi/`
			* `exit`
	* Run `scripts/build-gadget make linux-nconfig` again.
		* Make the following changes:
			*  Browse to `Device drivers -> Sound card support -> Advanced Linux Sound Architecture -> ALSA for SoC Audio`
				* In `Allwinner SoC Audio support`, select `Allwinner A10 I2S Support`
				* In `CODEC drivers`, select `TI PCM5102A DAC`
			* Press `F6` then `Enter` then `F9` for saving and exiting.
	* Run `scripts/build-gadget make -s`
		* Get a cold one or equivalent while waiting.
* Flash the resulting image on your CHIP Pro
* Log in to your freshly flashed CHIP
* Connect it to Internet
	* You can follow https://docs.getchip.com/chip_pro_devkit.html#wifi-setup-buildroot
* Run these:
	* `wget https://github.com/hazarkarabay/gr8-i2s/raw/master/overlays/i2s-5102a.dtbo -O /root/i2s-5102a.dtbo`
	* `depmod`
	* `modprobe snd-soc-pcm5102a`
	* `rm -rf /sys/kernel/config/device-tree/overlays/i2s*`
	* `mkdir -p /sys/kernel/config/device-tree/overlays/i2s-5102a`
	* `dmesg -C`
	* `cat /root/i2s-5102a.dtbo > /sys/kernel/config/device-tree/overlays/i2s-5102a/dtbo`
	* `dmesg`
* If you're fast, you'll see a `i2s mapping ok` message after you run the last `dmesg`. This means you are successful and your CHIP Pro can now use your I2S DAC as a sound card.
* Verify with `aplay -l`. You'll see your DAC in there.
* Make some noise:
	* `speaker-test -c 2 -r 44100 -D hw:1,0` (yes I was serious)
	* `mplayer -ao alsa:device=hw=1.0 http://someaudiofile/ontheinternet.flac`
* Enjoy.


### Current issues:
* Can't get 24 bit output even using patched sunxi-i2s driver. (`Format s24le is not supported by hardware, trying default.`)



#### Proved to be a non-issue 
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
  
[![ghit.me](https://ghit.me/badge.svg?repo=hazarkarabay/gr8-i2s)](https://ghit.me/repo/hazarkarabay/gr8-i2s)
