// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 Tildeguy (tildeguy@mainlining.org)
 */

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/regmap.h>
#include <linux/gpio/consumer.h>
#include <linux/mfd/syscon.h>

struct hi6250_reboot {
	struct device *dev;
	struct regmap *pmuctrl;
	struct regmap *sysctrl;
	struct gpio_desc *powerhold_gpio;
};

/* Bootup marker for Hi6250 BL */
/* TODO: Consider moving into a new driver under hisilicon,hi6250-pmuctrl */

#define HI6250_BOOTUP_PMUCTRL_OFFSET 0x630
#define HI6250_BOOTUP_PMUCTRL_MASK   0xff
#define HI6250_BOOTUP_PMUCTRL_VALUE  250

static void hi6250_reboot_init(
  struct hi6250_reboot *hi6250_reboot)
{
	regmap_update_bits(hi6250_reboot->pmuctrl,
		HI6250_BOOTUP_PMUCTRL_OFFSET,
		HI6250_BOOTUP_PMUCTRL_MASK,
		HI6250_BOOTUP_PMUCTRL_VALUE);
}

#define HI6250_REBOOT_REASON_PMUCTRL_OFFSET		0x62c
#define HI6250_REBOOT_REASON_PMUCTRL_MASK		0xff
#define HI6250_REBOOT_REASON_AP_S_COLDBOOT	0x00
#define HI6250_REBOOT_REASON_CHARGEREBOOT	0x06
#define HI6250_REBOOT_REASON_COLDBOOT		0x10

#define HI6250_RESET_SYSCTRL_OFFSET 0x510
#define HI6250_RESET_SYSCTRL_MASK   (1<<2)
#define HI6250_RESET_SYSCTRL_VALUE   (1<<2)

static void hi6250_reset_with_reason(
  struct hi6250_reboot *hi6250_reboot,
  u8 reason
) {
	/* do some goop with gpio25 */
	gpiod_set_value_cansleep(hi6250_reboot->powerhold_gpio, 1);
	mdelay(1000);

	/* setup reboot reason in pmuctrl */
	regmap_update_bits(hi6250_reboot->pmuctrl,
	  HI6250_REBOOT_REASON_PMUCTRL_OFFSET,
	  HI6250_REBOOT_REASON_PMUCTRL_MASK,
	  reason);

	/* request reset from sysctrl */
	regmap_update_bits(hi6250_reboot->sysctrl,
	  HI6250_RESET_SYSCTRL_OFFSET,
	  HI6250_RESET_SYSCTRL_MASK,
	  HI6250_RESET_SYSCTRL_VALUE);
}

static int hi6250_restart_handler(
  struct sys_off_data *data
) {
	struct hi6250_reboot *hi6250_reboot = data->cb_data;

	dev_info(hi6250_reboot->dev, "[restart]running out of goops");

	hi6250_reset_with_reason(
		hi6250_reboot,
    HI6250_REBOOT_REASON_COLDBOOT);
	
	while (1)
		cpu_do_idle();

	return NOTIFY_DONE;
}

static int hi6250_poweroff_handler(
  struct sys_off_data *data
) {
	struct hi6250_reboot *hi6250_reboot = data->cb_data;
	
	dev_info(hi6250_reboot->dev, "[poweroff]running out of goops");

	hi6250_reset_with_reason(
		hi6250_reboot,
	  HI6250_REBOOT_REASON_COLDBOOT);
	
	while (1)
		cpu_do_idle();

	return NOTIFY_DONE;
}

static int hi6250_reboot_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct hi6250_reboot *hi6250_reboot;
	int priority = SYS_OFF_PRIO_FIRMWARE + 1;
	int ret;

	hi6250_reboot = devm_kzalloc(&pdev->dev, sizeof(*hi6250_reboot),
				GFP_KERNEL);
	if (!hi6250_reboot)
		return -ENOMEM;

	hi6250_reboot->pmuctrl = syscon_regmap_lookup_by_phandle(np,
						  "hisilicon,pmuctrl-syscon");
	if (IS_ERR(hi6250_reboot->pmuctrl))
		return dev_err_probe(&pdev->dev, PTR_ERR(hi6250_reboot->pmuctrl),
				     "failed to get pmuctrl regmap\n");

	hi6250_reboot->sysctrl = syscon_regmap_lookup_by_phandle(np,
						  "hisilicon,sysctrl-syscon");
	if (IS_ERR(hi6250_reboot->sysctrl))
		return dev_err_probe(&pdev->dev, PTR_ERR(hi6250_reboot->sysctrl),
				     "failed to get sysctrl regmap\n");

	hi6250_reboot->powerhold_gpio = devm_gpiod_get_optional(&pdev->dev, "hisilicon,powerhold",
							GPIOD_OUT_LOW);
	if (IS_ERR(hi6250_reboot->powerhold_gpio))
		return dev_err_probe(&pdev->dev, PTR_ERR(hi6250_reboot->powerhold_gpio),
				     "failed to get powerhold gpio\n");

	hi6250_reboot->dev = &pdev->dev;
	hi6250_reboot_init(hi6250_reboot);
	
	ret = devm_register_sys_off_handler(&pdev->dev,
						  SYS_OFF_MODE_POWER_OFF,
					    priority,
					    hi6250_poweroff_handler,
					    hi6250_reboot);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "cannot register poweroff handler\n");

	dev_info(hi6250_reboot->dev, "registered poweroff driver");
	
	ret = devm_register_sys_off_handler(&pdev->dev,
					    SYS_OFF_MODE_RESTART,
					    priority,
					    hi6250_restart_handler,
					    hi6250_reboot);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "cannot register restart handler\n");

	dev_info(hi6250_reboot->dev, "registered restart driver");

	return ret;
}

static const struct of_device_id hi6250_reboot_of_match[] = {
	{ .compatible = "hisilicon,hi6250-reboot" },
	{}
};
MODULE_DEVICE_TABLE(of, hi6250_reboot_of_match);

static struct platform_driver hi6250_reboot_driver = {
	.probe = hi6250_reboot_probe,
	.driver = {
		.name = "hi6250-reboot",
		.of_match_table = hi6250_reboot_of_match,
	},
};
module_platform_driver(hi6250_reboot_driver);
