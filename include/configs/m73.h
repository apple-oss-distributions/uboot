/*
 * (C) Copyright 2009 Apple Computer
 */

/************************************************************************
 * m73.h - configuration for m73
 ***********************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_IDENT_STRING		" M-2.2.1.0"

/**************
 * DEBUG STUFF
 **************/

#undef DEBUG

/*-----------------------------------------------------------------------
 * High Level Configuration Options
 *----------------------------------------------------------------------*/

#define CONFIG_HOSTNAME			m73

#define CONFIG_4xx			1 /* ... PPC4xx family */
#define CONFIG_440			1 /* ... PPC440 family */
#define CONFIG_440SPE			1 /* Specifc SPe support */
#define CONFIG_BOARD_EARLY_INIT_F	1 /* Call board_pre_init */
#define CONFIG_SYS_CLK_FREQ		33333333 /* external freq to pll */
#define EXTCLK_33_33			33333333
#define EXTCLK_66_66			66666666
#define EXTCLK_50			50000000
#define EXTCLK_83			83333333

#define	CONFIG_MISC_INIT_F		/* Use misc_init_f() */
#undef  CONFIG_SHOW_BOOT_PROGRESS

/*-----------------------------------------------------------------------
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_SDRAM_BASE		0x00000000 /* _must_ be 0 */
#define CONFIG_SYS_MONITOR_BASE		(TEXT_BASE) /* Start of U-Boot */
#define CONFIG_SYS_ISRAM_BASE		0x90000000 /* internal SRAM */
#define CONFIG_SYS_PERIPHERAL_BASE	0xa0000000 /* internal peripherals */
#define CONFIG_SYS_FLASH_BASE		0xfff00000 /* start of BOOT Flash */
#define CONFIG_SYS_FLASH_2_BASE		0xe0000000 /* 2nd Flash [OS & Filesystem] */

#define CONFIG_SYS_PCI_MEMBASE		0x80000000 /* mapped PCI memory */
#define CONFIG_SYS_PCI_BASE		0xd0000000 /* internal PCI regs */
#define CONFIG_SYS_PCI_TARGBASE		(CONFIG_SYS_PCI_MEMBASE)

#define CONFIG_SYS_PCIE_MEMBASE		0xb0000000 /* mapped PCIe memory */
#define CONFIG_SYS_PCIE_MEMSIZE		0x01000000 /* smallest incr for PCIe port */
#define CONFIG_SYS_PCIE_BASE		0xc0000000 /* PCIe UTL regs */

#define CONFIG_SYS_PCIE0_CFGBASE	(CONFIG_SYS_PCIE_BASE)
#define CONFIG_SYS_PCIE1_CFGBASE	(CONFIG_SYS_PCIE_BASE + 0x1000)
#define CONFIG_SYS_PCIE2_CFGBASE	(CONFIG_SYS_PCIE_BASE + 0x2000)
#define CONFIG_SYS_PCIE0_XCFGBASE	(CONFIG_SYS_PCIE0_CFGBASE + 0x0400)
#define CONFIG_SYS_PCIE1_XCFGBASE	(CONFIG_SYS_PCIE1_CFGBASE + 0x0400)
#define CONFIG_SYS_PCIE2_XCFGBASE	(CONFIG_SYS_PCIE2_CFGBASE + 0x0400)

#define CONFIG_SYS_PCIE_REGBASE		(CONFIG_SYS_PCIE_BASE + 0x3000)
#define CONFIG_SYS_PCIE0_REGOFF		(0x0000)
#define CONFIG_SYS_PCIE1_REGOFF		(0x0400)
#define CONFIG_SYS_PCIE2_REGOFF		(0x1000)
#define CONFIG_SYS_PCIE3_REGOFF		(0x1400)
#define CONFIG_SYS_PCIE4_REGOFF		(0x2000)
#define CONFIG_SYS_PCIE5_REGOFF		(0x2400)
#define CONFIG_SYS_PCIE0_REGBASE	(CONFIG_SYS_PCIE_REGBASE + CONFIG_SYS_PCIE0_REGOFF)
#define CONFIG_SYS_PCIE1_REGBASE	(CONFIG_SYS_PCIE_REGBASE + CONFIG_SYS_PCIE1_REGOFF)
#define CONFIG_SYS_PCIE2_REGBASE	(CONFIG_SYS_PCIE_REGBASE + CONFIG_SYS_PCIE2_REGOFF)
#define CONFIG_SYS_PCIE3_REGBASE	(CONFIG_SYS_PCIE_REGBASE + CONFIG_SYS_PCIE3_REGOFF)
#define CONFIG_SYS_PCIE4_REGBASE	(CONFIG_SYS_PCIE_REGBASE + CONFIG_SYS_PCIE4_REGOFF)
#define CONFIG_SYS_PCIE5_REGBASE	(CONFIG_SYS_PCIE_REGBASE + CONFIG_SYS_PCIE5_REGOFF)

#define CONFIG_SYS_TLB_FLASH_MEMBASE_HIGH 0x4
#define CONFIG_SYS_TLB_FLASH_MEMBASE_LOW (CONFIG_SYS_FLASH_BASE)

#define CONFIG_SYS_TLB_FLASH_2_MEMBASE_HIGH 0x4
#define CONFIG_SYS_TLB_FLASH_2_MEMBASE_LOW (CONFIG_SYS_FLASH_2_BASE)


#define CONFIG_SYS_TLB_PCIE_CFGBASE_HIGH 0xd
#define CONFIG_SYS_TLB_PCIE0_CFGBASE_LOW (0x00100000)
#define CONFIG_SYS_TLB_PCIE1_CFGBASE_LOW (0x20100000)
#define CONFIG_SYS_TLB_PCIE2_CFGBASE_LOW (0x40100000)

#define CONFIG_SYS_TLB_PCIE_XFGBASE_HIGH 0xd
#define CONFIG_SYS_TLB_PCIE0_XFGBASE_LOW (0x10000000)
#define CONFIG_SYS_TLB_PCIE1_XFGBASE_LOW (0x30000000)
#define CONFIG_SYS_TLB_PCIE2_XFGBASE_LOW (0x50000000)

#define CONFIG_SYS_TLB_PCIE_MEMBASE_HIGH 0xd
#define CONFIG_SYS_TLB_PCIE_MEMBASE_LOW (CONFIG_SYS_PCIE_MEMBASE)

#define CONFIG_SYS_TLB_PCIE_REGBASE_HIGH 0xd
#define CONFIG_SYS_TLB_PCIE_REGBASE_LOW (0x60000000)
#define CONFIG_SYS_TLB_PCIE0_REGBASE_LOW (CONFIG_SYS_TLB_PCIE_REGBASE_LOW + CONFIG_SYS_PCIE0_REGOFF)
#define CONFIG_SYS_TLB_PCIE1_REGBASE_LOW (CONFIG_SYS_TLB_PCIE_REGBASE_LOW + CONFIG_SYS_PCIE1_REGOFF)
#define CONFIG_SYS_TLB_PCIE2_REGBASE_LOW (CONFIG_SYS_TLB_PCIE_REGBASE_LOW + CONFIG_SYS_PCIE2_REGOFF)
#define CONFIG_SYS_TLB_PCIE3_REGBASE_LOW (CONFIG_SYS_TLB_PCIE_REGBASE_LOW + CONFIG_SYS_PCIE3_REGOFF)
#define CONFIG_SYS_TLB_PCIE4_REGBASE_LOW (CONFIG_SYS_TLB_PCIE_REGBASE_LOW + CONFIG_SYS_PCIE4_REGOFF)
#define CONFIG_SYS_TLB_PCIE5_REGBASE_LOW (CONFIG_SYS_TLB_PCIE_REGBASE_LOW + CONFIG_SYS_PCIE5_REGOFF)

/* base address of inbound PCIe window */
#define CONFIG_SYS_PCIE_INBOUND_BASE	0x0000000400000000ULL

/* System RAM mapped to PCI space */
#define CONFIG_PCI_SYS_MEM_BUS		CONFIG_SYS_SDRAM_BASE
#define CONFIG_PCI_SYS_MEM_PHYS		CONFIG_SYS_SDRAM_BASE
#define CONFIG_PCI_SYS_MEM_SIZE		(1024 * 1024 * 1024)

/*-----------------------------------------------------------------------
 * Initial RAM & stack pointer (placed in internal SRAM)
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_TEMP_STACK_OCM	1
#define CONFIG_SYS_OCM_DATA_ADDR	CONFIG_SYS_ISRAM_BASE
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_ISRAM_BASE /* Initial RAM address */
#define CONFIG_SYS_INIT_RAM_END		0x2000 /* End of used area in RAM */
#define CONFIG_SYS_GBL_DATA_SIZE	128 /* num bytes initial data */

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_END - CONFIG_SYS_GBL_DATA_SIZE)
#define CONFIG_SYS_POST_WORD_ADDR	(CONFIG_SYS_GBL_DATA_OFFSET - 0x4)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_POST_WORD_ADDR

#define CONFIG_SYS_MONITOR_LEN		(0xFFFFFFFF - CONFIG_SYS_MONITOR_BASE + 1)
#define CONFIG_SYS_MALLOC_LEN		(1 << 19) /* 512 KBytes Reserved for malloc */

/*-----------------------------------------------------------------------
 * Serial Port
 *----------------------------------------------------------------------*/

#define CONFIG_SYS_EXT_SERIAL_CLOCK	(1843200 * 6) /* Ext clk @ 11.059 MHz */
#define CONFIG_BAUDRATE			115200

#define CONFIG_SYS_BAUDRATE_TABLE  \
    {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400}


/*-----------------------------------------------------------------------
 * DDR SDRAM
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_MBYTES_SDRAM		256

#define CONFIG_AUTOCALIB		"silent\0" /* default is non-verbose */
#define CONFIG_PPC4xx_DDR_AUTOCALIBRATION	/* IBM DDR autocalibration */
#define DEBUG_PPC4xx_DDR_AUTOCALIBRATION	/* dynamic DDR autocal debug */
#define CONFIG_PPC4xx_DDR_METHOD_A

#define CONFIG_4xx_DCACHE		/* enable Data Cache */
#define CONFIG_DDR_ECC			/* enable ECC support */

/* DDR1/2 SDRAM Device Control Register Data Values */

#define CONFIG_SYS_SDRAM_PLBADDULL	0x00000000 /* Upper 32 bit PLB base address */
#define CONFIG_SYS_SDRAM_PLBADDUHB	0x00000000 /* Upper 32 bit PLB base address High Bandwidth */
#define CONFIG_SYS_SDRAM_CONF1LL	0x00001080
#define CONFIG_SYS_SDRAM_CONF1HB	\
	(SDRAM_CONF1HB_RPLM | \
	 SDRAM_CONF1HB_WRCL)
#define CONFIG_SYS_SDRAM_CONFPATHB	0x10a68000

#define CONFIG_SYS_SDRAM_R0BAS	\
	(SDRAM_RXBAS_SDBA_ENCODE(0) | \
	 SDRAM_RXBAS_SDSZ_256)
#define CONFIG_SYS_SDRAM_R1BAS	\
	(SDRAM_RXBAS_SDBA_ENCODE(0) | \
	 SDRAM_RXBAS_SDSZ_0)
#define CONFIG_SYS_SDRAM_R2BAS	\
	(SDRAM_RXBAS_SDBA_ENCODE(0) | \
	 SDRAM_RXBAS_SDSZ_0)
#define CONFIG_SYS_SDRAM_R3BAS	\
	(SDRAM_RXBAS_SDBA_ENCODE(0) | \
	 SDRAM_RXBAS_SDSZ_0)

/* SDRAM Controller */
#define CONFIG_SYS_SDRAM0_MCOPT1	\
	(SDRAM_MCOPT1_MCHK_GEN | \
	 SDRAM_MCOPT1_PMU_OPEN | \
	 SDRAM_MCOPT1_DMWD_MASK | \
	 SDRAM_MCOPT1_DDR1_TYPE | \
	 SDRAM_MCOPT1_4_BANKS | \
	 SDRAM_MCOPT1_QDEP | \
	 SDRAM_MCOPT1_DCOO_DISABLED | \
	 SDRAM_MCOPT1_DREF_NORMAL)
#define CONFIG_SYS_SDRAM0_MODT0		\
	(SDRAM_MODT_ODTON_DISABLE | \
	 SDRAM_MODT_EB1W_DISABLE | \
	 SDRAM_MODT_EB1R_DISABLE | \
	 SDRAM_MODT_EB0W_ENABLE | \
	 SDRAM_MODT_EB0R_DISABLE)
#define CONFIG_SYS_SDRAM0_MODT1		\
	(SDRAM_MODT_ODTON_DISABLE | \
	 SDRAM_MODT_EB1W_DISABLE | \
	 SDRAM_MODT_EB1R_DISABLE | \
	 SDRAM_MODT_EB0W_DISABLE | \
	 SDRAM_MODT_EB0R_DISABLE)
#define CONFIG_SYS_SDRAM0_MODT2		\
	(SDRAM_MODT_ODTON_DISABLE | \
	 SDRAM_MODT_EB1W_DISABLE | \
	 SDRAM_MODT_EB1R_DISABLE | \
	 SDRAM_MODT_EB0W_DISABLE | \
	 SDRAM_MODT_EB0R_DISABLE)
#define CONFIG_SYS_SDRAM0_MODT3		\
	(SDRAM_MODT_ODTON_DISABLE | \
	 SDRAM_MODT_EB1W_DISABLE  | \
	 SDRAM_MODT_EB1R_DISABLE | \
	 SDRAM_MODT_EB0W_DISABLE | \
	 SDRAM_MODT_EB0R_DISABLE)
#define CONFIG_SYS_SDRAM0_MB0CF		\
	(SDRAM_BXCF_M_AM_2 | \
	 SDRAM_BXCF_M_BE_ENABLE)
#define CONFIG_SYS_SDRAM0_MB1CF		\
	(SDRAM_BXCF_M_BE_DISABLE)
#define CONFIG_SYS_SDRAM0_MB2CF		\
	(SDRAM_BXCF_M_BE_DISABLE)
#define CONFIG_SYS_SDRAM0_MB3CF		\
	(SDRAM_BXCF_M_BE_DISABLE)
#define CONFIG_SYS_SDRAM0_CODT		\
	(SDRAM_CODT_ODT_OFF | \
	 SDRAM_CODT_RK1W_OFF | \
	 SDRAM_CODT_RK1R_OFF | \
	 SDRAM_CODT_RK0W_OFF | \
	 SDRAM_CODT_RK0R_ON | \
	 SDRAM_CODT_ODTSH_NORMAL | \
	 SDRAM_CODT_CODTZ_75OHM | \
	 SDRAM_CODT_CKEG_OFF | \
	 SDRAM_CODT_CTLG_OFF | \
	 SDRAM_CODT_FBDG_OFF | \
	 SDRAM_CODT_FBRG_OFF | \
	 SDRAM_CODT_CKLZ_18OHM | \
	 SDRAM_CODT_DQS_2_5_V_DDR1 | \
	 SDRAM_CODT_DQS_SINGLE_END | \
	 SDRAM_CODT_CKSE_DIFFERENTIAL | \
	 SDRAM_CODT_IO_HIZ)
#define CONFIG_SYS_SDRAM0_MMODE		\
	(SDRAM_MMODE_DCL_DDR1_2_5_CLK | \
	 SDRAM_MMODE_BLEN_4)
#define CONFIG_SYS_SDRAM0_MEMODE	\
	(SDRAM_MEMODE_QOFF_ENABLE | \
	 SDRAM_MEMODE_RDQS_DISABLE | \
	 SDRAM_MEMODE_DQS_ENABLE | \
	 SDRAM_MEMODE_AL_DDR1_0_CYC | \
	 SDRAM_MEMODE_RTT_DISABLED | \
	 SDRAM_MEMODE_DIC_NORMAL | \
	 SDRAM_MEMODE_DLL_ENABLE)
#define CONFIG_SYS_SDRAM0_RTR		\
	(SDRAM_RTR_RINT_ENCODE(0x0BE0))
#define CONFIG_SYS_SDRAM0_INITPLR0	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(3) | \
	 SDRAM_INITPLR_ICMD_ENCODE(7) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR1	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(7) | \
	 SDRAM_INITPLR_ICMD_ENCODE(2) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(1024))
#define CONFIG_SYS_SDRAM0_INITPLR2	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(3) | \
	 SDRAM_INITPLR_ICMD_ENCODE(0) | \
	 SDRAM_INITPLR_IBA_ENCODE(1) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR3	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(3) | \
	 SDRAM_INITPLR_ICMD_ENCODE(7) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR4	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(1) | \
	 SDRAM_INITPLR_ICMD_ENCODE(7) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR5	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(255) | \
	 SDRAM_INITPLR_ICMD_ENCODE(0) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(354))
#define CONFIG_SYS_SDRAM0_INITPLR6	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(7) | \
	 SDRAM_INITPLR_ICMD_ENCODE(2) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(1024))
#define CONFIG_SYS_SDRAM0_INITPLR7	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(1) | \
	 SDRAM_INITPLR_ICMD_ENCODE(7) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR8	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(52) | \
	 SDRAM_INITPLR_ICMD_ENCODE(1) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR9	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(52) | \
	 SDRAM_INITPLR_ICMD_ENCODE(1) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR10	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(255) | \
	 SDRAM_INITPLR_ICMD_ENCODE(7) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(0))
#define CONFIG_SYS_SDRAM0_INITPLR11	\
	(SDRAM_INITPLR_ENABLE | \
	 SDRAM_INITPLR_IMWT_ENCODE(255) | \
	 SDRAM_INITPLR_ICMD_ENCODE(0) | \
	 SDRAM_INITPLR_IBA_ENCODE(0) | \
	 SDRAM_INITPLR_IMA_ENCODE(98))
#define CONFIG_SYS_SDRAM0_INITPLR12	\
	(SDRAM_INITPLR_DISABLE)
#define CONFIG_SYS_SDRAM0_INITPLR13	\
	(SDRAM_INITPLR_DISABLE)
#define CONFIG_SYS_SDRAM0_INITPLR14	\
	(SDRAM_INITPLR_DISABLE)
#define CONFIG_SYS_SDRAM0_INITPLR15	\
	(SDRAM_INITPLR_DISABLE)
#define CONFIG_SYS_SDRAM0_CLKTR		\
	(SDRAM_CLKTR_CLKP_90_DEG_ADV)
#define CONFIG_SYS_SDRAM0_WRDTR	\
	(0x80000800 | \
	 SDRAM_WRDTR_LLWP_1_CYC | \
	 SDRAM_WRDTR_WTR_90_DEG_ADV)
#define CONFIG_SYS_SDRAM0_SDTR1	\
	(SDRAM_SDTR1_LDOF_2_CLK | \
	 SDRAM_SDTR1_RTW_2_CLK | \
	 SDRAM_SDTR1_WTWO_0_CLK | \
	 SDRAM_SDTR1_RTRO_1_CLK)
#define CONFIG_SYS_SDRAM0_SDTR2	\
	(SDRAM_SDTR2_RCD_4_CLK | \
	 SDRAM_SDTR2_WTR_3_CLK | \
	 SDRAM_SDTR2_XSNR_16_CLK | \
	 SDRAM_SDTR2_WPC_3_CLK | \
	 SDRAM_SDTR2_RPC_2_CLK | \
	 SDRAM_SDTR2_RP_4_CLK | \
	 SDRAM_SDTR2_RRD_2_CLK)
#define CONFIG_SYS_SDRAM0_SDTR3	\
	(SDRAM_SDTR3_RAS_ENCODE(7) | \
	 SDRAM_SDTR3_RC_ENCODE(10) | \
	 SDRAM_SDTR3_XCS | \
	 SDRAM_SDTR3_RFC_ENCODE(12))
#define CONFIG_SYS_SDRAM0_DLCR	\
	(SDRAM_DLCR_DCLM_AUTO | \
	 SDRAM_DLCR_DLCR_IDLE | \
	 SDRAM_DLCR_DLCS_NOT_RUN | \
	 SDRAM_DLCR_DLCV_ENCODE(20))

#if !defined(CONFIG_PPC4xx_DDR_AUTOCALIBRATION)

#define CONFIG_SYS_SDRAM0_RQDC	\
	(SDRAM_RQDC_RQDE_ENABLE | \
	 SDRAM_RQDC_RQFD_ENCODE(56))
#define CONFIG_SYS_SDRAM0_RDCC	\
	(SDRAM_RDCC_RDSS_T1 | \
	 SDRAM_RDCC_RDSS_ENCODE(0) | \
	 SDRAM_RDCC_RSAE_ENABLE)
#define CONFIG_SYS_SDRAM0_RFDC	\
	(SDRAM_RFDC_ARSE_ENABLE | \
	 SDRAM_RFDC_RFOS_ENCODE(95) | \
	 SDRAM_RFDC_RFFD_ENCODE(640))

#endif /* CONFIG_PPC4xx_DDR_AUTOCALIBRATION */


/*-----------------------------------------------------------------------
 * I2C
 *----------------------------------------------------------------------*/
#define CONFIG_HARD_I2C			/* I2C with hardware support */
#define CONFIG_SYS_I2C_SPEED		400000 /* I2C speed and slave address */
#define CONFIG_SYS_I2C_SLAVE		0x7F

#define IIC0_BOOTPROM_ADDR		0x50
#define IIC0_ALT_BOOTPROM_ADDR		0x54

#define CONFIG_SYS_I2C_NOPROBES		{0x52, 0x53}
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2 /* Bytes of address */

/*-----------------------------------------------------------------------
 * Environment
 *----------------------------------------------------------------------*/

#define CONFIG_ENV_IS_IN_FLASH		/* Environment uses flash */
#undef CONFIG_ENV_IS_IN_NVRAM		/* ... not in NVRAM */
#undef CONFIG_ENV_IS_IN_EEPROM		/* ... not in EEPROM */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_MISC_INIT_R

/*
 * Default environment variables
 */

#define	CONFIG_EXTRA_ENV_SETTINGS \
        "bootdelay=3\0" \
        "dualboot=1\0" \
        "bootcmd=bootm e0000000\0" \
        "bootargs=console=ttyS0,115200 root=/dev/mtdblock1 rw rootfstype=jffs2 ip=off\0" \
        "bootcmd1=setenv bootcmd bootm E0000000\0" \
        "bootargs1=setenv bootargs console=ttyS0,115200 root=/dev/mtdblock1 rw rootfstype=jffs2 ip=off\0" \
        "bootcmd2=setenv bootcmd bootm E3F00000\0" \
        "bootargs2=setenv bootargs console=ttyS0,115200 root=/dev/mtdblock3 rw rootfstype=jffs2 ip=off\0" \
        ""

#define CONFIG_BOOTDELAY		3 /* autoboot after 5 seconds */
#define CONFIG_LOADS_ECHO		1 /* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1 /* allow baudrate change */

/*
 * Ethernet/EMAC/PHY
 */
#define CONFIG_PPC4xx_EMAC
#define	CONFIG_IBM_EMAC4_V4		/* 440SPe has this EMAC version */
#define CONFIG_ID_EEPROM 		/* Read ethernet params from not volatile memory */
#define CONFIG_HAS_ETH0
#define CONFIG_FIXED_PHY		0xFFFFFFFF
#define CONFIG_PHY_ADDR			CONFIG_FIXED_PHY
#define CONFIG_MII			/* MII PHY management */
#define CONFIG_NET_MULTI

#define CONFIG_SYS_FIXED_PHY_PORT(devnum, speed, duplex) {devnum, speed, duplex}
#define CONFIG_SYS_FIXED_PHY_PORTS	CONFIG_SYS_FIXED_PHY_PORT(0, 100, FULL)

#define CONFIG_PHY_RESET		1 /* reset phy upon startup */
#define CONFIG_PHY_RESET_DELAY		1000
#define CONFIG_PHY_CMD_DELAY		40
#define CONFIG_SYS_RX_ETH_BUFFER	32 /* number of eth rx buffers */

/*
 * Commands
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DFL
#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DIAG
#define CONFIG_CMD_ELF
#define CONFIG_CMD_I2C
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_MII
#define CONFIG_CMD_PCI
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_CACHE

/*
 * Miscellaneous configurable options
 */

#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_PROMPT		"=> " /* Monitor Command Prompt */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE		1024 /* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE		256 /* Console I/O Buffer Size */
#endif

#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS		16 /* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE /* Boot Argument Buffer Size */

#define CONFIG_SYS_MEMTEST_START	0x0400000 /* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x0C00000 /* 4 ... 12 MB in DRAM */

#define CONFIG_SYS_LOAD_ADDR		0x100000  /* default load address */
#define CONFIG_SYS_EXTBDINFO		1 /* To use extended board_into (bd_t) */

#define CONFIG_SYS_HZ			1000 /* decrementer freq: 1 ms ticks */

#define CONFIG_CMDLINE_EDITING		/* add command line history */
#define CONFIG_AUTO_COMPLETE		/* add autocompletion support */
#define CONFIG_LOOPW			/* enable loopw command */
#define CONFIG_MX_CYCLIC		/* enable mdc/mwc commands */
#define CONFIG_ZERO_BOOTDELAY_CHECK	/* check for keypress on bootdelay==0 */
#define CONFIG_VERSION_VARIABLE 	/* include version env variable */
#define CONFIG_SYS_CONSOLE_INFO_QUIET	/* don't print console @ startup */


/*-----------------------------------------------------------------------
 * FLASH related
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_MAX_FLASH_BANKS	2 /* max number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	1024 /* max sectors per device */
#undef	CONFIG_SYS_FLASH_PROTECTION
#undef	CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT	120000 /* Timeout for Flash Erase (in ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	9000 /* Timeout for Flash Write (in ms) */

#ifdef CONFIG_ENV_IS_IN_FLASH

#define CONFIG_ENV_SECT_SIZE		0x10000 /* size of one complete sector */
#define CONFIG_ENV_SIZE			0x10000 /* Size of Environment vars */
#define CONFIG_ENV_ADDR			(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_OFFSET		(CONFIG_ENV_ADDR - CONFIG_SYS_FLASH_BASE) */

#define CONFIG_ENV_REDUND
#define CONFIG_ENV_ADDR_REDUND		(CONFIG_ENV_ADDR - CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND		CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)

#define VPD_BASE_ADDR			0x4E7E00000ull


#endif /* CONFIG_ENV_IS_IN_FLASH */


/*-----------------------------------------------------------------------
 * PCI stuff
 *-----------------------------------------------------------------------
 */
/* General PCI */
#define CONFIG_PCI			/* include pci support */
#define CONFIG_PCI_PNP			1 /* do pci plug-and-play */
#define CONFIG_PCI_SCAN_SHOW		1 /* show pci devices on startup */

/* Board-specific PCI */
#define CONFIG_SYS_PCI_TARGET_INIT	/* let board init pci target */
#undef	CONFIG_SYS_PCI_MASTER_INIT

#define CONFIG_SYS_PCI_SUBSYS_VENDORID	0x1014 /* IBM */
#define CONFIG_SYS_PCI_SUBSYS_DEVICEID	0xcafe /* Whatever */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ		(8 << 20) /*Initial Memory map for Linux */

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD			0x01 /* Normal Power-On: Boot from FLASH */
#define BOOTFLAG_WARM			0x02 /* Software reboot */


/*----------------------------------------------------------------------------+
| Defines
+----------------------------------------------------------------------------*/
#define PERIOD_133_33MHZ		7500 /* 7,5ns */
#define PERIOD_100_00MHZ		10000 /* 10ns */
#define PERIOD_83_33MHZ			12000 /* 12ns */
#define PERIOD_75_00MHZ			13333 /* 13,333ns */
#define PERIOD_66_66MHZ			15000 /* 15ns */
#define PERIOD_50_00MHZ			20000 /* 20ns */
#define PERIOD_33_33MHZ			30000 /* 30ns */
#define PERIOD_25_00MHZ			40000 /* 40ns */

#endif /* __CONFIG_H */
