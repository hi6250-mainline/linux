// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Hi6250 stub clock driver
 *
 * Copyright (C) 2025, Tildeguy (tildeguy@mainlining.org)
 */

#include <linux/clk-provider.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mailbox_client.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <dt-bindings/clock/hi6250-clock.h>

#define HI6250_STUB_CLOCK_BASE    (0x41C)

#define DEFINE_CLK_STUB(_id, _freqs, _cmd, _name) \
	{                                         \
		.id = (_id),                            \
		.freqs = (_freqs),                      \
		.cmd = (_cmd),                          \
		.hw.init = &(struct clk_init_data) {    \
			.name = #_name,                       \
			.ops = &hi6250_stub_clk_ops,          \
			.num_parents = 0,                     \
			.flags = CLK_GET_RATE_NOCACHE,        \
		},                                      \
	}

#define to_stub_clk(_hw) container_of(_hw, struct hi6250_stub_clk, hw)

struct hi6250_stub_clk_chan {
	struct mbox_client cl;
	struct mbox_chan *mbox;
};

struct hi6250_stub_clk {
	unsigned int id;
	struct clk_hw hw;
	unsigned long *freqs;
	unsigned int cmd;
	unsigned int msg[8];
	unsigned int rate; // mhz
};

static void __iomem *freq_reg;
static struct hi6250_stub_clk_chan stub_clk_chan;

static unsigned long hi6250_stub_clk_recalc_rate(
  struct clk_hw *hw,
	unsigned long parent_rate
) {
	struct hi6250_stub_clk *stub_clk = to_stub_clk(hw);

	unsigned int freq_id = (readl(freq_reg) >> (stub_clk->id * 4)) & 0xf;

	stub_clk->rate = stub_clk->freqs[freq_id];
	return stub_clk->rate;
}

static int hi6250_stub_clk_determine_rate(
  struct clk_hw *hw,
  struct clk_rate_request *req
) {
	return 0;
}

static int hi6250_stub_clk_set_rate(
  struct clk_hw *hw,
  unsigned long rate,
	unsigned long parent_rate
) {
	struct hi6250_stub_clk *stub_clk = to_stub_clk(hw);

	stub_clk->msg[0] = stub_clk->cmd;
	stub_clk->msg[1] = rate / 1000000;

	dev_dbg(stub_clk_chan.cl.dev, "set rate msg[0]=0x%x msg[1]=0x%x\n",
		stub_clk->msg[0], stub_clk->msg[1]);

	mbox_send_message(stub_clk_chan.mbox, stub_clk->msg);
	mbox_client_txdone(stub_clk_chan.mbox, 0);

	stub_clk->rate = rate;
	return 0;
}

static const struct clk_ops hi6250_stub_clk_ops = {
	.recalc_rate    = hi6250_stub_clk_recalc_rate,
	.determine_rate = hi6250_stub_clk_determine_rate,
	.set_rate       = hi6250_stub_clk_set_rate,
};

// mHz
static unsigned long hi6250_stub_clk_freqs_cluster0[] = {
	480000000000,
	807000000000,
	1306000000000,
	1709000000000,
};
static unsigned long hi6250_stub_clk_freqs_cluster1[] = {
	1402000000000,
	1805000000000,
	2016000000000,
	2112000000000,
	2362000000000,
};
static unsigned long hi6250_stub_clk_freqs_ddr[] = {
	120000000000,
	240000000000,
	360000000000,
	533000000000,
	800000000000,
	933000000000,
};
static unsigned long hi6250_stub_clk_freqs_gpu[] = {
	120000000000,
	240000000000,
	360000000000,
	480000000000,
	680000000000,
	800000000000,
	900000000000,
};

static struct hi6250_stub_clk hi6250_stub_clks[HI6250_CLK_STUB_NUM] = {
	DEFINE_CLK_STUB(HI6250_CLK_STUB_CLUSTER0, hi6250_stub_clk_freqs_cluster0, 0x0001030A, "cpu-cluster.0"),
	DEFINE_CLK_STUB(HI6250_CLK_STUB_CLUSTER1, hi6250_stub_clk_freqs_cluster1, 0x0002030A, "cpu-cluster.1"),
	DEFINE_CLK_STUB(HI6250_CLK_STUB_DDR, hi6250_stub_clk_freqs_ddr, 0x00040309, "clk_ddrc"),
	DEFINE_CLK_STUB(HI6250_CLK_STUB_GPU, hi6250_stub_clk_freqs_gpu, 0x0003030A, "clk_g3d"),
};

static struct clk_hw *hi6250_stub_clk_hw_get(
  struct of_phandle_args *clkspec,
	void *data
) {
	unsigned int idx = clkspec->args[0];

	if (idx >= HI6250_CLK_STUB_NUM) {
		pr_err("%s: invalid index %u\n", __func__, idx);
		return ERR_PTR(-EINVAL);
	}

	return &hi6250_stub_clks[idx].hw;
}

static int hi6250_stub_clk_probe(
  struct platform_device *pdev
) {
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	unsigned int i;
	int ret;

	/* Use mailbox client without blocking */
	stub_clk_chan.cl.dev = dev;
	stub_clk_chan.cl.tx_done = NULL;
	stub_clk_chan.cl.tx_block = false;
	stub_clk_chan.cl.knows_txdone = false;

	/* Allocate mailbox channel */
	stub_clk_chan.mbox = mbox_request_channel(&stub_clk_chan.cl, 0);
	if (IS_ERR(stub_clk_chan.mbox))
		return PTR_ERR(stub_clk_chan.mbox);

	freq_reg = syscon_regmap_lookup_by_phandle(np,
				"hisilicon,hi6250-sys-ctrl");
	if (IS_ERR(freq_reg)) {
		dev_err(dev, "failed to get sysctrl regmap\n");
		return PTR_ERR(freq_reg);
	}

	freq_reg += HI6250_STUB_CLOCK_BASE;

	for (i = 0; i < HI6250_CLK_STUB_NUM; i++) {
		ret = devm_clk_hw_register(&pdev->dev, &hi6250_stub_clks[i].hw);
		if (ret)
			return ret;
	}

	return devm_of_clk_add_hw_provider(&pdev->dev, hi6250_stub_clk_hw_get,
					   hi6250_stub_clks);
}

static const struct of_device_id hi6250_stub_clk_of_match[] = {
	{ .compatible = "hisilicon,hi6250-stub-clk", },
	{}
};

static struct platform_driver hi6250_stub_clk_driver = {
	.probe	= hi6250_stub_clk_probe,
	.driver = {
		.name = "hi6250-stub-clk",
		.of_match_table = hi6250_stub_clk_of_match,
	},
};

static int __init hi6250_stub_clk_init(void)
{
	return platform_driver_register(&hi6250_stub_clk_driver);
}
subsys_initcall(hi6250_stub_clk_init);
