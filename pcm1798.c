/*
 * Driver for the pcm1798 codec
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/soc.h>

static struct snd_soc_dai_driver pcm1798_dai = {
	.name = "pcm1798-hifi",
	.playback = {
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_192000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE
	},
};

static struct snd_soc_codec_driver soc_codec_dev_pcm1798;

static int pcm1798_probe(struct platform_device *pdev)
{
	pr_info("Hello from PCM1798 driver! 15.04.2017 03:22\n");
	return snd_soc_register_codec(&pdev->dev, &soc_codec_dev_pcm1798,
			&pcm1798_dai, 1);
}

static int pcm1798_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static const struct of_device_id pcm1798_of_match[] = {
	{ .compatible = "ti,pcm1798", },
	{ }
};
MODULE_DEVICE_TABLE(of, pcm1798_of_match);

static struct platform_driver pcm1798_codec_driver = {
	.probe		= pcm1798_probe,
	.remove		= pcm1798_remove,
	.driver		= {
		.name	= "pcm1798-codec",
		.of_match_table = pcm1798_of_match,
	},
};

module_platform_driver(pcm1798_codec_driver);

MODULE_DESCRIPTION("ASoC pcm1798 codec driver");
MODULE_AUTHOR("Hazar Karabay <hazar@hazarkarabay.com.tr>");
MODULE_LICENSE("GPL v2");
