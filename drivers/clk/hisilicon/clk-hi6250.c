// SPDX-License-Identifier: GPL-2.0-only
/*
 * Hisilicon Hi6250 clock driver
 *
 * Copyright (C) 2025, Tildeguy (tildeguy@mainlining.org)
 */

#include <linux/kernel.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/io.h>
#include <linux/slab.h>

#include <dt-bindings/clock/hi6250-clock.h>

#include "clk.h"


// ao

static const struct hisi_fixed_rate_clock hi6250_fixed_rate_clks[] = {
  // { HI6250_CLKIN_SYS, "clkin_sys", NULL, 0, 19200000 },
  // { HI6250_CLKIN_REF, "clkin_ref", NULL, 0, 32764 },
  // { HI6250_CLK_FLL_SRC, "clk_fll_src", NULL, 0, 128000000 },
  // { HI6250_CLK_PPLL0, "clk_ppll0", NULL, 0, 1440000000 },
  // { HI6250_CLK_PPLL1, "clk_ppll1", NULL, 0, 1334000000 },
  // { HI6250_CLK_PPLL2, "clk_ppll2", NULL, 0, 1290000000 },
  // { HI6250_CLK_PPLL3, "clk_ppll3", NULL, 0, 1600000000 },
  // { HI6250_CLK_MODEM_BASE, "clk_modem_base", NULL, 0, 49152000 },
  // { HI6250_CLK_FAKE_DISPLAY, "clk_fake_display", NULL, 0, 20000000 },
  { HI6250_APB_PCLK, "apb_pclk", NULL, 0, 20000000 },
  // { HI6250_UART0CLK_DBG, "uart0clk_dbg", NULL, 0, 19200000 },
  { HI6250_OSC32KHZ, "osc32khz", NULL, 0, 32768 },
  // { HI6250_OSC19MHZ, "osc19mhz", NULL, 0, 19200000 },
  // { HI6250_AUTODIV_SOURCEBUS, "autodiv_sourcebus", NULL, 0, 19200000 },
  // { HI6250_CLK_FPGA_2M, "clk_fpga_2m", NULL, 0, 2000000 },
  // { HI6250_CLK_FPGA_10M, "clk_fpga_10m", NULL, 0, 10000000 },
  // { HI6250_CLK_FPGA_20M, "clk_fpga_20m", NULL, 0, 20000000 },
  // { HI6250_CLK_FPGA_24M, "clk_fpga_24m", NULL, 0, 24000000 },
  // { HI6250_CLK_FPGA_26M, "clk_fpga_26m", NULL, 0, 26000000 },
  // { HI6250_CLK_FPGA_27M, "clk_fpga_27m", NULL, 0, 27000000 },
  // { HI6250_CLK_FPGA_32M, "clk_fpga_32m", NULL, 0, 32000000 },
  // { HI6250_CLK_FPGA_40M, "clk_fpga_40m", NULL, 0, 40000000 },
  // { HI6250_CLK_FPGA_50M, "clk_fpga_50m", NULL, 0, 50000000 },
  // { HI6250_CLK_FPGA_57M, "clk_fpga_57m", NULL, 0, 57000000 },
  // { HI6250_CLK_FPGA_60M, "clk_fpga_60m", NULL, 0, 60000000 },
  // { HI6250_CLK_FPGA_64M, "clk_fpga_64m", NULL, 0, 64000000 },
  // { HI6250_CLK_FPGA_80M, "clk_fpga_80m", NULL, 0, 80000000 },
  // { HI6250_CLK_FPGA_100M, "clk_fpga_100m", NULL, 0, 100000000 },
  // { HI6250_CLK_FPGA_160M, "clk_fpga_160m", NULL, 0, 160000000 },
  // { HI6250_CLK_FPGA_150M, "clk_fpga_150m", NULL, 0, 150000000 },
};

static void __init hi6250_clk_ao_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data_ao;
	struct clk **clk_table;

	/*
	 * ao_ctrl only provides fixed-rate clocks (PLLs) which don't need
	 * register access.  Bypass hisi_clk_init() because of_iomap() fails
	 * when ao_ctrl and sys_ctrl share the same register block (0xfff0a000).
	 */
	clk_data_ao = kzalloc(sizeof(*clk_data_ao), GFP_KERNEL);
	if (!clk_data_ao)
		return;

	clk_table = kcalloc(HI6250_AO_NR_CLKS, sizeof(*clk_table), GFP_KERNEL);
	if (!clk_table) {
		kfree(clk_data_ao);
		return;
	}

	clk_data_ao->base = NULL;
	clk_data_ao->clk_data.clks = clk_table;
	clk_data_ao->clk_data.clk_num = HI6250_AO_NR_CLKS;
	of_clk_add_provider(np, of_clk_src_onecell_get, &clk_data_ao->clk_data);

	hisi_clk_register_fixed_rate(hi6250_fixed_rate_clks,
				ARRAY_SIZE(hi6250_fixed_rate_clks), clk_data_ao);
}
CLK_OF_DECLARE_DRIVER(hi6250_clk_ao, "hisilicon,hi6250-aoctrl", hi6250_clk_ao_init);


// pmuctrl

static const struct hisi_gate_clock hi6250_pmuctrl_gate_clks[] = {
  // { HI6250_CLK_GATE_ABB_192, "clk_gate_abb_192", "clkin_sys", 0, 0x43c, 0, 9, },
  // { HI6250_CLK_PMU32KA, "clk_pmu32ka", "clkin_ref", 0, 0x484, 0, 0, },
  // { HI6250_CLK_PMU32KB, "clk_pmu32kb", "clkin_ref", 0, 0x484, 1, 0, },
  // { HI6250_CLK_PMU32KC, "clk_pmu32kc", "clkin_ref", 0, 0x484, 2, 0, },
  // { HI6250_CLK_PMUAUDIOCLK, "clk_pmuaudioclk", "clkin_sys", 0, 0x450, 0, 0, },
};

static void __init hi6250_clk_pmuctrl_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data_pmu;

	clk_data_pmu = hisi_clk_init(np, HI6250_PMUCTRL_NR_CLKS);
	if (!clk_data_pmu)
		return;

	hisi_clk_register_gate(hi6250_pmuctrl_gate_clks,
				ARRAY_SIZE(hi6250_pmuctrl_gate_clks), clk_data_pmu);
}
CLK_OF_DECLARE_DRIVER(hi6250_clk_pmuctrl, "hisilicon,hi6250-pmuctrl", hi6250_clk_pmuctrl_init);


// pctrl
// only stubs here ._.


// sctrl

static const struct hisi_fixed_factor_clock hi6250_sctrl_fixed_factor_clks[] = {
  // { HI6250_CLK_FACTOR_TCXO, "clk_factor_tcxo", "clkin_sys", 0x1, 4, 0, },
  // { HI6250_CLK_180M, "clk_180m", "clk_ppll0", 0x1, 8, 0, },
  // { HI6250_AUTODIV_SOCP, "autodiv_socp", "autodiv_dbgbus", 0x1, 1, 0, },
};


static const struct hisi_gate_clock hi6250_sctrl_gate_clks[] = {
  // { HI6250_CLK_ANGT_ASP_SUBSYS, "clk_angt_asp_subsys", "clk_ap_ppll0", CLK_GATE_HIWORD_MASK, 0x258, 0, 0, },
  // { HI6250_CLK_MMBUF_PLL_ANDGT, "clk_mmbuf_pll_andgt", "clk_ppll0", CLK_GATE_HIWORD_MASK, 0x258, 6, 0, },
  // { HI6250_PCLK_MMBUF_ANDGT, "pclk_mmbuf_andgt", "clk_mmbuf_sw", CLK_GATE_HIWORD_MASK, 0x258, 7, 0, },
  // { HI6250_CLK_SYS_MMBUF_ANDGT, "clk_sys_mmbuf_andgt", "clkin_sys", CLK_GATE_HIWORD_MASK, 0x258, 6, 0, },
  // { HI6250_CLK_FLL_MMBUF_ANDGT, "clk_fll_mmbuf_andgt", "clk_fll_src", CLK_GATE_HIWORD_MASK, 0x258, 6, 0, },
  // { HI6250_CLK_GATE_UART3, "clk_gate_uart3", "clk_180m", CLK_GATE_HIWORD_MASK, 0x580, 0, 0, },
};


static const struct hisi_gate_clock hi6250_sctrl_gate_sep_clks[] = {
  // { HI6250_CLK_TIMER0_A, "clk_timer0_a", "clkmux_timer0_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER0_B, "clk_timer0_b", "clkmux_timer0_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER2_A, "clk_timer2_a", "clkmux_timer2_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER2_B, "clk_timer2_b", "clkmux_timer2_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER3_A, "clk_timer3_a", "clkmux_timer3_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER3_B, "clk_timer3_b", "clkmux_timer3_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER0, "clk_timer0", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 4, 0, },
  // { HI6250_CLK_TIMER2, "clk_timer2", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 8, 0, },
  // { HI6250_CLK_TIMER3, "clk_timer3", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 10, 0, },
  // { HI6250_CLK_TIMER4_A, "clk_timer4_a", "clkmux_timer4_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER4_B, "clk_timer4_b", "clkmux_timer4_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER5_A, "clk_timer5_a", "clkmux_timer5_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER5_B, "clk_timer5_b", "clkmux_timer5_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER6_A, "clk_timer6_a", "clkmux_timer6_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER6_B, "clk_timer6_b", "clkmux_timer6_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER7_A, "clk_timer7_a", "clkmux_timer7_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER7_B, "clk_timer7_b", "clkmux_timer7_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER8_A, "clk_timer8_a", "clkmux_timer8_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER8_B, "clk_timer8_b", "clkmux_timer8_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
  // { HI6250_CLK_TIMER4, "clk_timer4", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 11, 0, },
  // { HI6250_CLK_TIMER5, "clk_timer5", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 13, 0, },
  // { HI6250_CLK_TIMER6, "clk_timer6", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 15, 0, },
  // { HI6250_CLK_TIMER7, "clk_timer7", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 17, 0, },
  // { HI6250_CLK_TIMER8, "clk_timer8", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 19, 0, },
  // { HI6250_PCLK_RTC, "pclk_rtc", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 1, 0, },
  // { HI6250_PCLK_RTC1, "pclk_rtc1", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 2, 0, },
  // { HI6250_PCLK_AO_GPIO0, "pclk_ao_gpio0", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 11, 0, },
  // { HI6250_PCLK_AO_GPIO1, "pclk_ao_gpio1", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 12, 0, },
  // { HI6250_PCLK_AO_GPIO2, "pclk_ao_gpio2", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 13, 0, },
  // { HI6250_PCLK_AO_GPIO3, "pclk_ao_gpio3", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 14, 0, },
  // { HI6250_PCLK_AO_GPIO4, "pclk_ao_gpio4", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 21, 0, },
  // { HI6250_PCLK_AO_GPIO5, "pclk_ao_gpio5", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 22, 0, },
  // { HI6250_PCLK_AO_GPIO6, "pclk_ao_gpio6", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x190, 17, 0, },
  // { HI6250_PCLK_AO_GPIO7, "pclk_ao_gpio7", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x190, 18, 0, },
  // { HI6250_PCLK_AO_GPIO8, "pclk_ao_gpio8", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x190, 19, 0, },
  // { HI6250_CLK_OUT0, "clk_out0", "clkmux_clkout0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 16, 0, },
  // { HI6250_CLK_OUT1, "clk_out1", "clkmux_clkout1", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 17, 0, },
  // { HI6250_PCLK_SYSCNT, "pclk_syscnt", "clk_aobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 19, 0, },
  // { HI6250_CLK_SYSCNT, "clk_syscnt", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 20, 0, },
  // { HI6250_CLK_ASP_TCXO, "clk_asp_tcxo", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x160, 27, 0, },
  // { HI6250_ACLK_ASC, "aclk_asc", "clk_mmbuf_sw", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 8, 0, },
  // { HI6250_CLK_AOBUS2MMBUF, "clk_aobus2mmbuf", "clk_mmbuf_sw", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 6, 0, },
  // { HI6250_CLK_DSS_AXI_MM, "clk_dss_axi_mm", "clk_mmbuf_sw", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 24, 0, },
  // { HI6250_ACLK_MMBUF, "aclk_mmbuf", "clk_mmbuf_sw", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 22, 0, },
  // { HI6250_PCLK_MMBUF, "pclk_mmbuf", "pclk_mmbuf_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x170, 23, 0, },
  // { HI6250_CLK_ASPCODEC, "clk_aspcodec", "clk_modem_base", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x190, 20, 0, },
};


static const struct hi6220_divider_clock hi6250_sctrl_divider_clks[] = {
  // { HI6250_CLK_AOBUS_DIV, "clk_aobus_div", "clk_ap_ppll0", CLK_SET_RATE_PARENT, 0x254, 0, 6, 0x3f0000, },
  // { HI6250_CLKDIV_OUT0TCXO, "clkdiv_out0tcxo", "clkin_sys", CLK_SET_RATE_PARENT, 0x254, 6, 3, 0x1c00000, },
  // { HI6250_CLKDIV_OUT1TCXO, "clkdiv_out1tcxo", "clkin_sys", CLK_SET_RATE_PARENT, 0x254, 9, 3, 0xe000000, },
  // { HI6250_CLKDIV_ASPSYS, "clkdiv_aspsys", "clk_ap_ppll0", CLK_SET_RATE_PARENT, 0x250, 0, 3, 0x70000, },
  // { HI6250_ACLK_MMBUF_DIV, "aclk_mmbuf_div", "clk_mmbuf_pll_andgt", CLK_SET_RATE_PARENT, 0x258, 12, 4, 0xf0000000, },
  // { HI6250_PCLK_MMBUF_DIV, "pclk_mmbuf_div", "pclk_mmbuf_andgt", CLK_SET_RATE_PARENT, 0x258, 10, 2, 0xc000000, },
};


// static const char *clkmux_timer0_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer0", "apb_pclk" };
// static const char *clkmux_timer0_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer0", "apb_pclk" };
// static const char *clkmux_timer2_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer2", "apb_pclk" };
// static const char *clkmux_timer2_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer2", "apb_pclk" };
// static const char *clkmux_timer3_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer3", "apb_pclk" };
// static const char *clkmux_timer3_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer3", "apb_pclk" };
// static const char *clkmux_timer4_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer4", "apb_pclk" };
// static const char *clkmux_timer4_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer4", "apb_pclk" };
// static const char *clkmux_timer5_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer5", "apb_pclk" };
// static const char *clkmux_timer5_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer5", "apb_pclk" };
// static const char *clkmux_timer6_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer6", "apb_pclk" };
// static const char *clkmux_timer6_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer6", "apb_pclk" };
// static const char *clkmux_timer7_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer7", "apb_pclk" };
// static const char *clkmux_timer7_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer7", "apb_pclk" };
// static const char *clkmux_timer8_a_p[] = { "clkin_ref", "apb_pclk", "clk_timer8", "apb_pclk" };
// static const char *clkmux_timer8_b_p[] = { "clkin_ref", "apb_pclk", "clk_timer8", "apb_pclk" };
// static const char *clkmux_clkout0_p[] = { "clkin_ref", "clkdiv_out0tcxo", "clkdiv_out0_pll", "clkdiv_out0_pll" };
// static const char *clkmux_clkout1_p[] = { "clkin_ref", "clkdiv_out1tcxo", "clkdiv_out1_pll", "clkdiv_out1_pll" };
// static const char *clk_asp_pll_sel_p[] = { "clkdiv_aspsys", "clk_fll_src" };
// static const char *clk_mmbuf_sw_p[] = { "clk_sys_mmbuf_andgt", "clk_fll_mmbuf_andgt", "aclk_mmbuf_div", "aclk_mmbuf_div" };

static const struct hisi_mux_clock hi6250_sctrl_mux_clks[] = {
  // { HI6250_CLKMUX_TIMER0_A, "clkmux_timer0_a", clkmux_timer0_a_p, ARRAY_SIZE(clkmux_timer0_a_p), CLK_SET_RATE_PARENT, 0x3c0, 2, 2, 0, },
  // { HI6250_CLKMUX_TIMER0_B, "clkmux_timer0_b", clkmux_timer0_b_p, ARRAY_SIZE(clkmux_timer0_b_p), CLK_SET_RATE_PARENT, 0x3c0, 4, 2, 0, },
  // { HI6250_CLKMUX_TIMER2_A, "clkmux_timer2_a", clkmux_timer2_a_p, ARRAY_SIZE(clkmux_timer2_a_p), CLK_SET_RATE_PARENT, 0x3c0, 6, 2, 0, },
  // { HI6250_CLKMUX_TIMER2_B, "clkmux_timer2_b", clkmux_timer2_b_p, ARRAY_SIZE(clkmux_timer2_b_p), CLK_SET_RATE_PARENT, 0x3c0, 8, 2, 0, },
  // { HI6250_CLKMUX_TIMER3_A, "clkmux_timer3_a", clkmux_timer3_a_p, ARRAY_SIZE(clkmux_timer3_a_p), CLK_SET_RATE_PARENT, 0x3c0, 10, 2, 0, },
  // { HI6250_CLKMUX_TIMER3_B, "clkmux_timer3_b", clkmux_timer3_b_p, ARRAY_SIZE(clkmux_timer3_b_p), CLK_SET_RATE_PARENT, 0x3c0, 12, 2, 0, },
  // { HI6250_CLKMUX_TIMER4_A, "clkmux_timer4_a", clkmux_timer4_a_p, ARRAY_SIZE(clkmux_timer4_a_p), CLK_SET_RATE_PARENT, 0x3c4, 0, 2, 0, },
  // { HI6250_CLKMUX_TIMER4_B, "clkmux_timer4_b", clkmux_timer4_b_p, ARRAY_SIZE(clkmux_timer4_b_p), CLK_SET_RATE_PARENT, 0x3c4, 2, 2, 0, },
  // { HI6250_CLKMUX_TIMER5_A, "clkmux_timer5_a", clkmux_timer5_a_p, ARRAY_SIZE(clkmux_timer5_a_p), CLK_SET_RATE_PARENT, 0x3c4, 4, 2, 0, },
  // { HI6250_CLKMUX_TIMER5_B, "clkmux_timer5_b", clkmux_timer5_b_p, ARRAY_SIZE(clkmux_timer5_b_p), CLK_SET_RATE_PARENT, 0x3c4, 6, 2, 0, },
  // { HI6250_CLKMUX_TIMER6_A, "clkmux_timer6_a", clkmux_timer6_a_p, ARRAY_SIZE(clkmux_timer6_a_p), CLK_SET_RATE_PARENT, 0x3c4, 8, 2, 0, },
  // { HI6250_CLKMUX_TIMER6_B, "clkmux_timer6_b", clkmux_timer6_b_p, ARRAY_SIZE(clkmux_timer6_b_p), CLK_SET_RATE_PARENT, 0x3c4, 10, 2, 0, },
  // { HI6250_CLKMUX_TIMER7_A, "clkmux_timer7_a", clkmux_timer7_a_p, ARRAY_SIZE(clkmux_timer7_a_p), CLK_SET_RATE_PARENT, 0x3c4, 12, 2, 0, },
  // { HI6250_CLKMUX_TIMER7_B, "clkmux_timer7_b", clkmux_timer7_b_p, ARRAY_SIZE(clkmux_timer7_b_p), CLK_SET_RATE_PARENT, 0x3c4, 14, 2, 0, },
  // { HI6250_CLKMUX_TIMER8_A, "clkmux_timer8_a", clkmux_timer8_a_p, ARRAY_SIZE(clkmux_timer8_a_p), CLK_SET_RATE_PARENT, 0x3c4, 16, 2, 0, },
  // { HI6250_CLKMUX_TIMER8_B, "clkmux_timer8_b", clkmux_timer8_b_p, ARRAY_SIZE(clkmux_timer8_b_p), CLK_SET_RATE_PARENT, 0x3c4, 18, 2, 0, },
  // { HI6250_CLKMUX_CLKOUT0, "clkmux_clkout0", clkmux_clkout0_p, ARRAY_SIZE(clkmux_clkout0_p), CLK_SET_RATE_PARENT, 0x254, 12, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_CLKOUT1, "clkmux_clkout1", clkmux_clkout1_p, ARRAY_SIZE(clkmux_clkout1_p), CLK_SET_RATE_PARENT, 0x254, 14, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_ASP_PLL_SEL, "clk_asp_pll_sel", clk_asp_pll_sel_p, ARRAY_SIZE(clk_asp_pll_sel_p), CLK_SET_RATE_PARENT, 0x250, 11, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_MMBUF_SW, "clk_mmbuf_sw", clk_mmbuf_sw_p, ARRAY_SIZE(clk_mmbuf_sw_p), CLK_SET_RATE_PARENT, 0x258, 8, 2, CLK_MUX_HIWORD_MASK, },
};

static void __init hi6250_clk_sys_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data_sys;

	clk_data_sys = hisi_clk_init(np, HI6250_SCTRL_NR_CLKS);
	if (!clk_data_sys)
		return;

	hisi_clk_register_fixed_factor(hi6250_sctrl_fixed_factor_clks,
				ARRAY_SIZE(hi6250_sctrl_fixed_factor_clks), clk_data_sys);
	hisi_clk_register_gate(hi6250_sctrl_gate_clks,
				ARRAY_SIZE(hi6250_sctrl_gate_clks), clk_data_sys);
	hisi_clk_register_gate_sep(hi6250_sctrl_gate_sep_clks,
				ARRAY_SIZE(hi6250_sctrl_gate_sep_clks), clk_data_sys);
  hi6220_clk_register_divider(hi6250_sctrl_divider_clks,
				ARRAY_SIZE(hi6250_sctrl_divider_clks), clk_data_sys);
  hisi_clk_register_mux(hi6250_sctrl_mux_clks,
				ARRAY_SIZE(hi6250_sctrl_mux_clks), clk_data_sys);
}
CLK_OF_DECLARE_DRIVER(hi6250_clk_sysctrl, "hisilicon,hi6250-sysctrl", hi6250_clk_sys_init);


// crgctrl

static const struct hisi_fixed_factor_clock hi6250_crgctrl_fixed_factor_clks[] = {
  // { HI6250_CLK_SYSBUS_DIV, "clk_sysbus_div", "clk_sysbus_mux", 0x1, 6, 0, },
  // { HI6250_CLK_WD0_HIGH, "clk_wd0_high", "clk_cfgbus_div", 0x1, 1, 0, },
  // { HI6250_CLK_AT, "clk_at", "clk_cssys_div", 0x1, 1, 0, },
  // { HI6250_CLK_TRACK, "clk_track", "clkdiv_track", 0x1, 1, 0, },
  // { HI6250_PCLK_DBG, "pclk_dbg", "pclkdiv_dbg", 0x1, 1, 0, },
  // { HI6250_CLK_DMA_IOMCU, "clk_dma_iomcu", "clk_fll_src", 0x1, 4, 0, },
  // { HI6250_CLK_FACTOR_MMC0, "clk_factor_mmc0", "clkin_sys", 0x1, 6, 0, },
  // { HI6250_CLK_A53HPM_DIV, "clk_a53hpm_div", "clk_ap_ppll0", 0x1, 3, 0, },
  // { HI6250_CLK_UART0_FAC, "clk_uart0_fac", "clkmux_uartl", 0x1, 1, 0, },
  // { HI6250_CLKFAC_USB2PHY, "clkfac_usb2phy", "clk_ap_ppll0", 0x1, 60, 0, },
  // { HI6250_CLK_ABB_USB, "clk_abb_usb", "clk_gate_abb_192", 0x1, 1, 0, },
  // { HI6250_CLK_BLPWM, "clk_blpwm", "clk_ap_ppll0", 0x1, 8, 0, },
  // { HI6250_CLK_GPS_REF, "clk_gps_ref", "clkmux_gps_ref", 0x1, 1, 0, },
  // { HI6250_CLK_FAC_ISPSN, "clk_fac_ispsn", "clk_isp_snclk_angt", 0x1, 10, 0, },
  // { HI6250_CLK_RXDCFG_FAC, "clk_rxdcfg_fac", "clk_andgt_rxdphy", 0x1, 6, 0, },
  // { HI6250_CLK_LOADMONITOR0_DIV, "clk_loadmonitor0_div", "clk_andgt_loadmonitor0", 0x1, 2, 0, },
  // { HI6250_CLK_60M_DIV, "clk_60m_div", "clk_a53hpm_div", 0x1, 8, 0, },
  // { HI6250_UART6CLK, "uart6clk", "clkin_sys", 0x1, 1, 0, },
  // { HI6250_CLK_I2C0, "clk_i2c0", "clk_fll_src", 0x1, 1, 0, },
  // { HI6250_CLK_I2C1, "clk_i2c1", "clk_fll_src", 0x1, 1, 0, },
  // { HI6250_CLK_I2C2, "clk_i2c2", "clk_fll_src", 0x1, 1, 0, },
  // { HI6250_CLK_SPI0, "clk_spi0", "clk_fll_src", 0x1, 1, 0, },
  // { HI6250_CLK_SPI2, "clk_spi2", "clk_ppll0", 0x1, 8, 0, },
  // { HI6250_CLK_UART7, "clk_uart7", "clkmux_uartl", 0x1, 1, 0, },
};


static const struct hisi_gate_clock hi6250_crgctrl_gate_clks[] = {
  // { HI6250_CLK_GATE_VIVOBUS_ANDGT, "clk_gate_vivobus_andgt", "clk_vivobus_mux", CLK_GATE_HIWORD_MASK, 0xf8, 1, 0, },
  // { HI6250_CLK_GATE_VCODECBUS_ANDGT, "clk_gate_vcodecbus_andgt", "clk_vcodecbus_mux", CLK_GATE_HIWORD_MASK, 0xf8, 2, 0, },
  // { HI6250_CLK_ANDGT_MMC0, "clk_andgt_mmc0", "clk_mmc0_muxpll", CLK_GATE_HIWORD_MASK, 0xf4, 2, 0, },
  // { HI6250_CLK_ANDGT_MMC1, "clk_andgt_mmc1", "clk_sd_muxpll", CLK_GATE_HIWORD_MASK, 0xf4, 3, 0, },
  // { HI6250_CLK_ANDGT_SDIO0, "clk_andgt_sdio0", "clk_sdio0_muxpl", CLK_GATE_HIWORD_MASK, 0xf4, 4, 0, },
  // { HI6250_CLK_A53HPM_ANDGT, "clk_a53hpm_andgt", "clk_a53hpm_mux", CLK_GATE_HIWORD_MASK, 0xf4, 7, 0, },
  // { HI6250_CLK_ANDGT_UARTH, "clk_andgt_uarth", "clk_a53hpm_div", CLK_GATE_HIWORD_MASK, 0xf4, 11, 0, },
  // { HI6250_CLK_ANDGT_UARTL, "clk_andgt_uartl", "clk_a53hpm_div", CLK_GATE_HIWORD_MASK, 0xf4, 12, 0, },
  // { HI6250_CLK_ANDGT_SPI, "clk_andgt_spi", "clk_a53hpm_div", CLK_GATE_HIWORD_MASK, 0xf4, 13, 0, },
  // { HI6250_CLK_ANDGT_OUT0, "clk_andgt_out0", "clk_ap_ppll3", CLK_GATE_HIWORD_MASK, 0xf0, 10, 0, },
  // { HI6250_CLK_ANDGT_OUT1, "clk_andgt_out1", "clk_ap_ppll3", CLK_GATE_HIWORD_MASK, 0xf0, 11, 0, },
  // { HI6250_CLK_ANDGT_EDC0, "clk_andgt_edc0", "clkmux_edc0", CLK_GATE_HIWORD_MASK, 0xf0, 8, 0, },
  // { HI6250_CLK_ANDGT_LDI0, "clk_andgt_ldi0", "clkmux_ldi0", CLK_GATE_HIWORD_MASK, 0xf0, 6, 0, },
  // { HI6250_CLK_ANDGT_VENC, "clk_andgt_venc", "clkmux_venc", CLK_GATE_HIWORD_MASK, 0xf4, 0, 0, },
  // { HI6250_CLK_ANDGT_VDEC, "clk_andgt_vdec", "clkmux_vdec", CLK_GATE_HIWORD_MASK, 0xf0, 15, 0, },
  // { HI6250_CLK_ANDGT_ISPA7, "clk_andgt_ispa7", "clkmux_ispa7", CLK_GATE_HIWORD_MASK, 0xf8, 4, 0, },
  // { HI6250_CLK_ANDGT_ISPFUNC, "clk_andgt_ispfunc", "clkmux_ispfunc", CLK_GATE_HIWORD_MASK, 0xf0, 13, 0, },
  // { HI6250_CLK_ISP_SNCLK_ANGT, "clk_isp_snclk_angt", "clk_a53hpm_div", CLK_GATE_HIWORD_MASK, 0x108, 2, 0, },
  // { HI6250_CLK_ANDGT_RXDPHY, "clk_andgt_rxdphy", "clk_a53hpm_div", CLK_GATE_HIWORD_MASK, 0xf0, 12, 0, },
  // { HI6250_CLK_ANDGT_LOADMONITOR0, "clk_andgt_loadmonitor0", "clk_ppll0", CLK_GATE_HIWORD_MASK, 0xf0, 3, 0, },
  // { HI6250_AUTODIV_SYSBUS, "autodiv_sysbus", "autodiv_sourcebus", CLK_GATE_HIWORD_MASK, 0x404, 5, 0, },
  // { HI6250_AUTODIV_CFGBUS, "autodiv_cfgbus", "autodiv_sysbus", CLK_GATE_HIWORD_MASK, 0x404, 4, 0, },
  // { HI6250_AUTODIV_DMABUS, "autodiv_dmabus", "autodiv_sysbus", CLK_GATE_HIWORD_MASK, 0x404, 3, 0, },
  // { HI6250_AUTODIV_DBGBUS, "autodiv_dbgbus", "autodiv_sysbus", CLK_GATE_HIWORD_MASK, 0x404, 2, 0, },
  // { HI6250_AUTODIV_EMMC0BUS, "autodiv_emmc0bus", "autodiv_sourcebus", CLK_GATE_HIWORD_MASK, 0x404, 1, 0, },
  // { HI6250_AUTODIV_EMMC1BUS, "autodiv_emmc1bus", "autodiv_sourcebus", CLK_GATE_HIWORD_MASK, 0x404, 0, 0, },
};


static const struct hisi_gate_clock hi6250_crgctrl_gate_sep_clks[] = {
//   { HI6250_PCLK_GPIO0, "pclk_gpio0", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 0, 0, },
//   { HI6250_PCLK_GPIO1, "pclk_gpio1", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 1, 0, },
//   { HI6250_PCLK_GPIO2, "pclk_gpio2", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 2, 0, },
//   { HI6250_PCLK_GPIO3, "pclk_gpio3", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 3, 0, },
//   { HI6250_PCLK_GPIO4, "pclk_gpio4", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 4, 0, },
//   { HI6250_PCLK_GPIO5, "pclk_gpio5", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 5, 0, },
//   { HI6250_PCLK_GPIO6, "pclk_gpio6", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 6, 0, },
//   { HI6250_PCLK_GPIO7, "pclk_gpio7", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 7, 0, },
//   { HI6250_PCLK_GPIO8, "pclk_gpio8", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 8, 0, },
//   { HI6250_PCLK_GPIO9, "pclk_gpio9", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 9, 0, },
//   { HI6250_PCLK_GPIO10, "pclk_gpio10", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 10, 0, },
//   { HI6250_PCLK_GPIO11, "pclk_gpio11", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 11, 0, },
//   { HI6250_PCLK_GPIO12, "pclk_gpio12", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 12, 0, },
//   { HI6250_PCLK_GPIO13, "pclk_gpio13", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 13, 0, },
//   { HI6250_PCLK_GPIO14, "pclk_gpio14", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 14, 0, },
//   { HI6250_PCLK_GPIO15, "pclk_gpio15", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 15, 0, },
//   { HI6250_PCLK_GPIO16, "pclk_gpio16", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 16, 0, },
//   { HI6250_PCLK_GPIO17, "pclk_gpio17", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 17, 0, },
//   { HI6250_PCLK_GPIO18, "pclk_gpio18", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 18, 0, },
//   { HI6250_PCLK_GPIO19, "pclk_gpio19", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 19, 0, },
//   { HI6250_PCLK_GPIO20, "pclk_gpio20", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 20, 0, },
//   { HI6250_PCLK_GPIO21, "pclk_gpio21", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 21, 0, },
//   { HI6250_PCLK_WD0_HIGH, "pclk_wd0_high", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 16, 0, },
//   { HI6250_PCLK_WD0, "pclk_wd0", "clk_wd0_mux", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_PCLK_WD1, "pclk_wd1", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 17, 0, },
//   { HI6250_HCLK_ISP, "hclk_isp", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 24, 0, },
//   { HI6250_PCLK_DSS, "pclk_dss", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 12, 0, },
//   { HI6250_PCLK_DSI0, "pclk_dsi0", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 28, 0, },
//   { HI6250_PCLK_DSI1, "pclk_dsi1", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 29, 0, },
//   { HI6250_PCLK_PCTRL, "pclk_pctrl", "clk_ptp_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 31, 0, },
//   { HI6250_CLK_VCODECCFG, "clk_vcodeccfg", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 5, 0, },
//   { HI6250_CLK_CODECSSI, "clk_codecssi", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 26, 0, },
//   { HI6250_PCLK_CODECSSI, "pclk_codecssi", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 26, 0, },
//   { HI6250_CLK_HKADCSSI, "clk_hkadcssi", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 24, 0, },
//   { HI6250_PCLK_HKADCSSI, "pclk_hkadcssi", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 24, 0, },
//   { HI6250_HCLK_EMMC0, "hclk_emmc0", "clk_mmc0bus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 13, 0, },
//   { HI6250_HCLK_SDIO0, "hclk_sdio0", "clk_mmc1bus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 19, 0, },
//   { HI6250_HCLK_SD, "hclk_sd", "clk_mmc1bus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 30, 0, },
//   { HI6250_CLK_DBGBUS, "clk_dbgbus", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 25, 0, },
//   { HI6250_CLK_CSSYS_ATCLK, "clk_cssys_atclk", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x128, 25, 0, },
//   { HI6250_CLK_SECP, "clk_secp", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 12, 0, },
//   { HI6250_CLK_SOCP, "clk_socp", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 28, 0, },
//   { HI6250_ACLK_PERF_STAT, "aclk_perf_stat", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 10, 0, },
//   { HI6250_PCLK_PERF_STAT, "pclk_perf_stat", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 9, 0, },
//   { HI6250_CLK_PERF_STAT, "clk_perf_stat", "clk_60m", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 8, 0, },
//   { HI6250_CLK_DMAC, "clk_dmac", "clk_dmabus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 1, 0, },
//   { HI6250_ACLK_DSS, "aclk_dss", "clk_vivobus", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 13, 0, },
//   { HI6250_ACLK_ISP, "aclk_isp", "clk_vivobus", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 23, 0, },
//   { HI6250_CLK_VIVOBUS2DDR, "clk_vivobus2ddr", "clk_vivobus", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 12, 0, },
//   { HI6250_CLK_VIVOBUS, "clk_vivobus", "clk_vivobus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 16, 0, },
//   { HI6250_CLK_VCODECBUS, "clk_vcodecbus", "clk_vcodecbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, 6, 0, },
//   { HI6250_CLK_CCI400_BP, "clk_cci400_bp", "clk_ddrc_freq", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x120, 8, 0, },
//   { HI6250_CLK_CCI400, "clk_cci400", "clk_ddrc_freq", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 14, 0, },
//   { HI6250_CLK_EMMC0, "clk_emmc0", "clk_mmc0_muxsys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 15, 0, },
//   { HI6250_CLK_SD, "clk_sd", "clk_sd_muxsys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 17, 0, },
//   { HI6250_CLK_SDIO0, "clk_sdio0", "clk_sdio0_muxsy", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 18, 0, },
//   { HI6250_CLK_GPUHPM, "clk_gpuhpm", "clk_a53hpm_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 15, 0, },
//   { HI6250_CLK_UART1, "clk_uart1", "clkmux_uarth", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 11, 0, },
//   { HI6250_PCLK_UART4, "pclk_uart4", "clkmux_uarth", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 14, 0, },
//   { HI6250_CLK_UART4, "clk_uart4", "clkmux_uarth", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 14, 0, },
//   { HI6250_CLK_UART0, "clk_uart0", "clkmux_uartl", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 10, 0, },
//   { HI6250_CLK_UART2, "clk_uart2", "clkmux_uartl", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 12, 0, },
//   { HI6250_CLK_UART5, "clk_uart5", "clkmux_uartl", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 15, 0, },
//   { HI6250_CLK_I2C3, "clk_i2c3", "clkmux_i2c", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 7, 0, },
//   { HI6250_CLK_I2C4, "clk_i2c4", "clkmux_i2c", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 27, 0, },
//   { HI6250_CLK_SPI1, "clk_spi1", "clkmux_spi", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 9, 0, },
//   { HI6250_CLK_TIMER9_A, "clk_timer9_a", "clkmux_timer9_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER9_B, "clk_timer9_b", "clkmux_timer9_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER10_A, "clk_timer10_a", "clkmux_timer10_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER10_B, "clk_timer10_b", "clkmux_timer10_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER11_A, "clk_timer11_a", "clkmux_timer11_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER11_B, "clk_timer11_b", "clkmux_timer11_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER12_A, "clk_timer12_a", "clkmux_timer12_a", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER12_B, "clk_timer12_b", "clkmux_timer12_b", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x0, -1, 0, },
//   { HI6250_CLK_TIMER9, "clk_timer9", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 22, 0, },
//   { HI6250_CLK_TIMER10, "clk_timer10", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 23, 0, },
//   { HI6250_CLK_TIMER11, "clk_timer11", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 24, 0, },
//   { HI6250_CLK_TIMER12, "clk_timer12", "clk_factor_tcxo", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x10, 25, 0, },
//   { HI6250_CLK_USB2PHY_PLL, "clk_usb2phy_pll", "clkfac_usb2phy", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 6, 0, },
//   { HI6250_CLK_USB2PHY_REF, "clk_usb2phy_ref", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 2, 0, },
//   { HI6250_HCLK_USB2OTG, "hclk_usb2otg", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 1, 0, },
//   { HI6250_CLK_PWM, "clk_pwm", "clk_ptp_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 0, 0, },
//   { HI6250_CLK_MDM2GPS0, "clk_mdm2gps0", "clk_mdm2gps0_en", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 18, 0, },
//   { HI6250_CLK_MDM2GPS1, "clk_mdm2gps1", "clk_mdm2gps1_en", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 22, 0, },
//   { HI6250_CLK_MDM2GPS0_EN, "clk_mdm2gps0_en", "clk_modem_base", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 6, 0, },
//   { HI6250_CLK_MDM2GPS1_EN, "clk_mdm2gps1_en", "clk_modem_base", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 7, 0, },
//   { HI6250_CLK_EDC0, "clk_edc0", "clkdiv_edc0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 17, 0, },
//   { HI6250_CLK_LDI0, "clk_ldi0", "clkdiv_ldi0", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 15, 0, },
//   { HI6250_CLK_VENC, "clk_venc", "clkdiv_venc", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 10, 0, },
//   { HI6250_CLK_VDEC, "clk_vdec", "clkdiv_vdec", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 11, 0, },
//   { HI6250_CLK_ISP_TIMER, "clk_isp_timer", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 19, 0, },
//   { HI6250_CLK_ISPA7, "clk_ispa7", "clkdiv_ispa7", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 4, 0, },
//   { HI6250_CLK_ISPA7CFG, "clk_ispa7cfg", "clk_cfgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 27, 0, },
//   { HI6250_CLK_ISPFUNC, "clk_ispfunc", "clkdiv_ispfunc", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 25, 0, },
//   { HI6250_CLK_ISP_SNCLK0, "clk_isp_snclk0", "clk_mux_ispsn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 16, 0, },
//   { HI6250_CLK_ISP_SNCLK1, "clk_isp_snclk1", "clk_mux_ispsn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 17, 0, },
//   { HI6250_CLK_ISP_SNCLK2, "clk_isp_snclk2", "clk_mux_ispsn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 18, 0, },
//   { HI6250_CLK_ISP_SNCLK, "clk_isp_snclk", "clk_mux_ispsn", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x50, 18, 0, },
//   { HI6250_CLK_RXDPHY0_CFG, "clk_rxdphy0_cfg", "clk_rxdcfg_mux", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 20, 0, },
//   { HI6250_CLK_RXDPHY1_CFG, "clk_rxdphy1_cfg", "clk_rxdcfg_mux", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 21, 0, },
//   { HI6250_CLK_TXDPHY0_CFG, "clk_txdphy0_cfg", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 28, 0, },
//   { HI6250_CLK_TXDPHY0_REF, "clk_txdphy0_ref", "clkin_sys", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 29, 0, },
//   { HI6250_CLK_LOADMONITOR0, "clk_loadmonitor0", "clk_loadmonitor0_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 5, 0, },
//   { HI6250_CLK_LOADMONITOR1, "clk_loadmonitor1", "clk_a53hpm_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 6, 0, },
//   { HI6250_PCLK_LOADMONITOR, "pclk_loadmonitor", "clk_ptp_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 19, 0, },
//   { HI6250_CLK_60M, "clk_60m", "clk_60m_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x20, 4, 0, },
//   { HI6250_CLK_IPF0, "clk_ipf0", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 3, 0, },
//   { HI6250_PSAM_ACLK, "psam_aclk", "clk_dbgbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x40, 4, 0, },
//   { HI6250_CLK_MODEM2CODEC0, "clk_modem2codec0", "clk_modem2codec0_en", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 19, 0, },
//   { HI6250_CLK_MODEM2CODEC1, "clk_modem2codec1", "clk_modem2codec1_en", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 14, 0, },
//   { HI6250_CLK_MODEM2CODEC0_EN, "clk_modem2codec0_en", "clk_gate_abb_192", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 8, 0, },
//   { HI6250_CLK_MODEM2CODEC1_EN, "clk_modem2codec1_en", "clk_modem_base", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x30, 9, 0, },
//   { HI6250_CLK_ATDIV_VCBUS, "clk_atdiv_vcbus", "clk_vcodecbus_div", CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED, 0x410, 5, 0, },
};


static const struct hi6220_divider_clock hi6250_crgctrl_divider_clks[] = {
//   { HI6250_CLK_CFGBUS_DIV, "clk_cfgbus_div", "clk_sysbus_div", CLK_SET_RATE_PARENT, 0xec, 0, 2, 0x30000, },
//   { HI6250_CLK_MMC0BUS_DIV, "clk_mmc0bus_div", "clk_sysbus_div", CLK_SET_RATE_PARENT, 0xec, 2, 1, 0x40000, },
//   { HI6250_CLK_MMC1BUS_DIV, "clk_mmc1bus_div", "clk_sysbus_div", CLK_SET_RATE_PARENT, 0xec, 3, 1, 0x80000, },
//   { HI6250_CLK_DBGBUS_DIV, "clk_dbgbus_div", "clk_sysbus_div", CLK_SET_RATE_PARENT, 0xec, 12, 1, 0x10000000, },
//   // { HI6250_CLK_TIMESTP_DIV, "clk_timestp_div", "clk_dbgbus_div", CLK_SET_RATE_PARENT, 0x128, 22, 3, 0x1c000000000, },
//   { HI6250_CLK_PERF_DIV, "clk_perf_div", "clk_dbgbus_div", CLK_SET_RATE_PARENT, 0xd0, 14, 2, 0xc0000000, },
//   { HI6250_PCLKDIV_DBG, "pclkdiv_dbg", "clk_cssys_div", CLK_SET_RATE_PARENT, 0x128, 0, 1, 0x10000, },
//   { HI6250_CLKDIV_TRACK, "clkdiv_track", "clk_cssys_div", CLK_SET_RATE_PARENT, 0x128, 12, 2, 0x30000000, },
//   { HI6250_CLK_CSSYS_DIV, "clk_cssys_div", "clk_sysbus_div", CLK_SET_RATE_PARENT, 0xec, 13, 1, 0x20000000, },
//   { HI6250_CLK_DMABUS_DIV, "clk_dmabus_div", "clk_sysbus_div", CLK_SET_RATE_PARENT, 0xec, 15, 1, 0x80000000, },
//   { HI6250_CLK_VIVOBUS_DIV, "clk_vivobus_div", "clk_gate_vivobus_andgt", CLK_SET_RATE_PARENT, 0xd0, 7, 5, 0xf800000, },
//   { HI6250_CLK_VCODECBUS_DIV, "clk_vcodecbus_div", "clk_gate_vcodecbus_andgt", CLK_SET_RATE_PARENT, 0xd0, 0, 5, 0x1f0000, },
//   { HI6250_CLK_MMC0_DIV, "clk_mmc0_div", "clk_andgt_mmc0", CLK_SET_RATE_PARENT, 0xb4, 3, 4, 0x780000, },
//   { HI6250_CLK_MMC1_DIV, "clk_mmc1_div", "clk_andgt_mmc1", CLK_SET_RATE_PARENT, 0xb8, 0, 4, 0xf0000, },
//   { HI6250_CLKDIV_SDIO0, "clkdiv_sdio0", "clk_andgt_sdio0", CLK_SET_RATE_PARENT, 0xb8, 7, 4, 0x7800000, },
//   { HI6250_CLKDIV_UARTH, "clkdiv_uarth", "clk_andgt_uarth", CLK_SET_RATE_PARENT, 0xb0, 7, 4, 0x7800000, },
//   { HI6250_CLKDIV_UARTL, "clkdiv_uartl", "clk_andgt_uartl", CLK_SET_RATE_PARENT, 0xb0, 11, 4, 0x78000000, },
//   { HI6250_CLKDIV_I2C, "clkdiv_i2c", "clk_a53hpm_div", CLK_SET_RATE_PARENT, 0xe8, 4, 4, 0xf00000, },
//   { HI6250_CLKDIV_SPI, "clkdiv_spi", "clk_andgt_spi", CLK_SET_RATE_PARENT, 0xc4, 12, 4, 0xf0000000, },
//   { HI6250_CLK_PTP_DIV, "clk_ptp_div", "clk_a53hpm_div", CLK_SET_RATE_PARENT, 0xdc, 12, 4, 0xf0000000, },
//   { HI6250_CLKDIV_OUT0_PLL, "clkdiv_out0_pll", "clk_andgt_out0", CLK_SET_RATE_PARENT, 0xe0, 4, 6, 0x3f00000, },
//   { HI6250_CLKDIV_OUT1_PLL, "clkdiv_out1_pll", "clk_andgt_out1", CLK_SET_RATE_PARENT, 0xe0, 10, 6, 0xfc000000, },
//   { HI6250_CLKDIV_EDC0, "clkdiv_edc0", "clk_andgt_edc0", CLK_SET_RATE_PARENT, 0xbc, 0, 6, 0x3f0000, },
//   { HI6250_CLKDIV_LDI0, "clkdiv_ldi0", "clk_andgt_ldi0", CLK_SET_RATE_PARENT, 0xbc, 8, 6, 0x3f000000, },
//   { HI6250_CLKDIV_VENC, "clkdiv_venc", "clk_andgt_venc", CLK_SET_RATE_PARENT, 0xc8, 6, 5, 0x7c00000, },
//   { HI6250_CLKDIV_VDEC, "clkdiv_vdec", "clk_andgt_vdec", CLK_SET_RATE_PARENT, 0xcc, 0, 5, 0x1f0000, },
//   { HI6250_CLKDIV_ISPA7, "clkdiv_ispa7", "clk_andgt_ispa7", CLK_SET_RATE_PARENT, 0xd4, 0, 5, 0x1f0000, },
//   { HI6250_CLKDIV_ISPFUNC, "clkdiv_ispfunc", "clk_andgt_ispfunc", CLK_SET_RATE_PARENT, 0xc4, 0, 5, 0x1f0000, },
//   { HI6250_CLK_DIV_ISPSN, "clk_div_ispsn", "clk_fac_ispsn", CLK_SET_RATE_PARENT, 0x108, 0, 2, 0x30000, },
};


// static const char *clk_sysbus_mux_p[] = { "clk_ppll1", "clk_ap_ppll0" };
// static const char *clk_wd0_mux_p[] = { "clkin_ref", "pclk_wd0_high" };
// static const char *clk_vivobus_mux_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll2", "clk_ap_ppll3" };
// static const char *clk_vcodecbus_mux_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll2", "clk_ap_ppll3" };
// static const char *clk_mmc0_muxsys_p[] = { "clk_factor_mmc0", "clk_mmc0_div" };
// static const char *clk_mmc0_muxpll_p[] = { "clk_ap_ppll0", "clk_ap_ppll3" };
// static const char *clk_sd_muxsys_p[] = { "clk_factor_mmc0", "clk_mmc1_div" };
// static const char *clk_sd_muxpll_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll3", "clk_ap_ppll3" };
// static const char *clk_sdio0_muxsy_p[] = { "clk_factor_mmc0", "clkdiv_sdio0" };
// static const char *clk_sdio0_muxpl_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll3", "clk_ap_ppll3" };
// static const char *clk_a53hpm_mux_p[] = { "clk_ap_ppll0", "clk_ppll1" };
// static const char *clkmux_uarth_p[] = { "clkin_sys", "clkdiv_uarth" };
// static const char *clkmux_uartl_p[] = { "clkin_sys", "clkdiv_uartl" };
// static const char *clkmux_i2c_p[] = { "clkin_sys", "clkdiv_i2c" };
// static const char *clkmux_spi_p[] = { "clkin_sys", "clkdiv_spi" };
// static const char *clkmux_timer9_a_p[] = { "clkin_ref", "clk_timer9", "apb_pclk", "apb_pclk" };
// static const char *clkmux_timer9_b_p[] = { "clkin_ref", "clk_timer9", "apb_pclk", "apb_pclk" };
// static const char *clkmux_timer10_a_p[] = { "clkin_ref", "clk_timer10", "apb_pclk", "apb_pclk" };
// static const char *clkmux_timer10_b_p[] = { "clkin_ref", "clk_timer10", "apb_pclk", "apb_pclk" };
// static const char *clkmux_timer11_a_p[] = { "clkin_ref", "clk_timer11", "apb_pclk", "apb_pclk" };
// static const char *clkmux_timer11_b_p[] = { "clkin_ref", "clk_timer11", "apb_pclk", "apb_pclk" };
// static const char *clkmux_timer12_a_p[] = { "clkin_ref", "clk_timer12", "apb_pclk", "apb_pclk" };
// static const char *clkmux_timer12_b_p[] = { "clkin_ref", "clk_timer12", "apb_pclk", "apb_pclk" };
// static const char *clkmux_gps_ref_p[] = { "clk_mdm2gps0", "clk_mdm2gps1" };
// static const char *clkmux_edc0_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll2", "clk_ap_ppll3" };
// static const char *clkmux_ldi0_p[] = { "clk_ppll1", "clk_ap_ppll0", "clk_ap_ppll2", "clk_ap_ppll3" };
// static const char *clkmux_venc_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll3", "clk_ap_ppll3" };
// static const char *clkmux_vdec_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll2", "clk_ap_ppll3" };
// static const char *clkmux_ispa7_p[] = { "clk_ap_ppll0", "clk_ppll1", "clk_ap_ppll3", "clk_ap_ppll3" };
// static const char *clkmux_ispfunc_p[] = { "clk_ap_ppll0", "clk_ap_ppll2", "clk_ap_ppll3", "clk_ap_ppll3" };
// static const char *clk_mux_ispsn_p[] = { "clkin_sys", "clk_div_ispsn" };
// static const char *clk_rxdcfg_mux_p[] = { "clk_rxdcfg_fac", "clkin_sys" };

static const struct hisi_mux_clock hi6250_crgctrl_mux_clks[] = {
  // { HI6250_CLK_SYSBUS_MUX, "clk_sysbus_mux", clk_sysbus_mux_p, ARRAY_SIZE(clk_sysbus_mux_p), CLK_SET_RATE_PARENT, 0xac, 0, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_WD0_MUX, "clk_wd0_mux", clk_wd0_mux_p, ARRAY_SIZE(clk_wd0_mux_p), CLK_SET_RATE_PARENT, 0x140, 17, 1, 0, },
  // { HI6250_CLK_VIVOBUS_MUX, "clk_vivobus_mux", clk_vivobus_mux_p, ARRAY_SIZE(clk_vivobus_mux_p), CLK_SET_RATE_PARENT, 0xd0, 12, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_VCODECBUS_MUX, "clk_vcodecbus_mux", clk_vcodecbus_mux_p, ARRAY_SIZE(clk_vcodecbus_mux_p), CLK_SET_RATE_PARENT, 0xd0, 5, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_MMC0_MUXSYS, "clk_mmc0_muxsys", clk_mmc0_muxsys_p, ARRAY_SIZE(clk_mmc0_muxsys_p), CLK_SET_RATE_PARENT, 0xb4, 2, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_MMC0_MUXPLL, "clk_mmc0_muxpll", clk_mmc0_muxpll_p, ARRAY_SIZE(clk_mmc0_muxpll_p), CLK_SET_RATE_PARENT, 0xb4, 0, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_SD_MUXSYS, "clk_sd_muxsys", clk_sd_muxsys_p, ARRAY_SIZE(clk_sd_muxsys_p), CLK_SET_RATE_PARENT, 0xb8, 6, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_SD_MUXPLL, "clk_sd_muxpll", clk_sd_muxpll_p, ARRAY_SIZE(clk_sd_muxpll_p), CLK_SET_RATE_PARENT, 0xb8, 4, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_SDIO0_MUXSY, "clk_sdio0_muxsy", clk_sdio0_muxsy_p, ARRAY_SIZE(clk_sdio0_muxsy_p), CLK_SET_RATE_PARENT, 0xb8, 13, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_SDIO0_MUXPL, "clk_sdio0_muxpl", clk_sdio0_muxpl_p, ARRAY_SIZE(clk_sdio0_muxpl_p), CLK_SET_RATE_PARENT, 0xb8, 11, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_A53HPM_MUX, "clk_a53hpm_mux", clk_a53hpm_mux_p, ARRAY_SIZE(clk_a53hpm_mux_p), CLK_SET_RATE_PARENT, 0xd4, 9, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_UARTH, "clkmux_uarth", clkmux_uarth_p, ARRAY_SIZE(clkmux_uarth_p), CLK_SET_RATE_PARENT, 0xac, 3, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_UARTL, "clkmux_uartl", clkmux_uartl_p, ARRAY_SIZE(clkmux_uartl_p), CLK_SET_RATE_PARENT, 0xac, 2, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_I2C, "clkmux_i2c", clkmux_i2c_p, ARRAY_SIZE(clkmux_i2c_p), CLK_SET_RATE_PARENT, 0xac, 13, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_SPI, "clkmux_spi", clkmux_spi_p, ARRAY_SIZE(clkmux_spi_p), CLK_SET_RATE_PARENT, 0xac, 8, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_TIMER9_A, "clkmux_timer9_a", clkmux_timer9_a_p, ARRAY_SIZE(clkmux_timer9_a_p), CLK_SET_RATE_PARENT, 0x140, 0, 2, 0, },
  // { HI6250_CLKMUX_TIMER9_B, "clkmux_timer9_b", clkmux_timer9_b_p, ARRAY_SIZE(clkmux_timer9_b_p), CLK_SET_RATE_PARENT, 0x140, 2, 2, 0, },
  // { HI6250_CLKMUX_TIMER10_A, "clkmux_timer10_a", clkmux_timer10_a_p, ARRAY_SIZE(clkmux_timer10_a_p), CLK_SET_RATE_PARENT, 0x140, 4, 2, 0, },
  // { HI6250_CLKMUX_TIMER10_B, "clkmux_timer10_b", clkmux_timer10_b_p, ARRAY_SIZE(clkmux_timer10_b_p), CLK_SET_RATE_PARENT, 0x140, 6, 2, 0, },
  // { HI6250_CLKMUX_TIMER11_A, "clkmux_timer11_a", clkmux_timer11_a_p, ARRAY_SIZE(clkmux_timer11_a_p), CLK_SET_RATE_PARENT, 0x140, 8, 2, 0, },
  // { HI6250_CLKMUX_TIMER11_B, "clkmux_timer11_b", clkmux_timer11_b_p, ARRAY_SIZE(clkmux_timer11_b_p), CLK_SET_RATE_PARENT, 0x140, 10, 2, 0, },
  // { HI6250_CLKMUX_TIMER12_A, "clkmux_timer12_a", clkmux_timer12_a_p, ARRAY_SIZE(clkmux_timer12_a_p), CLK_SET_RATE_PARENT, 0x140, 12, 2, 0, },
  // { HI6250_CLKMUX_TIMER12_B, "clkmux_timer12_b", clkmux_timer12_b_p, ARRAY_SIZE(clkmux_timer12_b_p), CLK_SET_RATE_PARENT, 0x140, 14, 2, 0, },
  // { HI6250_CLKMUX_GPS_REF, "clkmux_gps_ref", clkmux_gps_ref_p, ARRAY_SIZE(clkmux_gps_ref_p), CLK_SET_RATE_PARENT, 0xac, 4, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_EDC0, "clkmux_edc0", clkmux_edc0_p, ARRAY_SIZE(clkmux_edc0_p), CLK_SET_RATE_PARENT, 0xbc, 6, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_LDI0, "clkmux_ldi0", clkmux_ldi0_p, ARRAY_SIZE(clkmux_ldi0_p), CLK_SET_RATE_PARENT, 0xbc, 14, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_VENC, "clkmux_venc", clkmux_venc_p, ARRAY_SIZE(clkmux_venc_p), CLK_SET_RATE_PARENT, 0xc8, 11, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_VDEC, "clkmux_vdec", clkmux_vdec_p, ARRAY_SIZE(clkmux_vdec_p), CLK_SET_RATE_PARENT, 0xcc, 5, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_ISPA7, "clkmux_ispa7", clkmux_ispa7_p, ARRAY_SIZE(clkmux_ispa7_p), CLK_SET_RATE_PARENT, 0xd4, 5, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLKMUX_ISPFUNC, "clkmux_ispfunc", clkmux_ispfunc_p, ARRAY_SIZE(clkmux_ispfunc_p), CLK_SET_RATE_PARENT, 0xc4, 5, 2, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_MUX_ISPSN, "clk_mux_ispsn", clk_mux_ispsn_p, ARRAY_SIZE(clk_mux_ispsn_p), CLK_SET_RATE_PARENT, 0x108, 3, 1, CLK_MUX_HIWORD_MASK, },
  // { HI6250_CLK_RXDCFG_MUX, "clk_rxdcfg_mux", clk_rxdcfg_mux_p, ARRAY_SIZE(clk_rxdcfg_mux_p), CLK_SET_RATE_PARENT, 0xc4, 8, 1, CLK_MUX_HIWORD_MASK, },
};

static void __init hi6250_clk_crg_init(struct device_node *np)
{
	struct hisi_clock_data *clk_data_crg;

	clk_data_crg = hisi_clk_init(np, HI6250_CRGCTRL_NR_CLKS);
	if (!clk_data_crg)
		return;

	hisi_clk_register_fixed_factor(hi6250_crgctrl_fixed_factor_clks,
				ARRAY_SIZE(hi6250_crgctrl_fixed_factor_clks), clk_data_crg);
	hisi_clk_register_gate(hi6250_crgctrl_gate_clks,
				ARRAY_SIZE(hi6250_crgctrl_gate_clks), clk_data_crg);
	hisi_clk_register_gate_sep(hi6250_crgctrl_gate_sep_clks,
				ARRAY_SIZE(hi6250_crgctrl_gate_sep_clks), clk_data_crg);
  hi6220_clk_register_divider(hi6250_crgctrl_divider_clks,
				ARRAY_SIZE(hi6250_crgctrl_divider_clks), clk_data_crg);
  hisi_clk_register_mux(hi6250_crgctrl_mux_clks,
				ARRAY_SIZE(hi6250_crgctrl_mux_clks), clk_data_crg);
}
CLK_OF_DECLARE_DRIVER(hi6250_clk_crgctrl, "hisilicon,hi6250-crgctrl", hi6250_clk_crg_init);
