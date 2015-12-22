/*
 * (C) Copyright 2009 Apple Computer
 */

#include <common.h>

#if defined(CONFIG_440SPE) && defined(CONFIG_PCI) && !defined(CONFIG_PCI_DISABLE_PCIE)

#include <asm/4xx_pcie.h>
#include <asm/processor.h>
#include <asm-ppc/io.h>

/*
 * Defines not declared in <asm/4xx_pcie.h>
 */

#define PECFGn_BAR0L			0x010
#define PECFGn_BAR0H			0x014
#define PECFGn_BAR2L			0x020
#define PECFGn_BAR2H			0x024
#define PECFG_EROMBA			0x030
#define PECFG_EROMBAPA			0x230
#define PECFG_CAPPA			0x234
#define PECFG_PIM3LAL			0x360
#define PECFG_PIM3LAH			0x364
#define PECFG_PIM34SAL			0x370
#define PECFG_PIM34SAH			0x374
#define PECFG_PIM5LAL			0x378
#define PECFG_PIM5LAH 			0x37c
#define PECFG_POM2LAL			0x390
#define PECFG_POM2LAH			0x394

#define DCRN_PEGPL_OMR3BAH(base)	(base + 0x0E)
#define DCRN_PEGPL_OMR3BAL(base)	(base + 0x0F)
#define DCRN_PEGPL_OMR3MSKH(base)	(base + 0x10)
#define DCRN_PEGPL_OMR3MSKL(base)	(base + 0x11)

#define PEUTL_OPHBSZ			0x60
#define PEUTL_ONHBSZ			0x80
#define PEUTL_INHBSZ			0x88

/*
 * PCIe settings
 */

#define PCIE_VENDOR_ID 0x106b
#define PCIE_DEVICE_ID 0x008a
#define PCIE_CLASS_CODE 0x010400
#define PCIE_MAX_READ 1024
#define PCIE_MAX_PAYLOAD 512

enum {
	PTYPE_ENDPOINT = 0x0,
	PTYPE_LEGACY_ENDPOINT = 0x1,
	PTYPE_ROOT_PORT = 0x4,

	LNKW_X1 = 0x1,
	LNKW_X4 = 0x4,
	LNKW_X8 = 0x8
};

/*
 * External functions
 */

extern int pcie_read_config_byte(struct pci_controller *hose,pci_dev_t dev,int offset,u8 *val);
extern int pcie_read_config_word(struct pci_controller *hose,pci_dev_t dev,int offset,u16 *val);
extern int pcie_read_config_dword(struct pci_controller *hose,pci_dev_t dev,int offset,u32 *val);
extern int pcie_write_config_byte(struct pci_controller *hose,pci_dev_t dev,int offset,u8 val);
extern int pcie_write_config_word(struct pci_controller *hose,pci_dev_t dev,int offset,u16 val);
extern int pcie_write_config_dword(struct pci_controller *hose,pci_dev_t dev,int offset,u32 val);


/*
 * Board-specific PCIe Platform code can reimplement get_pcie_vendor_specific_data () if needed
 */
void inline __get_pcie_rev (unsigned int * rev) {}
void inline get_pcie_rev (unsigned int * rev) __attribute__((weak, alias("__get_pcie_rev")));

void inline __get_rom_info(ulong * up, ulong * low, ulong * mask) {}
void inline get_rom_info(ulong * up, ulong * low, ulong * mask) __attribute__((weak, alias("__get_rom_info")));

/*
 * Exported functions
 */

/*
 * Initialize various parts of the PCI Express core for our port:
 *
 * - Set as a root port and enable max width
 *   (PXIE0 -> X8, PCIE1 and PCIE2 -> X4).
 * - Set up UTL configuration.
 * - Increase SERDES drive strength to levels suggested by AMCC.
 * - De-assert RSTPYN, RSTDL and RSTGU.
 *
 * NOTICE for 440SPE revB chip: PESDRn_UTLSET2 is not set - we leave it
 * with default setting 0x11310000. The register has new fields,
 * PESDRn_UTLSET2[LKINE] in particular: clearing it leads to PCIE core
 * hang.
 */
int ppc4xx_init_pcie_port_hw(int port, int rootport)
{
	u32 val = 0;
	u32 utlset1;

	/* See comment about turning scrambling on in 440spe manual,
	   it is suggested to start with scrambling on, when the link
	   is configured this setting will be useless.
	*/
	val = 1 << 24;

	if (rootport) {
		val = PTYPE_ROOT_PORT << 20;
		utlset1 = 0x21222222;
	} else {
		val = PTYPE_LEGACY_ENDPOINT << 20;
		utlset1 = 0x20121121; /* Used to be 0x21222222 */
	}

	if (port == 0) {
		val |= LNKW_X8 << 12;
	}
	else {
		val |= LNKW_X4 << 12;
	}

	SDR_WRITE(SDRN_PESDR_DLPSET(port), val);
	SDR_WRITE(SDRN_PESDR_UTLSET1(port), utlset1);

	SDR_WRITE(SDRN_PESDR_HSSL0SET1(port), 0xE7000000);
	SDR_WRITE(SDRN_PESDR_HSSL1SET1(port), 0xE7000000);
	SDR_WRITE(SDRN_PESDR_HSSL2SET1(port), 0xE7000000);
	SDR_WRITE(SDRN_PESDR_HSSL3SET1(port), 0xE7000000);

	if (port == 0) {
		SDR_WRITE(PESDR0_HSSL4SET1, 0xE7000000);
		SDR_WRITE(PESDR0_HSSL5SET1, 0xE7000000);
		SDR_WRITE(PESDR0_HSSL6SET1, 0xE7000000);
		SDR_WRITE(PESDR0_HSSL7SET1, 0xE7000000);
	}

	SDR_WRITE(SDRN_PESDR_RCSSET(port), (SDR_READ(SDRN_PESDR_RCSSET(port)) &
	    ~(PESDRx_RCSSET_RSTGU | PESDRx_RCSSET_RSTDL)) | PESDRx_RCSSET_RSTPYN);

	return 0;
}

/*
 * This function initialized the PCIe port 0 as an endpoint.
 */
int AMCC440Spe_init_pcie_endpoint(void)
{
	static int core_init = 0;
	volatile void *utl_base = NULL;
	volatile u32 val = 0;
	int attempts;
	int rc = -1;

	if (!core_init) {
		if (ppc4xx_init_pcie()) {
			goto exit;
		}
		++core_init;
	}

	/*
	 * Initialize various parts of the PCI Express core for our port
	 */
	ppc4xx_init_pcie_port_hw(0, 0);

	/*
	 * Notice: the following delay has critical impact on device
	 * initialization - if too short (<50ms) the link doesn't get up.
	 */
	mdelay(100);

	val = SDR_READ(SDRN_PESDR_RCSSTS(0));
	if (val & PESDRx_RCSSTS_PGRST) {
		printf("PCIE0: PGRST failed %08x\n", val);
		goto exit;
	}

#define PPC4XX_INIT_LINK_UP_TIMEOUT 50

	/*
	 * Verify link is up
	 */
	attempts = PPC4XX_INIT_LINK_UP_TIMEOUT;
	do {
		val = SDR_READ(SDRN_PESDR_LOOP(0));
		if ((val & 0x00001000)) {
			break;
		}
		mdelay(10);
	} while (--attempts);

	if (attempts == 0)
	{
		printf("PCIE0: link is not up after %d ms.\n", PPC4XX_INIT_LINK_UP_TIMEOUT * 10);
		return -1;
	}

	/*
	 * Map UTL registers
	 */
	mtdcr(DCRN_PEGPL_REGBAH(PCIE0), CONFIG_SYS_TLB_PCIE_REGBASE_HIGH);
	mtdcr(DCRN_PEGPL_REGBAL(PCIE0), CONFIG_SYS_TLB_PCIE1_REGBASE_LOW);
	mtdcr(DCRN_PEGPL_REGMSK(PCIE0), 0x00007001);
	mtdcr(DCRN_PEGPL_SPECIAL(PCIE0), 0x68782800);

	utl_base = (unsigned int *)(CONFIG_SYS_PCIE1_REGBASE);

	out_be32(utl_base + PEUTL_PBCTL,   0x08000000);
	out_be32(utl_base + PEUTL_OUTTR,   0x04000000);
	out_be32(utl_base + PEUTL_INTR,    0x04000000);
	out_be32(utl_base + PEUTL_OPDBSZ,  0x10000000);
	out_be32(utl_base + PEUTL_OPHBSZ,  0x04000000);
	out_be32(utl_base + PEUTL_PBBSZ,   0x42000000);
	out_be32(utl_base + PEUTL_IPHBSZ,  0x04000000);
	out_be32(utl_base + PEUTL_IPDBSZ,  0x10000000);
	out_be32(utl_base + PEUTL_ONHBSZ,  0x04000000);
	out_be32(utl_base + PEUTL_INHBSZ,  0x04000000);
	out_be32(utl_base + PEUTL_RCIRQEN, 0x00f00000);
	out_be32(utl_base + PEUTL_PCTL,    0x80800066);

	/*
	 * We map PCI Express configuration access into the 512MB regions
	 */
	mtdcr(DCRN_PEGPL_CFGBAH(PCIE0), CONFIG_SYS_TLB_PCIE_CFGBASE_HIGH);
	mtdcr(DCRN_PEGPL_CFGBAL(PCIE0), 0x00000000); /* CONFIG_SYS_TLB_PCIE0_CFGBASE_LOW); */
	mtdcr(DCRN_PEGPL_CFGMSK(PCIE0), 0xe0000001); /* 512MB region, valid */

	/*
	 * Check for VC0 active and assert RDY.
	 */
	{
		int attempts = 10;

		while(!(SDR_READ(SDRN_PESDR_RCSSTS(0)) & PESDRx_RCSSTS_VC0ACT)) {
			if (!(attempts--)) {
				printf("PCIE0: VC0 not active\n");
				goto exit;
			}
			mdelay(1000);
		}
	}

	rc = 0;
exit:

	return rc;
}

/*
 * This function initializes the PCIe module as an endpoint and it is
 * tailored to the Apple cards.
 */
int AMCC440Spe_setup_pcie_endpoint(struct pci_controller *hose)
{
	volatile void *mbase = NULL;
	int rc = -1;

	pci_set_ops(hose,
		    pcie_read_config_byte,
		    pcie_read_config_word,
		    pcie_read_config_dword,
		    pcie_write_config_byte,
		    pcie_write_config_word,
		    pcie_write_config_dword);

	mbase = (volatile void *)CONFIG_SYS_PCIE0_XCFGBASE;
	hose->cfg_data = (volatile unsigned char *)CONFIG_SYS_PCIE0_CFGBASE;

	/*
	 * Set Vendor ID, Device ID, Class and Rev.
	 */

	{
		unsigned int rev = 0;

		get_pcie_rev(&rev);

		/* Set Device and Vendor Id */
		out_le16(mbase + 0x200, PCIE_VENDOR_ID);
		out_le16(mbase + 0x202, PCIE_DEVICE_ID);

		/* Set Class Code to PCI-PCI bridge and Revision Id */
		out_le32(mbase + 0x208, (PCIE_CLASS_CODE << 8) + rev);
	}

	/*
	 * Set Max packet and transfer sizes.
	 */

	{
		unsigned int ecdevctl = 0;
		unsigned int ecdevcappa = 0;

		switch(PCIE_MAX_READ)
		{
			case 128:
				ecdevctl = 0x0000;
				break;
			case 256:
				ecdevctl = 0x1000;
				break;
			case 512:
				ecdevctl = 0x2000;
				break;
			case 1024:
				ecdevctl = 0x3000;
				break;
			default:
				return -1;
		}

		switch(PCIE_MAX_PAYLOAD)
		{
			case 128:
				ecdevctl |= 0x0000;
				ecdevcappa = 0x0000;
				break;
			case 256:
				ecdevctl |= 0x0020;
				ecdevcappa = 0x0001;
				break;
			case 512:
				ecdevctl |= 0x0040;
				ecdevcappa = 0x0002;
				break;
			default:
				return -1;
		}

		out_le16(mbase + 0x60, ecdevctl);
		out_le16(mbase + 0x25c, ecdevcappa);
	}

	/********************************************
	* PCIe INBOUND                              *
	********************************************/

	/* UltraSlice */

	{
               /* BAR0 here is BAR2 (LMS SRAM) of Ultraslice which is programmed for inbound. */

		out_le32(mbase + PECFGn_BAR0L, 0x00000000);
		out_le32(mbase + PECFGn_BAR0H, 0x00000000);
		out_le32(mbase + PECFG_BAR0HMPA, 0xffffffff);
		out_le32(mbase + PECFG_BAR0LMPA, 0xfff00004); /* 1MB */

		out_le32(mbase + PECFG_PIM01SAH, 0xffffffff);
		out_le32(mbase + PECFG_PIM01SAL, 0xffff0000);  /* 64KB */

		out_le32(mbase + PECFG_PIM0LAL, CONFIG_SYS_TLB_PCIE_MEMBASE_LOW + 0x10000);
		out_le32(mbase + PECFG_PIM0LAH, CONFIG_SYS_TLB_PCIE_MEMBASE_HIGH);
		out_le32(mbase + PECFG_PIM1LAL, CONFIG_SYS_TLB_PCIE_MEMBASE_LOW);
		out_le32(mbase + PECFG_PIM1LAH, CONFIG_SYS_TLB_PCIE_MEMBASE_HIGH);


		out_le32(mbase + PECFG_BAR1MPA, 0x00000000);
		out_le32(mbase + PECFG_BAR2LMPA, 0x00000000);
		out_le32(mbase + PECFG_BAR2HMPA, 0x00000000);
	}

	/* Expansion ROM */

	{
		ulong up;
		ulong low;
		ulong mask;

		get_rom_info(&up, &low, &mask);

		out_le32(mbase + PECFG_EROMBA, 0x00000000);
		out_le32(mbase + PECFG_EROMBAPA, mask);
		out_le32(mbase + PECFG_PIM5LAH, up);
		out_le32(mbase + PECFG_PIM5LAL, low);
	}

	/********************************************
	* PCIe OUTBOUND                             *
	********************************************/

	/* UltraSlice */

	{
               /* Only Outbound from PCIe is needed. Root never does a read
		  or write in this ultraslice bar but uses his local PLB
		  address.  Ultraslice Host mem BAR, which would be PCI-X0
		  BAR2, now it's using PCIX0 BAR0, ultraslice BAR2.
               */

		/* Assuming we would not do DMA from Ultraslice to host at
		  this point in BIOS, until linux is up and PCIX configured.
		*/

		/* We could not redirect PCIX bar for ultraslice for DMAing
		  in high PCI address as PCIX POM,s could not be programmed
		  in UBOOT.
		*/

		out_le32(mbase + PECFG_POM2LAH, 0x00000000);
		out_le32(mbase + PECFG_POM2LAL, 0x00000000); /* 2GB offset. */
		mtdcr(DCRN_PEGPL_OMR3BAH(PCIE0),  0x20000000);
		mtdcr(DCRN_PEGPL_OMR3BAL(PCIE0),  0x00000000);
		mtdcr(DCRN_PEGPL_OMR3MSKH(PCIE0), 0xfffffff0);
		mtdcr(DCRN_PEGPL_OMR3MSKL(PCIE0), 0x00000001); /* 64GB, whether memory region 3 is valid. */
	}

	SDR_WRITE(PESDR0_RCSSET, SDR_READ(PESDR0_RCSSET) | PESDRx_RCSSET_RDY);        
	out_le32(mbase + PECFG_PIMEN, in_le32(mbase + PECFG_PIMEN) | 0x09); /* enable BAR0 & BAR3. */

	/* Enable I/O, Mem, and Busmaster cycles, disable legacy INTx */
	out_le16((u16 *)(mbase + PCI_COMMAND),
		 in_le16((u16 *)(mbase + PCI_COMMAND)) |
		 PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | 0x400);

	{
		int attempts = 10;

		while(!(SDR_READ(SDRN_PESDR_RCSSTS(0)) & PESDRx_RCSSTS_BMEN)) {
			if (!(attempts--)) {
				printf("PCIE: BME not active\n");
				goto exit;
			}
			mdelay(1000);
		}
	}

	rc = 0;
	printf("PCIE0: successfully set as endpoint\n");

exit:

	return 0;
}

#endif /* (defined(CONFIG_440SPE) && defined(CONFIG_PCI) && !defined(CONFIG_PCI_DISABLE_PCIE) */
