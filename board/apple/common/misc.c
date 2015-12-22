/*
 * (C) Copyright 2009 Apple Computer
 */

#include <common.h>
#include <ppc4xx.h>
#include <i2c.h>
#include <netdev.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/4xx_pcie.h>

DECLARE_GLOBAL_DATA_PTR;

#define DEBUG_ENV
#ifdef DEBUG_ENV
#define DEBUGF(fmt,args...) printf(fmt ,##args)
#else
#define DEBUGF(fmt,args...)
#endif

#define ALTERNATE_BOOT_FLAG_OFFSET  0x10
#define SDRN_PESDR_DLPSET_MASK      0x00400000
#define EEPROM_PRIMARY_OFFSET       0x00000000
#define EEPROM_SECONDARY_OFFSET     0x00040000
#define EEPROM_PCIE_MASK            0xFFFC0000 /* 256K */

extern int AMCC440Spe_init_pcie_endpoint(void);
extern int AMCC440Spe_setup_pcie_endpoint(struct pci_controller *hose);


/*
 * This function returns the OS/filesystem boot partition
 */
static unsigned char get_boot_partition(void)
{
	unsigned char bootflag = 0xFF;

	if (i2c_read(IIC0_ALT_BOOTPROM_ADDR, ALTERNATE_BOOT_FLAG_OFFSET, 1, &bootflag, 1) != 0)
	{
		printf("ERR: Failed accessing I2C read of partition ID.\n");
	}
	return bootflag;
}

/*
 * This function returns location and size information about the "ROM" data
 */
void get_rom_info(ulong * up,ulong * low, ulong * mask)
{
	unsigned char bootid;

	bootid =  get_boot_partition();

	switch (bootid)
	{
		case 0:
			*up = CONFIG_SYS_TLB_FLASH_MEMBASE_HIGH;
			*low = CONFIG_SYS_TLB_FLASH_MEMBASE_LOW + EEPROM_PRIMARY_OFFSET;
			*mask = EEPROM_PCIE_MASK;
			break;
		case 1:
			*up = CONFIG_SYS_TLB_FLASH_MEMBASE_HIGH;
			*low = CONFIG_SYS_TLB_FLASH_MEMBASE_LOW + EEPROM_SECONDARY_OFFSET;
			*mask = EEPROM_PCIE_MASK;
			break;
		default:

			printf("ERR: Booting partition out of range [%d].\n", bootid);
			*up = 0;
			*low = 0;
			*mask = 0;
			break;
	}
}

#if defined(CONFIG_PCI)

/*************************************************************************
 *  pci_pre_init
 *
 *  This routine is called just prior to registering the hose and gives
 *  the board the opportunity to check things. Returning a value of zero
 *  indicates that things are bad & PCI initialization should be aborted.
 *
 * Different boards may wish to customize the pci controller structure
 * (add regions, override default access routines, etc) or perform
 * certain pre-initialization actions.
 *
 ************************************************************************/
int pci_pre_init(struct pci_controller * hose )
{
	unsigned long strap;

	mfsdr(sdr_sdstp1, strap);
	if( (strap & SDR0_SDSTP1_PAE_MASK) == 0 ) {
		printf("PCI: SDR0_STRP1[%08lX] - PCI Arbiter disabled.\n",strap);
		return 0;
	}

	return 1;
}

/*************************************************************************
 *  is_pci_host
 *
 * This routine is called to determine if a pci scan should be
 * performed. With various hardware environments (especially cPCI and
 * PPMC) it's insufficient to depend on the state of the arbiter enable
 * bit in the strap register, or generic host/adapter assumptions.
 *
 * Rather than hard-code a bad assumption in the general 440 code, the
 * 440 pci code requires the board to decide at runtime.
 *
 * Return 0 for adapter mode, non-zero for host (monarch) mode.
 *
 *
 ************************************************************************/
int is_pci_host(struct pci_controller *hose)
{
	/* The K46 board is always configured as host. */
	return 1;
}

void get_pcie_rev(unsigned int * rev)
{
	char *dataPtr;

	dataPtr = (char *)VPD_BASE_ADDR;

	*rev = 0;

	if (dataPtr != NULL)
	{
		char buffer[4];
		int i;

		dataPtr += 0x30;

		sprintf(buffer, "%c%c%c", dataPtr[0], dataPtr[2], dataPtr[3]);

		for( i = 0; i < 3; i++)
		{
			*rev *= 10;
			*rev += ((*(buffer + i))-'0');
		}
	}
}

static struct pci_controller pcie_hose[1] = {{0}};

/*
 * NOTE: K46 is a PCIe end point only.
 */
void pcie_setup_hoses(int busno)
{
	struct pci_controller *hose;

	/*
	 * assume we're called after the PCIX hose is initialized, which takes
	 * bus ID 0 and therefore start numbering PCIe's from 1.
	 */
	if (AMCC440Spe_init_pcie_endpoint() == 0)
	{
		hose = &pcie_hose[0];
		hose->first_busno = busno;
		hose->last_busno = busno;
		hose->current_busno = busno;

		/* setup mem resource */
		pci_set_region(hose->regions,
			CONFIG_SYS_PCIE_MEMBASE,
			CONFIG_SYS_PCIE_MEMBASE,
			CONFIG_SYS_PCIE_MEMSIZE,
			PCI_REGION_MEM);
		hose->region_count = 1;

		pci_register_hose(hose);

		AMCC440Spe_setup_pcie_endpoint(hose);
	}
	else
	{
		printf("PCIE0: initialization as endpoint failed\n");
	}
}

#if defined(CONFIG_SYS_PCI_TARGET_INIT)

/*************************************************************************
 *  pci_target_init
 *
 * The bootstrap configuration provides default settings for the pci
 * inbound map (PIM). But the bootstrap config choices are limited and
 * may not be sufficient for a given board.
 *
 ************************************************************************/
void pci_target_init(struct pci_controller * hose )
{
	/*-------------------------------------------------------------------+
	 * Disable everything
	 *-------------------------------------------------------------------*/
	out32r( PCIX0_PIM0SA, 0 ); /* disable */
	out32r( PCIX0_PIM1SA, 0 ); /* disable */
	out32r( PCIX0_PIM2SA, 0 ); /* disable */
	out32r( PCIX0_EROMBA, 0 ); /* disable expansion rom */

	/*-------------------------------------------------------------------+
	 * Map all of SDRAM to PCI address 0x0000_0000. Note that the 440
	 * strapping options to not support sizes such as 128/256 MB.
	 *-------------------------------------------------------------------*/
	out32r( PCIX0_PIM0LAL, CONFIG_SYS_SDRAM_BASE );
	out32r( PCIX0_PIM0LAH, 0 );
	out32r( PCIX0_PIM0SA, ~(gd->ram_size - 1) | 1 );
	out32r( PCIX0_BAR0, 0 );

	/*-------------------------------------------------------------------+
	 * Program the board's subsystem id/vendor id
	 *-------------------------------------------------------------------*/
	out16r( PCIX0_SBSYSVID, CONFIG_SYS_PCI_SUBSYS_VENDORID );
	out16r( PCIX0_SBSYSID, CONFIG_SYS_PCI_SUBSYS_DEVICEID );

	out16r( PCIX0_CMD, in16r(PCIX0_CMD) | PCI_COMMAND_MEMORY );
}

#endif	/* defined(CONFIG_SYS_PCI_TARGET_INIT) */

#endif /* defined(CONFIG_PCI) */

#if defined(CONFIG_MISC_INIT_R)

extern void ecc_init_post(unsigned long * const start, unsigned long size);
extern void board_add_ram_info(int use_default);

/*
 * For this specific platform this function has been used to take care of the
 * dual boot setting and DRAM ECC final sweeping.
 *
 * This function must be called after relocation.
 */
int misc_init_r (void)
{
	char *s0 = "/0";

	s0 = getenv ("dualboot");

#if defined(CONFIG_DDR_ECC)

	/*
	 * This code has been added to complete DRAM ECC which was
	 * interrupted to anticipate the PCIe initialization.
	 */
	ecc_init_post(CONFIG_SYS_SDRAM_BASE, CONFIG_SYS_MBYTES_SDRAM << 20);
	printf ("DRAM:  %d MB", CONFIG_SYS_MBYTES_SDRAM);
	board_add_ram_info(1);
	printf ("\n");

#endif /* defined(CONFIG_DDR_ECC) */

	if((int)simple_strtol(s0, NULL, 10) == 1)
	{
		unsigned char bootflag;
		char *s1;
		char *s2;

		bootflag = get_boot_partition();

		switch (bootflag)
		{
			case 0:
				s2 = getenv ("bootargs1");
				run_command (s2, 0);
				s1 = getenv ("bootcmd1");
				run_command (s1, 0); 
				printf("DBOOT: Booting off primary partition [0]\n");
				break;
			case 1:
				s2 = getenv ("bootargs2");
				run_command (s2, 0);
				s1 = getenv ("bootcmd2");
				run_command (s1, 0); 
				printf("DBOOT: Booting off alternate partition [1].\n");
				break;
			default:
				printf("ERR: Booting partition out of range [%d].\n", bootflag);
				break;
		}
	}
	else {
		printf("DBOOT: 'setenv dualboot 1' if need a dual boot system.\n");
	}
	return 0;
}

#endif /* CONFIG_MISC_INIT_R */

#if defined(CONFIG_MISC_INIT_F)

int misc_init_f (void)
{
	uint reg;

	/* minimal init for PCIe */
	/* pci express 0 Endpoint Mode */
	mfsdr(SDRN_PESDR_DLPSET(0), reg);
	reg &= (~SDRN_PESDR_DLPSET_MASK);
	mtsdr(SDRN_PESDR_DLPSET(0), reg);
	/* pci express 1 Rootpoint  Mode */
	mfsdr(SDRN_PESDR_DLPSET(1), reg);
	reg &= (~SDRN_PESDR_DLPSET_MASK);
	mtsdr(SDRN_PESDR_DLPSET(1), reg);
	/* pci express 2 Rootpoint  Mode */
	mfsdr(SDRN_PESDR_DLPSET(2), reg);
	reg |= SDRN_PESDR_DLPSET_MASK;
	mtsdr(SDRN_PESDR_DLPSET(2), reg);

	return 0;
}

#endif /* defined(CONFIG_MISC_INIT_F) */

#if defined(CONFIG_ID_EEPROM)

/*
 * This function reads the MAC addresses from non volatile memory and
 * sets the appropriate environment variables for each one read.
 *
 * The environment variables are only set if they haven't been set already.
 * This ensures that any user-saved variables are never overwritten.
 *
 * This function must be called after relocation.
 */
int mac_read_from_eeprom(void) 
{
	char buffer[32];
	char *dataPtr;

	dataPtr = (char *)VPD_BASE_ADDR;

	sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
		dataPtr[0x34],
		dataPtr[0x35],
		dataPtr[0x36],
		dataPtr[0x37],
		dataPtr[0x38],
		dataPtr[0x39]);

	if (getenv ("ethaddr") == NULL)
	{
		setenv("ethaddr", buffer);
	}

	if (strcmp(getenv ("ethaddr"), buffer))
	{
		printf ("MAC:   %s\n", getenv ("ethaddr"));
	}
	else
	{
		printf ("MAC:   %s (VPD setting)\n", buffer);
	}
	return 0;
}

/*
 * This function needs to be implemented if MAC data is stored in an EEPROM
 * K46 doesn't really uses a EEPROM therefore this function is just a place
 * holder.
 *
 * This function must be called after relocation.
 */
int do_mac(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("This command is not supported.\n");

	return 0;
}

#endif /* CONFIG_ID_EEPROM */

extern void ecc_init_post(unsigned long * const start, unsigned long size);
extern void board_add_ram_info(int use_default);

void ecc_complete(void)
{
	ecc_init_post(CONFIG_SYS_SDRAM_BASE, CONFIG_SYS_MBYTES_SDRAM << 20);
	printf ("DRAM:  %d MB", CONFIG_SYS_MBYTES_SDRAM);
	board_add_ram_info(1);
	printf ("\n");
}
