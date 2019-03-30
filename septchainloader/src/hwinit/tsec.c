/*
 * Copyright (c) 2018 naehrwert
 * Copyright (c) 2018 CTCaer
 * Copyright (c) 2018 balika011
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include "tsec.h"
#include "tsec_t210.h"
#include "se_t210.h"
#include "clock.h"
#include "smmu.h"
#include "t210.h"
#include "heap.h"
#include "mc.h"
#include "util.h"
#include "gfx.h"

/* #include "../gfx/gfx.h"
extern gfx_con_t gfx_con; */

static int _tsec_dma_wait_idle()
{
	u32 timeout = get_tmr_ms() + 10000;
	while (!(TSEC(TSEC_DMATRFCMD) & TSEC_DMATRFCMD_IDLE))
		if (get_tmr_ms() > timeout)
			return 0;

	return 1;
}

static int _tsec_dma_pa_to_internal_100(int not_imem, int i_offset, int pa_offset)
{
	u32 cmd;

	if (not_imem)
		cmd = TSEC_DMATRFCMD_SIZE_256B; // DMA 256 bytes
	else
		cmd = TSEC_DMATRFCMD_IMEM;      // DMA IMEM (Instruction memmory)

	TSEC(TSEC_DMATRFMOFFS) = i_offset;
	TSEC(TSEC_DMATRFFBOFFS) = pa_offset;
	TSEC(TSEC_DMATRFCMD) = cmd;

	return _tsec_dma_wait_idle();
}

int tsec_query(u8 *tsec_keys, u8 kb, tsec_ctxt_t *tsec_ctxt)
{
	int res = 0;
	u8 *fwbuf = NULL;
	u32 *pdir, *car, *fuse, *pmc, *flowctrl, *se, *mc, *iram, *evec;

	//Enable clocks.
	clock_enable_host1x();
	clock_enable_tsec();
	clock_enable_sor_safe();
	clock_enable_sor0();
	clock_enable_sor1();
	clock_enable_kfuse();

	//Configure Falcon.
	TSEC(TSEC_DMACTL) = 0;
	TSEC(TSEC_IRQMSET) = 0xFFF2;
	TSEC(TSEC_IRQDEST) = 0xFFF0;
	TSEC(TSEC_ITFEN) = 0x03;
	if (!_tsec_dma_wait_idle())
	{
		res = -1;
		goto out;
	}

	//Load firmware or emulate memio environment for newer TSEC fw.
	if (kb <= KB_FIRMWARE_VERSION_600)
	{
		fwbuf = (u8 *)malloc(0x2000);
		u8 *fwbuf_aligned = (u8 *)ALIGN((u32)fwbuf + 0x1000, 0x100);
		memcpy(fwbuf_aligned, tsec_ctxt->fw, tsec_ctxt->size);
		TSEC(TSEC_DMATRFBASE) = (u32)fwbuf_aligned >> 8;
	}
	else{
		void * temp = malloc(0x3000);
		temp = (void *)ALIGN((u32)temp, 0x100);
		memcpy(temp, tsec_ctxt->fw, tsec_ctxt->size);
		tsec_ctxt->fw = temp;
		TSEC(TSEC_DMATRFBASE) = (u32)tsec_ctxt->fw >> 8;
        free(temp);
	}


	for (u32 addr = 0; addr < tsec_ctxt->size; addr += 0x100)
	{
		if (!_tsec_dma_pa_to_internal_100(false, addr, addr))
		{
			res = -2;
			goto out_free;
		}
	}
    
	if (kb >= KB_FIRMWARE_VERSION_620)
	{
		// Init SMMU translation for TSEC.
		print("initing smmu\n");
		pdir = smmu_init_for_tsec();
		print("initing secmon base\n");
		smmu_init(tsec_ctxt->secmon_base);
		print("enabling smmu\n");
		// Enable SMMU
		if (!smmu_is_used())
			smmu_enable();

		// Clock reset controller.
		car = page_alloc(1);
		memcpy(car, (void *)CLOCK_BASE, 0x1000);
		car[CLK_RST_CONTROLLER_CLK_SOURCE_TSEC / 4] = 2;
		smmu_map(pdir, CLOCK_BASE, (u32)car, 1, _WRITABLE | _READABLE | _NONSECURE);

		// Fuse driver.
		fuse = page_alloc(1);
		memcpy((void *)&fuse[0x800/4], (void *)FUSE_BASE, 0x400);
		fuse[0x82C / 4] = 0;
		fuse[0x9E0 / 4] = (1 << (kb + 2)) - 1;
		fuse[0x9E4 / 4] = (1 << (kb + 2)) - 1;
		smmu_map(pdir, (FUSE_BASE - 0x800), (u32)fuse, 1, _READABLE | _NONSECURE);

		// Power management controller.
		pmc = page_alloc(1);
		smmu_map(pdir, RTC_BASE, (u32)pmc, 1, _READABLE | _NONSECURE);

		// Flow control.
		flowctrl = page_alloc(1);
		smmu_map(pdir, FLOW_CTLR_BASE, (u32)flowctrl, 1, _WRITABLE | _NONSECURE);

		// Security engine.
		se = page_alloc(1);
		memcpy(se, (void *)SE_BASE, 0x1000);
		smmu_map(pdir, SE_BASE, (u32)se, 1, _READABLE | _WRITABLE | _NONSECURE);

		// Memory controller.
		mc = page_alloc(1);
		memcpy(mc, (void *)MC_BASE, 0x1000);
		mc[MC_IRAM_BOM / 4] = 0;
		mc[MC_IRAM_TOM / 4] = 0x80000000;
		smmu_map(pdir, MC_BASE, (u32)mc, 1, _READABLE | _NONSECURE);

		// IRAM
		iram = page_alloc(0x30);
		memcpy(iram, tsec_ctxt->pkg1, 0x30000);
		smmu_map(pdir, 0x40010000, (u32)iram, 0x30, _READABLE | _WRITABLE | _NONSECURE);

		// Exception vectors
		evec = page_alloc(1);
		smmu_map(pdir, EXCP_VEC_BASE, (u32)evec, 1, _READABLE | _WRITABLE | _NONSECURE);
	}
	print("Executing TSEC\n");
	//Execute firmware.
    
	HOST1X(0x3300) = 0x34C2E1DA;
	TSEC(TSEC_STATUS) = 0;
	TSEC(TSEC_BOOTKEYVER) = 1;
	TSEC(TSEC_BOOTVEC) = 0;
	TSEC(TSEC_CPUCTL) = TSEC_CPUCTL_STARTCPU;
    
	if (kb <= KB_FIRMWARE_VERSION_600)
	{
		if (!_tsec_dma_wait_idle())
		{
			res = -3;
			goto out_free;
		}
		u32 timeout = get_tmr_ms() + 2000;
		while (!TSEC(TSEC_STATUS)){
			if (get_tmr_ms() > timeout)
			{
				res = -4;
				goto out_free;
			}
        }
		if (TSEC(TSEC_STATUS) != 0xB0B0B0B0)
		{
			res = -5;
			goto out_free;
		}

		//Fetch result.
		HOST1X(0x3300) = 0;
		u32 buf[4];
		buf[0] = SOR1(SOR_NV_PDISP_SOR_DP_HDCP_BKSV_LSB);
		buf[1] = SOR1(SOR_NV_PDISP_SOR_TMDS_HDCP_BKSV_LSB);
		buf[2] = SOR1(SOR_NV_PDISP_SOR_TMDS_HDCP_CN_MSB);
		buf[3] = SOR1(SOR_NV_PDISP_SOR_TMDS_HDCP_CN_LSB);
		SOR1(SOR_NV_PDISP_SOR_DP_HDCP_BKSV_LSB) = 0;
		SOR1(SOR_NV_PDISP_SOR_TMDS_HDCP_BKSV_LSB) = 0;
		SOR1(SOR_NV_PDISP_SOR_TMDS_HDCP_CN_MSB) = 0;
		SOR1(SOR_NV_PDISP_SOR_TMDS_HDCP_CN_LSB) = 0;

		memcpy(tsec_keys, &buf, 0x10);
	}
	else
	{
		u32 start = get_tmr_us();
		u32 k = se[SE_KEYTABLE_DATA0_REG_OFFSET / 4];
		u32 key[16] = {0};
		u32 kidx = 0;
		int xx = 0;

        u32 key_buf[0x20/4] = {0};
		volatile u32 *key_data = (vu32 *)((void *)se + 0x320);
		u32 old_key_data = *key_data;
		u32 buf_counter = 0;
		while (!(TSEC(TSEC_CPUCTL) & 0x10)) {
            const u32 new_key_data = *key_data;
            if (new_key_data != old_key_data) {
                    old_key_data = new_key_data;
                    key_buf[buf_counter] = new_key_data;
                    buf_counter++;
            }
		}
		memcpy(tsec_keys, key_buf, 0x20);
		memcpy(tsec_ctxt->pkg1, iram, 0x30000);
		smmu_deinit_for_tsec();
	}

out_free:;
	free(fwbuf);

out:;

	//Disable clocks.
	clock_disable_kfuse();
	clock_disable_sor1();
	clock_disable_sor0();
	clock_disable_sor_safe();
	clock_disable_tsec();
	clock_disable_host1x();

	return res;
}