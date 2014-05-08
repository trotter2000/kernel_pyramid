/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Copyright (c) 2013 Sebastian Sobczyk <sebastiansobczyk@wp.pl>
 * Copyright (c) 2013 bilalliberty <dominos_liberty @ xda-developers>
 * Copyright (c) 2013 Sultanxda <sultanxda@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../../../drivers/video/msm/msm_fb.h"
#include "../../../drivers/video/msm/mipi_dsi.h"
#include "../../../drivers/video/msm/mdp4.h"

#include <mach/gpio.h>
#include <mach/panel_id.h>
#include <mach/msm_bus_board.h>
#include <linux/bootmem.h>
#include <mach/debug_display.h>

#include "devices.h"
#include "board-pyramid.h"

#ifdef CONFIG_FB_MSM_OVERLAY0_WRITEBACK
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE roundup((960 * ALIGN(540, 32) * 3 * 2), 4096)
#else
#define MSM_FB_OVERLAY0_WRITEBACK_SIZE (0)
#endif

#define PANEL_ID_PYD_SHARP	(0x21 | BL_MIPI | IF_MIPI | DEPTH_RGB888)
#define PANEL_ID_PYD_AUO_NT	(0x22 | BL_MIPI | IF_MIPI | DEPTH_RGB888)

#define HDMI_PANEL_NAME "hdmi_msm"

static int msm_fb_detect_panel(const char *name)
{
	if (!strncmp(name, HDMI_PANEL_NAME,
			strnlen(HDMI_PANEL_NAME,
				PANEL_NAME_MAX_LEN))) {
		return 0;
	}

	return -ENODEV;
}

static struct resource msm_fb_resources[] = {
	{
		.flags  = IORESOURCE_DMA,
	}
};

static struct msm_fb_platform_data msm_fb_pdata = {
	.detect_client = msm_fb_detect_panel,
};

static struct platform_device msm_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources     = ARRAY_SIZE(msm_fb_resources),
	.resource          = msm_fb_resources,
	.dev.platform_data = &msm_fb_pdata,
};

void __init pyramid_allocate_fb_region(void)
{
	void *addr;
	unsigned long size;

	size = MSM_FB_SIZE;
	addr = alloc_bootmem_align(size, 0x1000);
	msm_fb_resources[0].start = __pa(addr);
	msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
	pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
			size, addr, __pa(addr));
}

#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors mdp_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors mdp_sd_smi_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 147460000,
		.ib = 184325000,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors mdp_sd_ebi_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 168652800,
		.ib = 337305600,
	},
};

static struct msm_bus_vectors mdp_vga_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 37478400,
		.ib = 74956800,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 206131200,
		.ib = 412262400,
	},
};

static struct msm_bus_vectors mdp_720p_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 112435200,
		.ib = 224870400,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 281088000,
		.ib = 562176000,
	},
};

static struct msm_bus_vectors mdp_1080p_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 252979200,
		.ib = 505958400,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 421632000,
		.ib = 843264000,
	},
};

static struct msm_bus_paths mdp_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(mdp_init_vectors),
		mdp_init_vectors,
	},
	{
		ARRAY_SIZE(mdp_sd_smi_vectors),
		mdp_sd_smi_vectors,
	},
	{
		ARRAY_SIZE(mdp_sd_ebi_vectors),
		mdp_sd_ebi_vectors,
	},
	{
		ARRAY_SIZE(mdp_vga_vectors),
		mdp_vga_vectors,
	},
	{
		ARRAY_SIZE(mdp_720p_vectors),
		mdp_720p_vectors,
	},
	{
		ARRAY_SIZE(mdp_1080p_vectors),
		mdp_1080p_vectors,
	},
};

static struct msm_bus_scale_pdata mdp_bus_scale_pdata = {
	mdp_bus_scale_usecases,
	ARRAY_SIZE(mdp_bus_scale_usecases),
	.name = "mdp",
};

#ifdef CONFIG_FB_MSM_DTV
static struct msm_bus_vectors dtv_bus_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 0,
		.ib = 0,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors dtv_bus_def_vectors[] = {
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_SMI,
		.ab = 566092800 *2,
		.ib = 707616000 *2,
	},
	{
		.src = MSM_BUS_MASTER_MDP_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 566092800 *2,
		.ib = 707616000 *2,
	},
};

static struct msm_bus_paths dtv_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(dtv_bus_init_vectors),
		dtv_bus_init_vectors,
	},
	{
		ARRAY_SIZE(dtv_bus_def_vectors),
		dtv_bus_def_vectors,
	},
};

static struct msm_bus_scale_pdata dtv_bus_scale_pdata = {
	dtv_bus_scale_usecases,
	ARRAY_SIZE(dtv_bus_scale_usecases),
	.name = "dtv",
};

static struct lcdc_platform_data dtv_pdata = {
	.bus_scale_table = &dtv_bus_scale_pdata,
};
#endif
#endif

struct mdp_table_entry mdp_gamma_jdi[] = {
        {0x94800, 0x000000, 0x0},
        {0x94804, 0x000100, 0x0},
        {0x94808, 0x010201, 0x0},
        {0x9480C, 0x020302, 0x0},
        {0x94810, 0x030403, 0x0},
        {0x94814, 0x040504, 0x0},
        {0x94818, 0x050605, 0x0},
        {0x9481C, 0x060706, 0x0},
        {0x94820, 0x070807, 0x0},
        {0x94824, 0x080908, 0x0},
        {0x94828, 0x090A09, 0x0},
        {0x9482C, 0x0A0B0A, 0x0},
        {0x94830, 0x0B0C0B, 0x0},
        {0x94834, 0x0B0D0C, 0x0},
        {0x94838, 0x0C0E0D, 0x0},
        {0x9483C, 0x0D0F0E, 0x0},
        {0x94840, 0x0E100F, 0x0},
        {0x94844, 0x0F1110, 0x0},
        {0x94848, 0x101210, 0x0},
        {0x9484C, 0x111311, 0x0},
        {0x94850, 0x121412, 0x0},
        {0x94854, 0x131513, 0x0},
        {0x94858, 0x141614, 0x0},
        {0x9485C, 0x151715, 0x0},
        {0x94860, 0x161816, 0x0},
        {0x94864, 0x161917, 0x0},
        {0x94868, 0x171A18, 0x0},
        {0x9486C, 0x181B19, 0x0},
        {0x94870, 0x191C1A, 0x0},
        {0x94874, 0x1A1D1B, 0x0},
        {0x94878, 0x1B1E1C, 0x0},
        {0x9487C, 0x1C1F1D, 0x0},
        {0x94880, 0x1D201E, 0x0},
        {0x94884, 0x1E211F, 0x0},
        {0x94888, 0x1F2220, 0x0},
        {0x9488C, 0x202320, 0x0},
        {0x94890, 0x212421, 0x0},
        {0x94894, 0x212522, 0x0},
        {0x94898, 0x222623, 0x0},
        {0x9489C, 0x232724, 0x0},
        {0x948A0, 0x242825, 0x0},
        {0x948A4, 0x252926, 0x0},
        {0x948A8, 0x262A27, 0x0},
        {0x948AC, 0x272B28, 0x0},
        {0x948B0, 0x282C29, 0x0},
        {0x948B4, 0x292D2A, 0x0},
        {0x948B8, 0x2A2E2B, 0x0},
        {0x948BC, 0x2B2F2C, 0x0},
        {0x948C0, 0x2C302D, 0x0},
        {0x948C4, 0x2C312E, 0x0},
        {0x948C8, 0x2D322F, 0x0},
        {0x948CC, 0x2E3330, 0x0},
        {0x948D0, 0x2F3430, 0x0},
        {0x948D4, 0x303531, 0x0},
        {0x948D8, 0x313632, 0x0},
        {0x948DC, 0x323733, 0x0},
        {0x948E0, 0x333834, 0x0},
        {0x948E4, 0x343935, 0x0},
        {0x948E8, 0x353A36, 0x0},
        {0x948EC, 0x363B37, 0x0},
        {0x948F0, 0x373C38, 0x0},
        {0x948F4, 0x373D39, 0x0},
        {0x948F8, 0x383E3A, 0x0},
        {0x948FC, 0x393F3B, 0x0},
        {0x94900, 0x3A403C, 0x0},
        {0x94904, 0x3B413D, 0x0},
        {0x94908, 0x3C423E, 0x0},
        {0x9490C, 0x3D433F, 0x0},
        {0x94910, 0x3E4440, 0x0},
        {0x94914, 0x3F4540, 0x0},
        {0x94918, 0x404641, 0x0},
        {0x9491C, 0x414742, 0x0},
        {0x94920, 0x424843, 0x0},
        {0x94924, 0x424944, 0x0},
        {0x94928, 0x434A45, 0x0},
        {0x9492C, 0x444B46, 0x0},
        {0x94930, 0x454C47, 0x0},
        {0x94934, 0x464D48, 0x0},
        {0x94938, 0x474E49, 0x0},
        {0x9493C, 0x484F4A, 0x0},
        {0x94940, 0x49504B, 0x0},
        {0x94944, 0x4A514C, 0x0},
        {0x94948, 0x4B524D, 0x0},
        {0x9494C, 0x4C534E, 0x0},
        {0x94950, 0x4D544F, 0x0},
        {0x94954, 0x4E5550, 0x0},
        {0x94958, 0x4E5650, 0x0},
        {0x9495C, 0x4F5751, 0x0},
        {0x94960, 0x505852, 0x0},
        {0x94964, 0x515953, 0x0},
        {0x94968, 0x525A54, 0x0},
        {0x9496C, 0x535B55, 0x0},
        {0x94970, 0x545C56, 0x0},
        {0x94974, 0x555D57, 0x0},
        {0x94978, 0x565E58, 0x0},
        {0x9497C, 0x575F59, 0x0},
        {0x94980, 0x58605A, 0x0},
        {0x94984, 0x59615B, 0x0},
        {0x94988, 0x59625C, 0x0},
        {0x9498C, 0x5A635D, 0x0},
        {0x94990, 0x5B645E, 0x0},
        {0x94994, 0x5C655F, 0x0},
        {0x94998, 0x5D6660, 0x0},
        {0x9499C, 0x5E6760, 0x0},
        {0x949A0, 0x5F6861, 0x0},
        {0x949A4, 0x606962, 0x0},
        {0x949A8, 0x616A63, 0x0},
        {0x949AC, 0x626B64, 0x0},
        {0x949B0, 0x636C65, 0x0},
        {0x949B4, 0x646D66, 0x0},
        {0x949B8, 0x646E67, 0x0},
        {0x949BC, 0x656F68, 0x0},
        {0x949C0, 0x667069, 0x0},
        {0x949C4, 0x67716A, 0x0},
        {0x949C8, 0x68726B, 0x0},
        {0x949CC, 0x69736C, 0x0},
        {0x949D0, 0x6A746D, 0x0},
        {0x949D4, 0x6B756E, 0x0},
        {0x949D8, 0x6C766F, 0x0},
        {0x949DC, 0x6D7770, 0x0},
        {0x949E0, 0x6E7870, 0x0},
        {0x949E4, 0x6F7971, 0x0},
        {0x949E8, 0x6F7A72, 0x0},
        {0x949EC, 0x707B73, 0x0},
        {0x949F0, 0x717C74, 0x0},
        {0x949F4, 0x727D75, 0x0},
        {0x949F8, 0x737E76, 0x0},
        {0x949FC, 0x747F77, 0x0},
        {0x94A00, 0x758078, 0x0},
        {0x94A04, 0x768179, 0x0},
        {0x94A08, 0x77827A, 0x0},
        {0x94A0C, 0x78837B, 0x0},
        {0x94A10, 0x79847C, 0x0},
        {0x94A14, 0x7A857D, 0x0},
        {0x94A18, 0x7A867E, 0x0},
        {0x94A1C, 0x7B877F, 0x0},
        {0x94A20, 0x7C8880, 0x0},
        {0x94A24, 0x7D8980, 0x0},
        {0x94A28, 0x7E8A81, 0x0},
        {0x94A2C, 0x7F8B82, 0x0},
        {0x94A30, 0x808C83, 0x0},
        {0x94A34, 0x818D84, 0x0},
        {0x94A38, 0x828E85, 0x0},
        {0x94A3C, 0x838F86, 0x0},
        {0x94A40, 0x849087, 0x0},
        {0x94A44, 0x859188, 0x0},
        {0x94A48, 0x859289, 0x0},
        {0x94A4C, 0x86938A, 0x0},
        {0x94A50, 0x87948B, 0x0},
        {0x94A54, 0x88958C, 0x0},
        {0x94A58, 0x89968D, 0x0},
        {0x94A5C, 0x8A978E, 0x0},
        {0x94A60, 0x8B988F, 0x0},
        {0x94A64, 0x8C9990, 0x0},
        {0x94A68, 0x8D9A90, 0x0},
        {0x94A6C, 0x8E9B91, 0x0},
        {0x94A70, 0x8F9C92, 0x0},
        {0x94A74, 0x909D93, 0x0},
        {0x94A78, 0x909E94, 0x0},
        {0x94A7C, 0x919F95, 0x0},
        {0x94A80, 0x92A096, 0x0},
        {0x94A84, 0x93A197, 0x0},
        {0x94A88, 0x94A298, 0x0},
        {0x94A8C, 0x95A399, 0x0},
        {0x94A90, 0x96A49A, 0x0},
        {0x94A94, 0x97A59B, 0x0},
        {0x94A98, 0x98A69C, 0x0},
        {0x94A9C, 0x99A79D, 0x0},
        {0x94AA0, 0x9AA89E, 0x0},
        {0x94AA4, 0x9BA99F, 0x0},
        {0x94AA8, 0x9CAAA0, 0x0},
        {0x94AAC, 0x9CABA0, 0x0},
        {0x94AB0, 0x9DACA1, 0x0},
        {0x94AB4, 0x9EADA2, 0x0},
        {0x94AB8, 0x9FAEA3, 0x0},
        {0x94ABC, 0xA0AFA4, 0x0},
        {0x94AC0, 0xA1B0A5, 0x0},
        {0x94AC4, 0xA2B1A6, 0x0},
        {0x94AC8, 0xA3B2A7, 0x0},
        {0x94ACC, 0xA4B3A8, 0x0},
        {0x94AD0, 0xA5B4A9, 0x0},
        {0x94AD4, 0xA6B5AA, 0x0},
        {0x94AD8, 0xA7B6AB, 0x0},
        {0x94ADC, 0xA7B7AC, 0x0},
        {0x94AE0, 0xA8B8AD, 0x0},
        {0x94AE4, 0xA9B9AE, 0x0},
        {0x94AE8, 0xAABAAF, 0x0},
        {0x94AEC, 0xABBBB0, 0x0},
        {0x94AF0, 0xACBCB0, 0x0},
        {0x94AF4, 0xADBDB1, 0x0},
        {0x94AF8, 0xAEBEB2, 0x0},
        {0x94AFC, 0xAFBFB3, 0x0},
        {0x94B00, 0xB0C0B4, 0x0},
        {0x94B04, 0xB1C1B5, 0x0},
        {0x94B08, 0xB2C2B6, 0x0},
        {0x94B0C, 0xB2C3B7, 0x0},
        {0x94B10, 0xB3C4B8, 0x0},
        {0x94B14, 0xB4C5B9, 0x0},
        {0x94B18, 0xB5C6BA, 0x0},
        {0x94B1C, 0xB6C7BB, 0x0},
        {0x94B20, 0xB7C8BC, 0x0},
        {0x94B24, 0xB8C9BD, 0x0},
        {0x94B28, 0xB9CABE, 0x0},
        {0x94B2C, 0xBACBBF, 0x0},
        {0x94B30, 0xBBCCC0, 0x0},
        {0x94B34, 0xBCCDC0, 0x0},
        {0x94B38, 0xBDCEC1, 0x0},
        {0x94B3C, 0xBDCFC2, 0x0},
        {0x94B40, 0xBED0C3, 0x0},
        {0x94B44, 0xBFD1C4, 0x0},
        {0x94B48, 0xC0D2C5, 0x0},
        {0x94B4C, 0xC1D3C6, 0x0},
        {0x94B50, 0xC2D4C7, 0x0},
        {0x94B54, 0xC3D5C8, 0x0},
        {0x94B58, 0xC4D6C9, 0x0},
        {0x94B5C, 0xC5D7CA, 0x0},
        {0x94B60, 0xC6D8CB, 0x0},
        {0x94B64, 0xC7D9CC, 0x0},
        {0x94B68, 0xC8DACD, 0x0},
        {0x94B6C, 0xC8DBCE, 0x0},
        {0x94B70, 0xC9DCCF, 0x0},
        {0x94B74, 0xCADDD0, 0x0},
        {0x94B78, 0xCBDED0, 0x0},
        {0x94B7C, 0xCCDFD1, 0x0},
        {0x94B80, 0xCDE0D2, 0x0},
        {0x94B84, 0xCEE1D3, 0x0},
        {0x94B88, 0xCFE2D4, 0x0},
        {0x94B8C, 0xD0E3D5, 0x0},
        {0x94B90, 0xD1E4D6, 0x0},
        {0x94B94, 0xD2E5D7, 0x0},
        {0x94B98, 0xD3E6D8, 0x0},
        {0x94B9C, 0xD3E7D9, 0x0},
        {0x94BA0, 0xD4E8DA, 0x0},
        {0x94BA4, 0xD5E9DB, 0x0},
        {0x94BA8, 0xD6EADC, 0x0},
        {0x94BAC, 0xD7EBDD, 0x0},
        {0x94BB0, 0xD8ECDE, 0x0},
        {0x94BB4, 0xD9EDDF, 0x0},
        {0x94BB8, 0xDAEEE0, 0x0},
        {0x94BBC, 0xDBEFE0, 0x0},
        {0x94BC0, 0xDCF0E1, 0x0},
        {0x94BC4, 0xDDF1E2, 0x0},
        {0x94BC8, 0xDEF2E3, 0x0},
        {0x94BCC, 0xDEF3E4, 0x0},
        {0x94BD0, 0xDFF4E5, 0x0},
        {0x94BD4, 0xE0F5E6, 0x0},
        {0x94BD8, 0xE1F6E7, 0x0},
        {0x94BDC, 0xE2F7E8, 0x0},
        {0x94BE0, 0xE3F8E9, 0x0},
        {0x94BE4, 0xE4F9EA, 0x0},
        {0x94BE8, 0xE5FAEB, 0x0},
        {0x94BEC, 0xE6FBEC, 0x0},
        {0x94BF0, 0xE7FCED, 0x0},
        {0x94BF4, 0xE8FDEE, 0x0},
        {0x94BF8, 0xE9FEEF, 0x0},
        {0x94BFC, 0xEAFFF0, 0x0},
        {0x90070, 0x0F, 0x0},
};

int pyramid_mdp_gamma(void)
{
	mdp_color_enhancement(mdp_gamma_jdi, ARRAY_SIZE(mdp_gamma_jdi));

	return 0;
}

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = GPIO_LCD_TE,
	.mdp_max_clk = 200000000,
	.mdp_max_bw = 2000000000,
	.mdp_bw_ab_factor = 115,
	.mdp_bw_ib_factor = 150,
#ifdef CONFIG_MSM_BUS_SCALING
	.mdp_bus_scale_table = &mdp_bus_scale_pdata,
#endif
	.mdp_rev = MDP_REV_41,
	.mem_hid = BIT(ION_CP_WB_HEAP_ID),
	.mdp_iommu_split_domain = 0,
	.mdp_gamma = pyramid_mdp_gamma,
};

void __init pyramid_mdp_writeback(struct memtype_reserve* reserve_table)
{
	mdp_pdata.ov0_wb_size = MSM_FB_OVERLAY0_WRITEBACK_SIZE;
}

/*
 * Regulator initialization moved to mipi_dsi
 */

static struct mipi_dsi_platform_data mipi_dsi_pdata = {
	.vsync_gpio = GPIO_LCD_TE,
};

static struct dsi_buf panel_tx_buf;
static struct dsi_buf panel_rx_buf;

static char led_pwm1[] = {0x51, 0x0};
static char sw_reset[2] = {0x01, 0x00};
static char enter_sleep[2] = {0x10, 0x00};
static char exit_sleep[2] = {0x11, 0x00};
static char display_off[2] = {0x28, 0x00};
static char display_on[2] = {0x29, 0x00};
static char enable_te[2] = {0x35, 0x00};
static char test_reg[3] = {0x44, 0x02, 0xCF};
static char test_reg_ruy_auo[3] = {0x44, 0x01, 0x68};
static char set_twolane[2] = {0xae, 0x03};
static char rgb_888[2] = {0x3A, 0x77};
static char novatek_f4[2] = {0xf4, 0x55};
static char novatek_8c[16] = {
	0x8C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x08, 0x00, 0x30, 0xC0, 0xB7, 0x37};
static char novatek_ff[2] = {0xff, 0x55 };
static char set_width[5] = {
	0x2A, 0x00, 0x00, 0x02, 0x1B};
static char set_height[5] = {
	0x2B, 0x00, 0x00, 0x03, 0xBF};
static char novatek_pwm_f3[2] = {0xF3, 0xAA };
static char novatek_pwm_00[2] = {0x00, 0x01 };
static char novatek_pwm_21[2] = {0x21, 0x20 };
static char novatek_pwm_22[2] = {0x22, 0x03 };
static char novatek_pwm_7d[2] = {0x7D, 0x01 };
static char novatek_pwm_7f[2] = {0x7F, 0xAA };
static char novatek_pwm_cp[2] = {0x09, 0x34 };
static char novatek_pwm_cp2[2] = {0xc9, 0x01 };
static char novatek_pwm_cp3[2] = {0xff, 0xaa };
static char max_pktsize[2] =  {MIPI_DSI_MRPS, 0x00};
static unsigned char bkl_enable_cmds[] = {0x53, 0x24};

static char pyd_sharp_gm[] = {
	0xf3, 0xaa,
	
	0x24, 0x00, 0x25, 0x04, 0x26, 0x11, 0x27, 0x1c, 0x28, 0x1a, 0x29, 0x2e,
	0x2a, 0x5e, 0x2b, 0x21, 0x2d, 0x1f, 0x2f, 0x27, 0x30, 0x60, 0x31, 0x15,
	0x32, 0x3e, 0x33, 0x5f, 0x34, 0x7c, 0x35, 0x86, 0x36, 0x87, 0x37, 0x08,

	0x38, 0x01, 0x39, 0x06, 0x3a, 0x14, 0x3b, 0x21, 0x3d, 0x1a, 0x3f, 0x2d,
	0x40, 0x5f, 0x41, 0x33, 0x42, 0x20, 0x43, 0x27, 0x44, 0x7b, 0x45, 0x15,
	0x46, 0x3e, 0x47, 0x5f, 0x48, 0xa7, 0x49, 0xb3, 0x4a, 0xb4, 0x4b, 0x35,

	0x4c, 0x2a, 0x4d, 0x2d, 0x4e, 0x36, 0x4f, 0x3e, 0x50, 0x18, 0x51, 0x2a,
	0x52, 0x5c, 0x53, 0x2c, 0x54, 0x1d, 0x55, 0x25, 0x56, 0x65, 0x57, 0x12,
	0x58, 0x3a, 0x59, 0x57, 0x5a, 0x93, 0x5b, 0xb2, 0x5c, 0xb6, 0x5d, 0x37,

	0x5e, 0x30, 0x5f, 0x34, 0x60, 0x3e, 0x61, 0x46, 0x62, 0x19, 0x63, 0x2b,
	0x64, 0x5c, 0x65, 0x3f, 0x66, 0x1f, 0x67, 0x26, 0x68, 0x80, 0x69, 0x13,
	0x6a, 0x3c, 0x6b, 0x57, 0x6c, 0xc0, 0x6d, 0xe2, 0x6e, 0xe7, 0x6f, 0x68,

	0x70, 0x00, 0x71, 0x0a, 0x72, 0x26, 0x73, 0x37, 0x74, 0x1e, 0x75, 0x32,
	0x76, 0x60, 0x77, 0x32, 0x78, 0x1f, 0x79, 0x26, 0x7a, 0x68, 0x7b, 0x14,
	0x7c, 0x39, 0x7d, 0x59, 0x7e, 0x85, 0x7f, 0x86, 0x80, 0x87, 0x81, 0x08,

	0x82, 0x01, 0x83, 0x0c, 0x84, 0x2b, 0x85, 0x3e, 0x86, 0x1f, 0x87, 0x33,
	0x88, 0x61, 0x89, 0x45, 0x8a, 0x1f, 0x8b, 0x26, 0x8c, 0x84, 0x8d, 0x14,
	0x8e, 0x3a, 0x8f, 0x59, 0x90, 0xb1, 0x91, 0xb3, 0x92, 0xb4, 0x93, 0x35,
	
	0xc9, 0x01,
	0xff, 0xaa,
};

static struct dsi_cmd_desc pyd_sharp_cmd_on_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 8},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 20},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 24},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 28},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 30},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 32},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 34},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 38},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 40},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 42},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 44},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 46},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 48},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 50},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 52},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 54},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 56},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 58},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 60},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 62},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 64},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 66},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 68},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 70},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 72},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 74},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 76},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 78},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 80},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 82},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 84},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 86},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 88},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 90},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 92},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 94},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 96},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 98},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 130},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 136},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 138},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 140},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 142},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 144},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 146},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 148},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 150},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 152},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 154},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 156},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 158},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 160},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 162},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 164},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 166},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 168},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 170},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 172},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 174},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 176},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 178},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 180},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 182},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 184},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 186},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 188},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 190},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 192},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 194},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 196},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 198},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 200},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 202},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 204},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 206},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 208},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 210},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 212},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 214},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 216},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 218},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_sharp_gm + 220},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_00), novatek_pwm_00},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_21), novatek_pwm_21},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_22), novatek_pwm_22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7d), novatek_pwm_7d},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7f), novatek_pwm_7f},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp), novatek_pwm_cp},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp2), novatek_pwm_cp2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp3), novatek_pwm_cp3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg), test_reg},
	{DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_f4), novatek_f4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(novatek_8c), novatek_8c},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_ff), novatek_ff},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(set_twolane), set_twolane},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(rgb_888), rgb_888},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(bkl_enable_cmds), bkl_enable_cmds},
        {DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};

static char pyd_auo_gm[] = {
	0xf3, 0xaa,

	0x24, 0X63, 0X25, 0X6B, 0X26, 0X78, 0X27, 0X7E, 0X28, 0X19, 0X29, 0X2E,
	0x2A, 0X61, 0X2B, 0X61, 0X2D, 0X1b, 0X2F, 0X22, 0X30, 0X84, 0X31, 0X1B,
	0x32, 0X4F, 0X33, 0X63, 0X34, 0X28, 0X35, 0XDF, 0X36, 0XC9, 0X37, 0X69,

	0x38, 0X63, 0X39, 0X6B, 0X3A, 0X78, 0X3B, 0X7E, 0X3D, 0X19, 0X3F, 0X2E,
	0x40, 0X61, 0X41, 0X61, 0X42, 0X1b, 0X43, 0X22, 0X44, 0X84, 0X45, 0X1B,
	0x46, 0X4F, 0X47, 0X63, 0X48, 0XC7, 0X49, 0XDF, 0X4A, 0XC9, 0X4B, 0X69,

	0x4C, 0X45, 0X4D, 0X54, 0X4E, 0X64, 0X4F, 0X75, 0X50, 0X18, 0X51, 0X2E,
	0x52, 0X62, 0X53, 0X61, 0X54, 0X1D, 0X55, 0X26, 0X56, 0X9D, 0X57, 0X10,
	0x58, 0X39, 0X59, 0X55, 0X5A, 0XC3, 0X5B, 0XD7, 0X5C, 0XFF, 0X5D, 0X6B,

	0x5E, 0X45, 0X5F, 0X54, 0X60, 0X64, 0X61, 0X75, 0X62, 0X18, 0X63, 0X2E,
	0x64, 0X62, 0X65, 0X61, 0X66, 0X1D, 0X67, 0X26, 0X68, 0X65, 0X69, 0X10,
	0x6A, 0X39, 0X6B, 0X55, 0X6C, 0XC3, 0X6D, 0XD7, 0X6E, 0XFF, 0X6F, 0X6B,

	0x70, 0X7D, 0X71, 0X82, 0X72, 0X89, 0X73, 0X97, 0X74, 0X19, 0X75, 0X2E,
	0x76, 0X61, 0X77, 0X6E, 0X78, 0X1A, 0X79, 0X1E, 0X7A, 0X8E, 0X7B, 0X0C,
	0x7C, 0X27, 0X7D, 0X58, 0X7E, 0XCF, 0X7F, 0XD9, 0X80, 0XFc, 0X81, 0X68,

	0x82, 0X7D, 0X83, 0X82, 0X84, 0X89, 0X85, 0X97, 0X86, 0X19, 0X87, 0X2E,
	0x88, 0X61, 0X89, 0X6E, 0X8A, 0X1A, 0X8B, 0X1E, 0X8C, 0X8E, 0X8D, 0X0C,
	0x8E, 0X27, 0X8F, 0X58, 0X90, 0XCF, 0X91, 0XD9, 0X92, 0XFc, 0X93, 0X68,
	
	0xC9, 0x01,
	0xff, 0xaa,
};

static struct dsi_cmd_desc pyd_auo_cmd_on_cmds[] = {
    {DTYPE_DCS_WRITE, 1, 0, 0, 10,
		sizeof(sw_reset), sw_reset},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xf3, 0xaa} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA3, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA4, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA5, 0xFF} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xA6, 0x01} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, (char[]){0xFF, 0xAA} },
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 2},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 8},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 18},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 20},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 22},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 24},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 26},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 28},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 30},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 32},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 34},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 38},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 40},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 42},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 44},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 46},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 48},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 50},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 52},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 54},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 56},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 58},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 60},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 62},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 64},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 66},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 68},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 70},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 72},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 74},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 76},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 78},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 80},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 82},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 84},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 86},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 88},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 90},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 92},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 94},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 96},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 98},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 130},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 136},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 138},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 140},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 142},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 144},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 146},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 148},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 150},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 152},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 154},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 156},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 158},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 160},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 162},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 164},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 166},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 168},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 170},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 172},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 174},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 176},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 178},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 180},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 182},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 184},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 186},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 188},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 190},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 192},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 194},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 196},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 198},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 200},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 202},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 204},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 206},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 208},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 210},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 212},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 214},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 216},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 218},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, 2, pyd_auo_gm + 220},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
			sizeof(exit_sleep), exit_sleep},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_00), novatek_pwm_00},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_21), novatek_pwm_21},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_22), novatek_pwm_22},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7d), novatek_pwm_7d},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_7f), novatek_pwm_7f},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_f3), novatek_pwm_f3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp), novatek_pwm_cp},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp2), novatek_pwm_cp2},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(novatek_pwm_cp3), novatek_pwm_cp3},
    {DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(enable_te), enable_te},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(test_reg_ruy_auo), test_reg_ruy_auo},
    {DTYPE_MAX_PKTSIZE, 1, 0, 0, 0,
		sizeof(max_pktsize), max_pktsize},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_width), set_width},
    {DTYPE_DCS_LWRITE, 1, 0, 0, 0,
		sizeof(set_height), set_height},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(bkl_enable_cmds), bkl_enable_cmds},
	{DTYPE_DCS_WRITE, 1, 0, 0, 0,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc novatek_display_off_cmds[] = {
		{DTYPE_DCS_WRITE, 1, 0, 0, 0,
			sizeof(display_off), display_off},
		{DTYPE_DCS_WRITE, 1, 0, 0, 110,
			sizeof(enter_sleep), enter_sleep}
};

static struct dsi_cmd_desc novatek_cmd_backlight_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0,
		sizeof(led_pwm1), led_pwm1},
};

static int mipi_lcd_on = 1;
static struct dcs_cmd_req cmdreq;

static int pyramid_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if (mipi_lcd_on)
		return 0;

	switch (panel_type) {
		case PANEL_ID_PYD_SHARP:
			printk(KERN_INFO "pyramid_lcd_on PANEL_ID_PYD_SHARP\n");
			cmdreq.cmds = pyd_sharp_cmd_on_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(pyd_sharp_cmd_on_cmds);
			cmdreq.flags = CMD_REQ_COMMIT | CMD_CLK_CTRL;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;

			mipi_dsi_cmdlist_put(&cmdreq);
			break;
		case PANEL_ID_PYD_AUO_NT:
			printk(KERN_INFO "pyramid_lcd_on PANEL_ID_PYD_AUO_NT\n");
			cmdreq.cmds = pyd_auo_cmd_on_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(pyd_auo_cmd_on_cmds);
			cmdreq.flags = CMD_REQ_COMMIT | CMD_CLK_CTRL;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;

			mipi_dsi_cmdlist_put(&cmdreq);
			break;
		default:
			PR_DISP_ERR("%s: panel_type is not supported!(%d)\n", __func__, panel_type);
			break;
	}

	mipi_lcd_on = 1;

	return 0;
}

static int pyramid_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if (!mipi_lcd_on)
		return 0;

	switch (panel_type) {
		case PANEL_ID_PYD_SHARP:
			cmdreq.cmds = novatek_display_off_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(novatek_display_off_cmds);
			cmdreq.flags = CMD_REQ_COMMIT | CMD_CLK_CTRL;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;

			mipi_dsi_cmdlist_put(&cmdreq);
			break;
		case PANEL_ID_PYD_AUO_NT:
			cmdreq.cmds = novatek_display_off_cmds;
			cmdreq.cmds_cnt = ARRAY_SIZE(novatek_display_off_cmds);
			cmdreq.flags = CMD_REQ_COMMIT | CMD_CLK_CTRL;
			cmdreq.rlen = 0;
			cmdreq.cb = NULL;

			mipi_dsi_cmdlist_put(&cmdreq);
			break;
	}

	mipi_lcd_on = 0;

	return 0;
}

#define BRI_SETTING_MIN                 30
#define BRI_SETTING_DEF                 143
#define BRI_SETTING_MAX                 255

#define SHARP_PWM_MIN                   9	/* 3.5% of max pwm */
#define SHARP_PWM_DEFAULT               69	/* 27% of max pwm  */
#define SHARP_PWM_MAX                   194	/* 76% of max pwm */

static unsigned char pyd_shp_shrink_pwm(int br)
{
	unsigned char shrink_br = BRI_SETTING_MAX;

	if (br <= 0) {
		shrink_br = 0;
	} else if (br > 0 && br <= BRI_SETTING_MIN) {
		shrink_br = SHARP_PWM_MIN;
	} else if (br > BRI_SETTING_MIN && br <= BRI_SETTING_DEF) {
		shrink_br = (SHARP_PWM_MIN + (br - BRI_SETTING_MIN) *
				(SHARP_PWM_DEFAULT - SHARP_PWM_MIN) /
				(BRI_SETTING_DEF - BRI_SETTING_MIN));
	} else if (br > BRI_SETTING_DEF && br <= BRI_SETTING_MAX) {
		shrink_br = (SHARP_PWM_DEFAULT + (br - BRI_SETTING_DEF) *
				(SHARP_PWM_MAX - SHARP_PWM_DEFAULT) /
				(BRI_SETTING_MAX - BRI_SETTING_DEF));
	} else if (br > BRI_SETTING_MAX)
		shrink_br = SHARP_PWM_MAX;

	return shrink_br;
}

#define AUO_PWM_MIN                     9
#define AUO_PWM_DEFAULT                 87
#define AUO_PWM_MAX                     255

static unsigned char pyd_auo_shrink_pwm(int br)
{
	unsigned char shrink_br = 0;

	if (br <= 0) {
		shrink_br = 0;
	} else if (br > 0 && br <= BRI_SETTING_MIN) {
		shrink_br = AUO_PWM_MIN;
	} else if (br > BRI_SETTING_MIN && br <= BRI_SETTING_DEF) {
		shrink_br = (AUO_PWM_MIN + (br - BRI_SETTING_MIN) *
				(AUO_PWM_DEFAULT - AUO_PWM_MIN) /
				(BRI_SETTING_DEF - BRI_SETTING_MIN));
	} else if (br > BRI_SETTING_DEF && br <= BRI_SETTING_MAX) {
		shrink_br = (AUO_PWM_DEFAULT + (br - BRI_SETTING_DEF) *
				(AUO_PWM_MAX - AUO_PWM_DEFAULT) /
				(BRI_SETTING_MAX - BRI_SETTING_DEF));
	} else if (br > BRI_SETTING_MAX)
		shrink_br = AUO_PWM_MAX;

	return shrink_br;
}

static void pyramid_set_backlight(struct msm_fb_data_type *mfd)
{
	if (panel_type == PANEL_ID_PYD_SHARP) {
		led_pwm1[1] = pyd_shp_shrink_pwm((unsigned char)(mfd->bl_level));
	}
	else if (panel_type == PANEL_ID_PYD_AUO_NT) {
		led_pwm1[1] = pyd_auo_shrink_pwm((unsigned char)(mfd->bl_level));
	}
	else {
		led_pwm1[1] = (unsigned char)(mfd->bl_level);
	}

	cmdreq.cmds = novatek_cmd_backlight_cmds;
	cmdreq.cmds_cnt = ARRAY_SIZE(novatek_cmd_backlight_cmds);
	cmdreq.flags = CMD_REQ_COMMIT | CMD_CLK_CTRL;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

	mipi_dsi_cmdlist_put(&cmdreq);
}

static int __devinit pyramid_lcd_probe(struct platform_device *pdev)
{
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = pyramid_lcd_probe,
	.driver = {
		.name   = "mipi_novatek",
	},
};

static struct msm_fb_panel_data pyramid_panel_data = {
	.on	       = pyramid_lcd_on,
	.off	       = pyramid_lcd_off,
	.set_backlight = pyramid_set_backlight,
};

static struct msm_panel_info pinfo;
static int ch_used[3] = {0, 0, 0};

static int mipi_pyramid_device_register(const char* dev_name, struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc(dev_name, (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	pyramid_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &pyramid_panel_data,
		sizeof(pyramid_panel_data));
	if (ret) {
		PR_DISP_ERR("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		PR_DISP_ERR("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	{0x03, 0x01, 0x01, 0x00},
	{0x96, 0x1E, 0x1E, 0x00, 0x3C, 0x3C, 0x1E, 0x28, 0x0b, 0x13, 0x04},
	{0x7f, 0x00, 0x00, 0x00},
	{0xee, 0x02, 0x86, 0x00},
	{0x41, 0x9c, 0xb9, 0xd6, 0x00, 0x50, 0x48, 0x63, 0x01, 0x0f, 0x07,
	0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03 },
};

static int __init mipi_cmd_novatek_blue_qhd_pt_init(void)
{
	int ret;

	pinfo.xres = 540;
	pinfo.yres = 960;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.width = 53;
	pinfo.height = 95;
	pinfo.lcdc.h_back_porch = 64;
	pinfo.lcdc.h_front_porch = 96;
	pinfo.lcdc.h_pulse_width = 32;
	pinfo.lcdc.v_back_porch = 16;
	pinfo.lcdc.v_front_porch = 16;
	pinfo.lcdc.v_pulse_width = 4;
	pinfo.lcdc.border_clr = 0;
	pinfo.lcdc.underflow_clr = 0xff;
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 1;
	pinfo.fb_num = 2;
	pinfo.clk_rate = 482000000;
	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 6096;
	pinfo.lcd.v_back_porch = 16;
	pinfo.lcd.v_front_porch = 16;
	pinfo.lcd.v_pulse_width = 4;
	pinfo.mipi.mode = DSI_CMD_MODE;
	pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap = DSI_RGB_SWAP_BGR;
	pinfo.mipi.data_lane0 = TRUE;
	pinfo.mipi.data_lane1 = TRUE;
	pinfo.mipi.t_clk_post = 0x0a;
	pinfo.mipi.t_clk_pre = 0x1e;
	pinfo.mipi.stream = 0;
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.te_sel = 1;
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;

	ret = mipi_pyramid_device_register("mipi_novatek", &pinfo, MIPI_DSI_PRIM,
						MIPI_DSI_PANEL_QHD_PT);
	if (ret)
		PR_DISP_ERR("%s: failed to register device!\n", __func__);

	return ret;
}

void __init pyramid_init_fb(void)
{
	platform_device_register(&msm_fb_device);

	if (panel_type != PANEL_ID_NONE) {
		msm_fb_register_device("mdp", &mdp_pdata);
		msm_fb_register_device("mipi_dsi", &mipi_dsi_pdata);
	}

#ifdef CONFIG_FB_MSM_DTV
	msm_fb_register_device("dtv", &dtv_pdata);
#endif
}

static int __init pyramid_panel_init(void)
{
	mipi_dsi_buf_alloc(&panel_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&panel_rx_buf, DSI_BUF_SIZE);

	switch (panel_type) {
		case PANEL_ID_PYD_SHARP:
			PR_DISP_INFO("%s: panel ID = PANEL_ID_PYD_SHARP\n", __func__);
			mipi_cmd_novatek_blue_qhd_pt_init();
			break;
		case PANEL_ID_PYD_AUO_NT:
			PR_DISP_INFO("%s: panel ID = PANEL_ID_PYD_AUO_NT\n", __func__);
			mipi_cmd_novatek_blue_qhd_pt_init();
			break;
		default:
			PR_DISP_ERR("%s: panel not supported!\n", __func__);
			return -ENODEV;
	}

	return platform_driver_register(&this_driver);
}

device_initcall_sync(pyramid_panel_init);
