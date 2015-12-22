/*
 * (C) Copyright 2009 Apple Computer
 */

#include <common.h>
#include <ppc4xx.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/4xx_pcie.h>
#include <asm/ppc4xx-sdram.h>

#define DEBUG_ENV
#ifdef DEBUG_ENV
#define DEBUGF(fmt,args...) printf(fmt ,##args)
#else
#define DEBUGF(fmt,args...)
#endif

#define CONFIG_SYS_SET_IRQ_31       0x00000001
#define CONFIG_SYS_CLEAR_ALL_IRQ    0xFFFFFFFF
#define CONFIG_SYS_DISABLE_ALL_IRQ  0x00000000
#define CONFIG_SYS_SET_ALL_IRQ      0x00000000
#define CONFIG_SYS_SET_ALL_IRQ_TRIG 0x001fffff
#define CONFIG_SYS_SET_IRQ_POL2     0xebebebf3
#define CONFIG_SYS_SET_IRQ_LVL2     0x74747400
#define CONFIG_SYS_SET_IRQ_LVL1     0x001f8040
#define CONFIG_SYS_SET_IRQ_CRT0     0x00104001
#define CONFIG_SYS_SET_IRQ_LVL0     0x010f0004


/*
 * Small Flash and FRAM
 * BU Value
 * BxAP : 0x03800000  - 0 00000111 0 00 00 00 00 00 000 0 0 0 0 00000
 * B0CR : 0xff098000  - BAS = ff0 - 100 11 00 0000000000000
 * B2CR : 0xe7098000  - BAS = e70 - 100 11 00 0000000000000
 */
#define EBC_BXAP_SMALL_FLASH		EBC_BXAP_BME_DISABLED   | \
					EBC_BXAP_TWT_ENCODE(7)  | \
					EBC_BXAP_BCE_DISABLE	| \
					EBC_BXAP_BCT_2TRANS | \
					EBC_BXAP_CSN_ENCODE(0)  | \
					EBC_BXAP_OEN_ENCODE(0)  | \
					EBC_BXAP_WBN_ENCODE(0)  | \
					EBC_BXAP_WBF_ENCODE(0)  | \
					EBC_BXAP_TH_ENCODE(0)   | \
					EBC_BXAP_RE_DISABLED	| \
					EBC_BXAP_SOR_DELAYED	| \
					EBC_BXAP_BEM_WRITEONLY  | \
					EBC_BXAP_PEN_DISABLED

#define EBC_BXCR_SMALL_FLASH_CS0	EBC_BXCR_BAS_ENCODE(0xFF000000) | \
					EBC_BXCR_BS_16MB		| \
					EBC_BXCR_BU_RW		  | \
					EBC_BXCR_BW_8BIT

#define EBC_BXCR_SMALL_FLASH_CS2	EBC_BXCR_BAS_ENCODE(0xe7000000) | \
					EBC_BXCR_BS_16MB		| \
					EBC_BXCR_BU_RW		  | \
					EBC_BXCR_BW_8BIT

#define EBC_BXAP_LARGE_FLASH 0x0604fa00

#define EBC_BXCR_LARGE_FLASH_CS0	EBC_BXCR_BAS_ENCODE(0xFF000000) | \
					EBC_BXCR_BS_16MB		| \
					EBC_BXCR_BU_RW		  | \
					EBC_BXCR_BW_16BIT

#define EBC_BXCR_LARGE_FLASH_CS2	EBC_BXCR_BAS_ENCODE(0xE0000000) | \
					EBC_BXCR_BS_128MB	   | \
					EBC_BXCR_BU_RW		  | \
					EBC_BXCR_BW_16BIT


int board_early_init_f (void)
{
	unsigned long mfr;
	unsigned long ebc0_cs0_bxap_value = 0;
	unsigned long ebc0_cs0_bxcr_value = 0;
	unsigned long ebc0_cs2_bxap_value = 0;
	unsigned long ebc0_cs2_bxcr_value = 0;

	mtebc(xbcfg, EBC_CFG_LE_UNLOCK |
			EBC_CFG_PTD_ENABLE |
			EBC_CFG_RTC_16PERCLK |
			EBC_CFG_ATC_PREVIOUS |
			EBC_CFG_DTC_PREVIOUS |
			EBC_CFG_CTC_PREVIOUS |
			EBC_CFG_OEO_PREVIOUS |
			EBC_CFG_EMC_DEFAULT |
			EBC_CFG_PME_DISABLE |
			EBC_CFG_PR_16);

	/*-------------------------------------------------------------------+
	 | EBC Devices Characteristics
	 |   Peripheral Bank Access Parameters	   -   EBC_BxAP
	 |   Peripheral Bank Configuration Register  -   EBC_BxCR
	 +-------------------------------------------------------------------*/

	/*-------------------------------------------------------------------+
	 |  - Boot from EBC 8bits => boot from Small Flash selected
	 |			EBC-CS0	 = Small Flash
	 |			EBC-CS2	 = Large Flash and SRAM
	 +-------------------------------------------------------------------*/

	ebc0_cs0_bxap_value = EBC_BXAP_SMALL_FLASH;
	ebc0_cs0_bxcr_value = EBC_BXCR_SMALL_FLASH_CS0;
	ebc0_cs2_bxap_value = EBC_BXAP_LARGE_FLASH;
	ebc0_cs2_bxcr_value = EBC_BXCR_LARGE_FLASH_CS2;

	mtebc(pb0ap, ebc0_cs0_bxap_value);
	mtebc(pb0cr, ebc0_cs0_bxcr_value);
	mtebc(pb2ap, ebc0_cs2_bxap_value);
	mtebc(pb2cr, ebc0_cs2_bxcr_value);

	/*--------------------------------------------------------------------+
	 | Put UICs in PowerPC440SPemode.
	 | Initialise UIC registers.  Clear all interrupts.  Disable all
	 | interrupts.
	 | Set critical interrupt values.  Set interrupt polarities.  Set
	 | interrupt trigger levels.  Make bit 0 High  priority.  Clear all
	 | interrupts again.
	 +-------------------------------------------------------------------*/

	mtdcr (uic3sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* Clear all interrupts */
	mtdcr (uic3er, CONFIG_SYS_DISABLE_ALL_IRQ); /* disable all interrupts */
	mtdcr (uic3cr, CONFIG_SYS_SET_ALL_IRQ); /* Set Critical / Non Critical interrupts */
	mtdcr (uic3pr, CONFIG_SYS_CLEAR_ALL_IRQ); /* Set Interrupt Polarities */
	mtdcr (uic3tr, CONFIG_SYS_SET_ALL_IRQ_TRIG); /* Set Interrupt Trigger Levels */
	mtdcr (uic3vr, CONFIG_SYS_SET_IRQ_31); /* Set Vect base=0,INT31 Highest priority */
	mtdcr (uic3sr, CONFIG_SYS_DISABLE_ALL_IRQ); /* clear all  interrupts */
	mtdcr (uic3sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* clear all  interrupts */

	mtdcr (uic2sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* Clear all interrupts */
	mtdcr (uic2er, CONFIG_SYS_DISABLE_ALL_IRQ); /* disable all interrupts */
	mtdcr (uic2cr, CONFIG_SYS_SET_ALL_IRQ); /* Set Critical / Non Critical interrupts */

	mtdcr (uic2pr, CONFIG_SYS_SET_IRQ_POL2); /* Set Interrupt Polarities */
	mtdcr (uic2tr, CONFIG_SYS_SET_IRQ_LVL2); /* Set Interrupt Trigger Levels */
	mtdcr (uic2vr, CONFIG_SYS_SET_IRQ_31); /* Set Vect base=0,INT31 Highest priority */
	mtdcr (uic2sr, CONFIG_SYS_DISABLE_ALL_IRQ); /* clear all interrupts */
	mtdcr (uic2sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* clear all interrupts */

	mtdcr (uic1sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* Clear all interrupts */
	mtdcr (uic1er, CONFIG_SYS_DISABLE_ALL_IRQ); /* disable all interrupts */
	mtdcr (uic1cr, CONFIG_SYS_SET_ALL_IRQ); /* Set Critical / Non Critical interrupts */
	mtdcr (uic1pr, CONFIG_SYS_CLEAR_ALL_IRQ); /* Set Interrupt Polarities */
	mtdcr (uic1tr, CONFIG_SYS_SET_IRQ_LVL1); /* Set Interrupt Trigger Levels */
	mtdcr (uic1vr, CONFIG_SYS_SET_IRQ_31); /* Set Vect base=0,INT31 Highest priority */
	mtdcr (uic1sr, CONFIG_SYS_DISABLE_ALL_IRQ); /* clear all interrupts */
	mtdcr (uic1sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* clear all interrupts */

	mtdcr (uic0sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* Clear all interrupts */
	mtdcr (uic0er, CONFIG_SYS_DISABLE_ALL_IRQ); /* disable all interrupts excepted cascade to be checked */
	mtdcr (uic0cr, CONFIG_SYS_SET_IRQ_CRT0); /* Set Critical / Non Critical interrupts */
	mtdcr (uic0pr, CONFIG_SYS_CLEAR_ALL_IRQ); /* Set Interrupt Polarities */
	mtdcr (uic0tr, CONFIG_SYS_SET_IRQ_LVL0); /* Set Interrupt Trigger Levels */
	mtdcr (uic0vr, CONFIG_SYS_SET_IRQ_31); /* Set Vect base=0,INT31 Highest priority */
	mtdcr (uic0sr, CONFIG_SYS_DISABLE_ALL_IRQ); /* clear all interrupts */
	mtdcr (uic0sr, CONFIG_SYS_CLEAR_ALL_IRQ); /* clear all interrupts */

	mfsdr(sdr_mfr, mfr);
	mfr |= SDR0_MFR_FIXD; /* Workaround for PCI/DMA */
	mfr &= ~SDR0_MFR_ECS_MASK; /* Select ethernet external clock */
	mtsdr(sdr_mfr, mfr);

	return 0;
}

static char * hex2str(char * str, unsigned char num)
{
	const char map[] = {'0','1','2','3','4','5','6','7',
	                    '8','9','A','B','C','D','E','F'};

	str[0] = map[num >> 4];
	str[1] = map[num & 0xF];
	str[2] = '\0';

	return str;
}

#define XMK_STR(x) #x
#define MK_STR(x) XMK_STR(x)
#define PRINTABLE_CHAR(byte) (((byte) < 32) || ((byte) > 126)) ? '-' : (byte) 

int checkboard (void)
{
	char str[0x17];
	char * dataPtr = (char *)VPD_BASE_ADDR;

	puts("Board: ");
	puts(MK_STR(CONFIG_HOSTNAME));
	puts(" - Apple RAID Card");
	puts(  " - REV ");


	str[0x00] = PRINTABLE_CHAR(dataPtr[0x30]);
	str[0x01] = PRINTABLE_CHAR(dataPtr[0x31]);
	str[0x02] = PRINTABLE_CHAR(dataPtr[0x32]);
	str[0x03] = PRINTABLE_CHAR(dataPtr[0x33]);
	str[0x04] = '\0';

	puts(str);
	putc('\n');

	puts(  "       WWN # ");

	puts(hex2str(str, dataPtr[0x00]));
	puts(":");
	puts(hex2str(str, dataPtr[0x01]));
	puts(":");
	puts(hex2str(str, dataPtr[0x02]));
	puts(":");
	puts(hex2str(str, dataPtr[0x03]));
	puts(":");
	puts(hex2str(str, dataPtr[0x04]));
	puts(":");
	puts(hex2str(str, dataPtr[0x05]));
	puts(":");
	puts(hex2str(str, dataPtr[0x06]));
	puts(":");
	puts(hex2str(str, dataPtr[0x07]));
	putc('\n');

	puts(  "       serial # ");

	str[0x00] = PRINTABLE_CHAR(dataPtr[0x08]);
	str[0x01] = PRINTABLE_CHAR(dataPtr[0x09]);
	str[0x02] = PRINTABLE_CHAR(dataPtr[0x0A]);
	str[0x03] = PRINTABLE_CHAR(dataPtr[0x0B]);
	str[0x04] = PRINTABLE_CHAR(dataPtr[0x0C]);
	str[0x05] = PRINTABLE_CHAR(dataPtr[0x0D]);
	str[0x06] = PRINTABLE_CHAR(dataPtr[0x0E]);
	str[0x07] = PRINTABLE_CHAR(dataPtr[0x0F]);
	str[0x08] = '\0';
	puts(str);
	putc('\n');

	puts(  "       part # ");

	str[0x00] = PRINTABLE_CHAR(dataPtr[0x10]);
	str[0x01] = PRINTABLE_CHAR(dataPtr[0x11]);
	str[0x02] = PRINTABLE_CHAR(dataPtr[0x12]);
	str[0x03] = PRINTABLE_CHAR(dataPtr[0x13]);
	str[0x04] = PRINTABLE_CHAR(dataPtr[0x14]);
	str[0x05] = PRINTABLE_CHAR(dataPtr[0x15]);
	str[0x06] = PRINTABLE_CHAR(dataPtr[0x16]);
	str[0x07] = PRINTABLE_CHAR(dataPtr[0x17]);
	str[0x08] = '\0';

	puts(str);
	putc('\n');

	return 0;
}

#if !defined(CONFIG_PPC4xx_DDR_AUTOCALIBRATION)

/*
 * Override the default functions in cpu/ppc4xx/44x_spd_ddr2.c with
 * board specific values.
 */
static int ppc440spe_rev_a(void)
{
	if ((get_pvr() == PVR_440SPe_6_RA) || (get_pvr() == PVR_440SPe_RA))
		return 1;
	else
		return 0;
}

u32 ddr_wrdtr(u32 default_val)
{
	/*
	 * K46 boards with 440SPe rev. A need a slightly different setup
	 * for the MCIF0_WRDTR register.
	 */
	if (ppc440spe_rev_a())
		return (SDRAM_WRDTR_LLWP_1_CYC | SDRAM_WRDTR_WTR_270_DEG_ADV);

	return default_val;
}

u32 ddr_clktr(u32 default_val)
{
	/*
	 * K46 boards with 440SPe rev. A need a slightly different setup
	 * for the MCIF0_CLKTR register.
	 */
	if (ppc440spe_rev_a())
		return (SDRAM_CLKTR_CLKP_180_DEG_ADV);

	return default_val;
}

#else /* CONFIG_PPC4xx_DDR_AUTOCALIBRATION */

struct sdram_timing ddr_scan_opts [] = {
	{4, 3},
	{-1, -1}
};

static ulong rqfd_range[] = { 0x32, 0x36 };
static ulong rffd_range[] = { 500, 650 };

ulong ddr_scan_option(ulong def)
{
	return (ulong)(&ddr_scan_opts);
}

ulong ddr_rqfd_start_opt(ulong def)
{
	return(rqfd_range[0]);
}

ulong ddr_rqfd_end_opt(ulong def)
{
	return(rqfd_range[1]);
}

ulong ddr_rffd_start_opt(ulong def)
{
	return(rffd_range[0]);
}

ulong ddr_rffd_end_opt(ulong def)
{
	return(rffd_range[1]);
}

#endif /* end CONFIG_PPC4xx_DDR_AUTOCALIBRATION */

void board_add_ram_info(int use_default)
{
	PPC4xx_SYS_INFO board_cfg;
	u32 val;

#define MULDIV64(m1, m2, d)	(u32)(((u64)(m1) * (u64)(m2)) / (u64)(d))

	mfsdram(SDRAM_MCOPT1, val);

	switch (val & SDRAM_MCOPT1_MCHK_CHK_REP)
	{
		case SDRAM_MCOPT1_MCHK_GEN:
			puts(" (ECC generation only enabled");
			break;
		case SDRAM_MCOPT1_MCHK_CHK:
			puts(" (ECC check & correction");
			break;
		case SDRAM_MCOPT1_MCHK_CHK_REP:
			puts(" (ECC correction & reporting");
			break;
		default :
			puts(" (ECC disabled");
			break;
	}

	get_sys_info(&board_cfg);

	mfsdr(SDR0_DDR0, val);
	val = MULDIV64((board_cfg.freqPLB), SDR0_DDR0_DDRM_DECODE(val), 1);
	printf(", %d MHz", (val * 2) / 1000000);

	mfsdram(SDRAM_MMODE, val);
	val = (val & SDRAM_MMODE_DCL_MASK) >> 4;
	printf(", CL%d)", val);
}

#if defined(CONFIG_POST)

/*
 * Returns 1 if keys pressed to start the power-on long-running tests
 * Called from board_init_f().
 */
int post_hotkeys_pressed(void)
{
	return (ctrlc());
}

#endif /*  defined(CONFIG_POST) */

#if defined(CONFIG_SYS_DRAM_TEST)

/*
 * This function implements a very basic DRAM test.
 */
int testdram(void)
{
	uint *pstart = (uint *) CONFIG_SYS_MEMTEST_START;
	uint *pend = (uint *) CONFIG_SYS_MEMTEST_END;
	uint *p;

	printf ("SDRAM test phase 1:\n");
	for (p = pstart; p < pend; p++)
		*p = 0xaaaaaaaa;

	for (p = pstart; p < pend; p++) {
		if (*p != 0xaaaaaaaa) {
			printf ("SDRAM test fails at: %08x\n", (uint) p);
			return 1;
		}
	}

	printf ("SDRAM test phase 2:\n");
	for (p = pstart; p < pend; p++)
		*p = 0x55555555;

	for (p = pstart; p < pend; p++) {
		if (*p != 0x55555555) {
			printf ("SDRAM test fails at: %08x\n", (uint) p);
			return 1;
		}
	}

	printf ("SDRAM test passed.\n");
	return 0;
}

#endif /* CONFIG_SYS_DRAM_TEST */
