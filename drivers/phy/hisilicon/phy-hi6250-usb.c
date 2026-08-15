// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 Tildeguy (tildeguy@mainlining.org)
 */

#include <linux/mfd/syscon.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h>
#include <linux/regmap.h>
#include <linux/gpio/consumer.h>
#include <linux/clk.h>

#define PERI_CRG_RSTEN4				(0x90)
#define PERI_CRG_RSTDIS4			(0x94)
#define PERI_CRG_CLK_EN4			(0x40)
#define PERI_CRG_CLK_DIS4			(0x44)
#define BIT_RST_USB2OTG				(1<<9)
#define BIT_RST_USB2OTGPHY			(1<<12)
#define BIT_RST_USB2OTGPHYPOR			(1<<13)
#define BIT_RST_USB2OTG_ADP			(1<<27)
#define BIT_RST_USB2OTG_32K			(1<<14)
#define BIT_RST_USB2OTG_MUX			(1<<10)
#define BIT_RST_USB2OTG_AHBIF			(1<<11)
#define BIT_HCLK_USB2OTG			(1 << 1)

#define USB_AHBIF_CTRL0 0
#define USB_AHBIF_CTRL2 0x08
#define USB_AHBIF_CTRL3 0x0C

#define BIT_AHBIF_CTRL0_IDSEL (3<<4)
#define BIT_AHBIF_CTRL0_IDSEL_LOW (1<<4)
#define BIT_AHBIF_CTRL0_ID (1<<6)
#define BIT_AHBIF_CTRL0_ACAENB_SEL (1<<2)
#define BIT_AHBIF_CTRL0_ACAENB (1<<3)

#define BIT_AHBIF_CTRL2_VBUSVALIDEXT (1<<2)
#define BIT_AHBIF_CTRL2_VBUSVLDEXTSEL (1<<3)

#define BIT_PCTRL_ABB_MUX_MASK (7<<24)
#define BIT_PCTRL_ABB_MUX (5<<24)

#define PCTRL_PERI_CTRL24	0x64
#define PMU_ABB_192_OFFSET	0x43c

struct hi6250_priv {
	struct device *dev;
	enum phy_mode mode;
	struct regmap *pericrg;
	struct regmap *pctrl;
	struct regmap *ahbif;
	struct gpio_desc *mode_gpio;
	unsigned int eyepattern;
	unsigned int host_eyepattern;
};

static int hi6250_phy_power_on(struct hi6250_priv *priv)
{
	struct regmap *pericrg = priv->pericrg; /* pericrg regmap */
	struct regmap *pctrl = priv->pctrl; /* pctrl regmap */
	struct regmap *ahbif = priv->ahbif; /* usb ahbif base regmap */
	u32 val, mask;
	int ret;

	bool is_host_mode = priv->mode == PHY_MODE_USB_HOST; /* TODO: remove this stinkiness */

	/* configure abb clock with pctrl */
	mask = BIT_PCTRL_ABB_MUX_MASK;
	val = BIT_PCTRL_ABB_MUX;
	ret = regmap_update_bits(pctrl, PCTRL_PERI_CTRL24, mask, val);
	if (ret) goto out;

	/* set gpio_21[2] = is_host_mode, 0:device mode, 1:host mode */
	gpiod_set_value(priv->mode_gpio, is_host_mode);
	
	ret = regmap_write(pericrg, PERI_CRG_CLK_EN4, BIT_HCLK_USB2OTG);
	if (ret) goto out;
	
	udelay(100);

	/* unreset usb ahbif */
	ret = regmap_write(pericrg, PERI_CRG_RSTDIS4,
	  BIT_RST_USB2OTG_ADP |
	  BIT_RST_USB2OTG_32K |
	  BIT_RST_USB2OTG_MUX |
	  BIT_RST_USB2OTG_AHBIF);
	if (ret) goto out;
	udelay(100);

	val = (is_host_mode ? 0 : BIT_AHBIF_CTRL0_ID)
	      | BIT_AHBIF_CTRL0_ACAENB_SEL
				| BIT_AHBIF_CTRL0_IDSEL_LOW;
	mask = BIT_AHBIF_CTRL0_IDSEL |
	       BIT_AHBIF_CTRL0_ID |
	       BIT_AHBIF_CTRL0_ACAENB_SEL |
	       BIT_AHBIF_CTRL0_ACAENB;
	ret = regmap_update_bits(ahbif, USB_AHBIF_CTRL0, mask, val);
	if (ret) goto out;

	/* write eye pattern */
	ret = regmap_write(ahbif, USB_AHBIF_CTRL3,
	  is_host_mode ? priv->host_eyepattern : priv->eyepattern);
	if (ret) goto out;
	dev_info(priv->dev, "usb ahbif eye pattern setup\n");

	/* unreset phy */
	ret = regmap_write(pericrg, PERI_CRG_RSTDIS4, BIT_RST_USB2OTGPHYPOR);
	if (ret) goto out;

	/* delay 50us */
	udelay(50);

	/* unreset phy clk domain */
	ret = regmap_write(pericrg, PERI_CRG_RSTDIS4, BIT_RST_USB2OTGPHY);
	if (ret) goto out;

	/* delay 100us */
	udelay(100);

	/* unreset hclk domain */
	ret = regmap_write(pericrg, PERI_CRG_RSTDIS4, BIT_RST_USB2OTG);
	if (ret) goto out;

	/* enable vbusvalidext & vbusvldextsel */
	val = BIT_AHBIF_CTRL2_VBUSVALIDEXT | BIT_AHBIF_CTRL2_VBUSVLDEXTSEL;
	mask = val;
	ret = regmap_update_bits(ahbif, USB_AHBIF_CTRL2, mask, val);
	if (ret) goto out;
	
	msleep(1);

	dev_info(priv->dev, "phy power on\n");

	return 0;
out:
	dev_err(priv->dev, "failed to setup phy ret: %d\n", ret);
	return ret;
}

	
static int hi6250_phy_power_off(struct hi6250_priv *priv)
{
	struct regmap *pericrg = priv->pericrg; /* pericrg regmap */
	int ret;

	/* reset controller */
	ret = regmap_write(pericrg, PERI_CRG_RSTEN4, BIT_RST_USB2OTG);
	if (ret) goto out;
	udelay(1);

	/* reset phy */
	ret = regmap_write(pericrg, PERI_CRG_RSTEN4, BIT_RST_USB2OTGPHY);
	if (ret) goto out;
	ret = regmap_write(pericrg, PERI_CRG_RSTEN4, BIT_RST_USB2OTGPHYPOR);
	if (ret) goto out;

	/* reset usb ahbif */
	ret = regmap_write(pericrg, PERI_CRG_RSTEN4,
	                   BIT_RST_USB2OTG_ADP | BIT_RST_USB2OTG_32K | BIT_RST_USB2OTG_MUX |
	                   BIT_RST_USB2OTG_AHBIF);
	if (ret) goto out;
	ret = regmap_write(pericrg, PERI_CRG_CLK_DIS4, BIT_HCLK_USB2OTG);
	if (ret) goto out;
	
	msleep(1);

	dev_info(priv->dev, "phy power off\n");

	return 0;
out:
	dev_err(priv->dev, "failed to setup phy ret: %d\n", ret);
	return ret;
}

static void hi6250_phy_init(struct hi6250_priv *priv)
{
}

static int hi6250_phy_start(struct phy *phy)
{
	struct hi6250_priv *priv = phy_get_drvdata(phy);
	return hi6250_phy_power_on(priv);
}

static int hi6250_phy_exit(struct phy *phy)
{
	struct hi6250_priv *priv = phy_get_drvdata(phy);
	return hi6250_phy_power_off(priv);
}

static int hi6250_phy_set_mode(struct phy *phy, enum phy_mode mode, int submode)
{
	struct hi6250_priv *priv = phy_get_drvdata(phy);
	
	priv->mode = mode;
	return 0;		
}

static const struct phy_ops hi6250_phy_ops = {
	.init		= hi6250_phy_start,
	.exit		= hi6250_phy_exit,
	.set_mode = hi6250_phy_set_mode,
	.owner		= THIS_MODULE,
};

static int hi6250_phy_probe(struct platform_device *pdev)
{
	struct phy_provider *phy_provider;
	struct device *dev = &pdev->dev;
	struct phy *phy;
	struct hi6250_priv *priv;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;
	priv->pericrg = syscon_regmap_lookup_by_phandle(dev->of_node,
					"hisilicon,pericrg-syscon");
	if (IS_ERR(priv->pericrg)) {
		dev_err(dev, "no hisilicon,pericrg-syscon\n");
		return PTR_ERR(priv->pericrg);
	}
	priv->pctrl = syscon_regmap_lookup_by_phandle(dev->of_node,
					"hisilicon,pctrl-syscon");
	if (IS_ERR(priv->pctrl)) {
		dev_err(dev, "no hisilicon,pctrl-syscon\n");
		return PTR_ERR(priv->pctrl);
	}

	priv->ahbif = syscon_regmap_lookup_by_phandle(dev->of_node,
					"hisilicon,ahbif-syscon");
	if (IS_ERR(priv->ahbif)) {
		dev_err(dev, "no hisilicon,ahbif-syscon\n");
		return PTR_ERR(priv->ahbif);
	}
	
	device_property_read_u32(dev, "hisilicon,eye-pattern", &priv->eyepattern);
	device_property_read_u32(dev, "hisilicon,host-eye-pattern", &priv->host_eyepattern);
	
	priv->mode_gpio = devm_gpiod_get_optional(dev, "mode", GPIOD_OUT_LOW);
	if (IS_ERR(priv->mode_gpio))
    return PTR_ERR(priv->mode_gpio);
  
	hi6250_phy_init(priv);

	phy = devm_phy_create(dev, NULL, &hi6250_phy_ops);
	if (IS_ERR(phy))
		return PTR_ERR(phy);

	phy_set_drvdata(phy, priv);
	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	return PTR_ERR_OR_ZERO(phy_provider);
}

static const struct of_device_id hi6250_phy_of_match[] = {
	{.compatible = "hisilicon,hi6250-usb-phy",},
	{ },
};
MODULE_DEVICE_TABLE(of, hi6250_phy_of_match);

static struct platform_driver hi6250_phy_driver = {
	.probe	= hi6250_phy_probe,
	.driver = {
		.name	= "hi6250-usb-phy",
		.of_match_table	= hi6250_phy_of_match,
	}
};
module_platform_driver(hi6250_phy_driver);

MODULE_DESCRIPTION("HISILICON Hi6250 USB PHY driver");
MODULE_LICENSE("GPL");
