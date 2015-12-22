/*
 * (C) Copyright 2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002 Jun Gu <jung@artesyncp.com>
 * Add support for Am29F016D and dynamic switch setting.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Modified 4/5/2001
 * Wait for completion of each sector erase command issued
 * 4/5/2001
 * Chris Hallinan - DS4.COM, Inc. - clh@net1plus.com
 */

#include <common.h>
#include <ppc4xx.h>
#include <asm/processor.h>
#include <ppc440.h>

#ifdef DEBUG_FLASH
#define DEBUGF(x...) printf(x)
#else
#define DEBUGF(x...)
#endif /* DEBUG_FLASH */

/*----------------------------------------------------------------------------+
| SDR Configuration registers
+----------------------------------------------------------------------------*/

#define	SDR0_SDSTP1_EBC_ROM_BS_MASK	0x00000080 /* EBC Boot bus width Mask */
#define	SDR0_SDSTP1_EBC_ROM_BS_16BIT	0x00000080 /* EBC 16 Bits */
#define	SDR0_SDSTP1_EBC_ROM_BS_8BIT	0x00000000 /* EBC  8 Bits */

#define	SDR0_SDSTP1_BOOT_SEL_MASK	0x00080000 /* Boot device Selection Mask */
#define	SDR0_SDSTP1_BOOT_SEL_EBC	0x00000000 /* EBC */
#define	SDR0_SDSTP1_BOOT_SEL_PCI	0x00080000 /* PCI */

/* Serial Device Strap Reg 0 */
#define sdr_pstrp0	0x0040

/* Pin Straps Reg */
#define SDR0_PSTRP0_BOOTSTRAP_MASK	0xE0000000  /* Strap Bits */

#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS0	0x00000000  /* Default strap settings 0 */
#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS1	0x20000000  /* Default strap settings 1 */
#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS2	0x40000000  /* Default strap settings 2 */
#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS3	0x60000000  /* Default strap settings 3 */
#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS4	0x80000000  /* Default strap settings 4 */
#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS5	0xA0000000  /* Default strap settings 5 */
#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS6	0xC0000000  /* Default strap settings 6 */
#define SDR0_PSTRP0_BOOTSTRAP_SETTINGS7	0xE0000000  /* Default strap settings 7 */

/*----------------------------------------------------------------------------+
| Flash manufacturer and device IDs not present in the common include file
+----------------------------------------------------------------------------*/

#define FLASH_MAN_NUMONYX		0x02000000 /* Numonyx manufacturer */
#define NUMONYX_ID_W800T		0x22D722D7 /* Numonyx M29W800DT ID ( 8 Mbit, top boot sector) */
#define NUMONYX_ID_W800B		0x225B225B /* Numonyx M29W800DB ID ( 8 Mbit, bottom boot sect) */

#define CONFIG_SYS_FLASH_1_ADDR0	0xAAA
#define CONFIG_SYS_FLASH_1_ADDR1	0x555
#define CONFIG_SYS_FLASH_1_WORD_SIZE	unsigned char

#define CONFIG_SYS_FLASH_2_ADDR0	0x555
#define CONFIG_SYS_FLASH_2_ADDR1	0x2AA
#define CONFIG_SYS_FLASH_2_WORD_SIZE	unsigned short

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];	/* info for FLASH chips */

/*
 * Mark big flash bank (16 bit instead of 8 bit access) in address with bit 0
 */
static unsigned long flash_addr_table[][CONFIG_SYS_MAX_FLASH_BANKS] = {
	{CONFIG_SYS_FLASH_BASE, CONFIG_SYS_FLASH_2_BASE | 0x1}, /* 0:boot from small flash */
	{0x00000000, 0x00000000}, /* 1:boot from pci 66 */
	{0x00000000, 0x00000000}, /* 2:boot from nand flash  */
	{0xe7F00000, 0xFFC00001}, /* 3:boot from big flash 33*/
	{0xe7F00000, 0xFFC00001}, /* 4:boot from big flash 66*/
	{0x00000000, 0x00000000}, /* 5:boot from */
	{0x00000000, 0x00000000}, /* 6:boot from pci 66 */
	{0x00000000, 0x00000000}, /* 7:boot from */
	{CONFIG_SYS_FLASH_BASE, CONFIG_SYS_FLASH_2_BASE | 0x1} /* 8:boot from small flash */
};



	/***************************
	  LOCAL FUNCTION PROTOTYPES
	***************************/


static int flash_print_basic_info(flash_info_t * info);
static int write_word(flash_info_t * info, ulong dest, ulong data);
static int write_word_1(flash_info_t * info, ulong dest, ulong data);
static int write_word_2(flash_info_t * info, ulong dest, ulong data);
static int flash_erase_1(flash_info_t * info, int s_first, int s_last);
static int flash_erase_2(flash_info_t * info, int s_first, int s_last);
static ulong flash_get_size(vu_long * addr, flash_info_t * info);
static ulong flash_get_size_1(vu_long * addr, flash_info_t * info);
static ulong flash_get_size_2(vu_long * addr, flash_info_t * info);
static int wait_for_DQ7_1(flash_info_t * info, int sect);
static int wait_for_DQ7_2(flash_info_t * info, int sect);


	/******************
	  GLOBAL FUNCTIONS
	******************/


unsigned long flash_init(void)
{
	unsigned long total_b = 0;
	unsigned long size_b[CONFIG_SYS_MAX_FLASH_BANKS];
	unsigned short index = 0;
	int i;
	unsigned long val;
	unsigned long ebc_boot_size;
	unsigned long boot_selection;

	mfsdr(sdr_pstrp0, val);
	index = (val & SDR0_PSTRP0_BOOTSTRAP_MASK) >> 28;

	if ((index == 0xc) || (index == 8)) {
		/*
		 * Boot Settings in IIC EEprom address 0xA8 or 0xA0
		 * Read Serial Device Strap Register1 in PPC440SPe
		 */
		mfsdr(sdr_sdstp1, val);
		boot_selection = val & SDR0_SDSTP1_BOOT_SEL_MASK;
		ebc_boot_size = val & SDR0_SDSTP1_EBC_ROM_BS_MASK;

		switch(boot_selection) {
			case SDR0_SDSTP1_BOOT_SEL_EBC:
				switch(ebc_boot_size) {
					case SDR0_SDSTP1_EBC_ROM_BS_16BIT:
						index = 3;
						break;
					case SDR0_SDSTP1_EBC_ROM_BS_8BIT:
						index = 0;
						break;
				}
				break;

			case SDR0_SDSTP1_BOOT_SEL_PCI:
				index = 1;
				break;

		}
	}

#if defined(DEBUG) || defined(DEBUG_FLASH) 
	printf("\n");
	printf(" - FLASH Index: %d\n", index);
#endif

	/* Init: no FLASHes known */
	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
		flash_info[i].sector_count = -1;
		flash_info[i].size = 0;

#if defined(DEBUG) || defined(DEBUG_FLASH) 
		printf("   BANK: % d\n", i);
#endif

		/* check whether the address is 0 */
		if (flash_addr_table[index][i] == 0)
			continue;

		/* call flash_get_size() to initialize sector address */
		size_b[i] = flash_get_size((vu_long *) flash_addr_table[index][i],
				&flash_info[i]);

		flash_info[i].size = size_b[i];

		if (flash_info[i].flash_id == FLASH_UNKNOWN) {
			printf("## Unknown FLASH on Bank %d - Size = 0x%08lx = %ld MB\n",
					i, size_b[i], size_b[i] << 20);
			flash_info[i].sector_count = -1;
			flash_info[i].size = 0;
		}

		/* Monitor protection ON by default */
		(void)flash_protect(FLAG_PROTECT_SET, CONFIG_SYS_MONITOR_BASE,
				CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN - 1,
				&flash_info[i]);
#if defined(CONFIG_ENV_IS_IN_FLASH)
		(void)flash_protect(FLAG_PROTECT_SET, CONFIG_ENV_ADDR,
				CONFIG_ENV_ADDR + CONFIG_ENV_SECT_SIZE - 1,
				&flash_info[i]);
#if defined(CONFIG_ENV_ADDR_REDUND)
		(void)flash_protect(FLAG_PROTECT_SET, CONFIG_ENV_ADDR_REDUND,
				CONFIG_ENV_ADDR_REDUND + CONFIG_ENV_SECT_SIZE - 1,
				&flash_info[i]);
#endif
#endif

		total_b += flash_info[i].size;
	}

	return total_b;
}

void flash_print_info(flash_info_t * info)
{
	int i;
	int k;
	int size;
	int erased;
	volatile unsigned long *flash;

	if(flash_print_basic_info(info) == FLASH_UNKNOWN)
	{
		return;
	}

	printf("  Size: %ld KB in %d Sectors\n",
			info->size >> 10, info->sector_count);

	printf("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		/*
		 * Check if whole sector is erased
		 */
		if (i != (info->sector_count - 1))
			size = info->start[i + 1] - info->start[i];
		else
			size = info->start[0] + info->size - info->start[i];
		erased = 1;
		flash = (volatile unsigned long *)info->start[i];
		size = size >> 2;	/* divide by 4 for longword access */
		for (k = 0; k < size; k++) {
			if (*flash++ != 0xffffffff) {
				erased = 0;
				break;
			}
		}

		if ((i % 5) == 0)
			printf("\n   ");
		printf(" %08lX%s%s",
				info->start[i],
				erased ? " E" : "  ",
				info->protect[i] ? "RO " : "   ");
	}
	printf("\n");
	return;
}

int flash_erase(flash_info_t * info, int s_first, int s_last)
{
	if (((info->flash_id & FLASH_TYPEMASK) == FLASH_AM800B) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM800T)) {
		return flash_erase_1(info, s_first, s_last);
	} else {
		return flash_erase_2(info, s_first, s_last);
	}
}

ulong flash_size(ulong baseaddr)
{
	ulong size = 0;
	ulong val;
	int index;
	int i;

	mfsdr(sdr_pstrp0, val);
	index = (val & SDR0_PSTRP0_BOOTSTRAP_MASK) >> 28;

	for(i=0; i<CONFIG_SYS_MAX_FLASH_BANKS; i++)
	{
		if (baseaddr == ((flash_addr_table[index][i]) & 0xFFFFFFFE))
		{
			size = flash_get_size((vu_long *)(flash_addr_table[index][i]), &flash_info[i]);
			break;
		}
	}
	return size;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff(flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	ulong cp, wp, data;
	int i, l, rc;

	wp = (addr & ~3);	/* get lower word aligned address */

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - wp) != 0) {
		data = 0;
		for (i = 0, cp = wp; i < l; ++i, ++cp)
			data = (data << 8) | (*(uchar *) cp);

		for (; i < 4 && cnt > 0; ++i) {
			data = (data << 8) | *src++;
			--cnt;
			++cp;
		}

		for (; cnt == 0 && i < 4; ++i, ++cp)
			data = (data << 8) | (*(uchar *) cp);

		if ((rc = write_word(info, wp, data)) != 0)
			return (rc);

		wp += 4;
	}

	/*
	 * handle word aligned part
	 */
	while (cnt >= 4) {
		data = 0;
		for (i = 0; i < 4; ++i)
			data = (data << 8) | *src++;

		if ((rc = write_word(info, wp, data)) != 0)
			return (rc);

		wp += 4;
		cnt -= 4;
	}

	if (cnt == 0)
		return (0);

	/*
	 * handle unaligned tail bytes
	 */
	data = 0;
	for (i = 0, cp = wp; i < 4 && cnt > 0; ++i, ++cp) {
		data = (data << 8) | *src++;
		--cnt;
	}
	for (; i < 4; ++i, ++cp)
		data = (data << 8) | (*(uchar *) cp);

	return (write_word(info, wp, data));
}


	/******************
	  LOCAL FUNCTIONS
	******************/


static int flash_print_basic_info(flash_info_t * info)
{
	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return FLASH_UNKNOWN;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
		case FLASH_MAN_AMD:
			printf("SPANSION ");
			break;
		case FLASH_MAN_NUMONYX:
			printf("NUMONYX ");
			break;
		case FLASH_MAN_STM:
			printf("STM ");
			break;
		case FLASH_MAN_FUJ:
			printf("FUJITSU ");
			break;
		case FLASH_MAN_SST:
			printf("SST ");
			break;
		case FLASH_MAN_MX:
			printf("MIXC ");
			break;
		default:
			printf("Unknown Vendor ");
			break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
		case FLASH_AM800B:
			switch (info->flash_id & FLASH_VENDMASK)
			{
				case FLASH_MAN_NUMONYX:
					printf("M29W800DB (8 Mbit, bottom boot sect)\n");
					break;
				case FLASH_MAN_AMD:
					printf("S29AL008D (8 Mbit, bottom boot sect)\n");
					break;
				default:
					printf("Unknown Chip Type\n");
					break;
			}
			break;
		case FLASH_AM800T:
			switch (info->flash_id & FLASH_VENDMASK)
			{
				case FLASH_MAN_NUMONYX:
					printf("M29W800DT (8 Mbit, top boot sect)\n");
					break;
				case FLASH_MAN_AMD:
					printf("S29AL008D (8 Mbit, top boot sect)\n");
					break;
				default:
					printf("Unknown Chip Type\n");
					break;
			}
			break;
		case FLASH_AMDL640:
			switch (info->flash_id & FLASH_VENDMASK)
			{
				case FLASH_MAN_NUMONYX:
					switch (info->sector_count)
					{
						case 256:
							printf("28F256M29EW (256 Kbit)\n");
							break;
						case 512:
							printf("28F512M29EW (512 Kbit)\n");
							break;
						case 1024:
							printf("28F00AM29EW (1 Gbit)\n");
							break;
						default:
							printf("Unknown Chip Type\n");
							break;
					}
					break;
				case FLASH_MAN_AMD:
					switch (info->sector_count)
					{
						case 128:
							printf("S29GL128P (128 Kbit)\n");
							break;
						case 256:
							printf("S29GL256P (256 Kbit)\n");
							break;
						case 512:
							printf("S29GL512P (512 Kbit)\n");
							break;
						case 1024:
							printf("S29GL01GP (1 Gbit)\n");
							break;
						default:
							printf("Unknown Chip Type\n");
							break;
					}
					break;
				default:
					printf("Unknown Chip Type\n");
					break;
			}
			break;
		default:
			printf("Unknown Chip Type\n");
			break;
	}
	return 0;
}

/*
 * The following code cannot be run from FLASH!
 */
static ulong flash_get_size(vu_long * addr, flash_info_t * info)
{
	ulong size = 0;

	/* bit 0 used for big flash marking */
	if ((ulong)addr & 0x1)
		size = flash_get_size_2((vu_long *)((ulong)addr & 0xfffffffe), info);
	else
		size = flash_get_size_1(addr, info);

#if defined(DEBUG) || defined(DEBUG_FLASH) 
	printf("   ");
	flash_print_basic_info(info);
#endif

	return size;
}

static ulong flash_get_size_1(vu_long * addr, flash_info_t * info)
{
	short i;
	CONFIG_SYS_FLASH_1_WORD_SIZE value;
	ulong base = (ulong) addr;
	volatile CONFIG_SYS_FLASH_1_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_1_WORD_SIZE *) addr;

	DEBUGF("   ADDR: %08x\n", (unsigned)addr);

	/* Write auto select command: read Manufacturer ID */
	addr2[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00AA00AA;
	addr2[CONFIG_SYS_FLASH_1_ADDR1] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00550055;
	addr2[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00900090;
	udelay(1000);

	value = addr2[0];
	DEBUGF("   MANUFACT: %x\n", value);

	switch (value) {
		case (CONFIG_SYS_FLASH_1_WORD_SIZE) STM_MANUFACT:
			info->flash_id = FLASH_MAN_NUMONYX;
			break;
		case (CONFIG_SYS_FLASH_1_WORD_SIZE) AMD_MANUFACT:
			info->flash_id = FLASH_MAN_AMD;
			break;
		default:
			info->flash_id = FLASH_UNKNOWN;
			info->sector_count = 0;
			info->size = 0;
			return (0); /* no or unknown flash */
	}

	value = addr2[2]; /* device ID */
	DEBUGF("   DEVICEID: %x\n", value);

	switch (value) {
		case (CONFIG_SYS_FLASH_1_WORD_SIZE) NUMONYX_ID_W800T:
		case (CONFIG_SYS_FLASH_1_WORD_SIZE) AMD_ID_LV800T:
			info->flash_id += FLASH_AM800T;
			info->sector_count = 19;
			info->size = 0x00100000;
			break;
		case (CONFIG_SYS_FLASH_1_WORD_SIZE) AMD_ID_LV800B:
			info->flash_id += FLASH_AM800B;
			info->sector_count = 19;
			info->size = 0x00100000;
			break;
		default:
			info->flash_id = FLASH_UNKNOWN;
			return (0); /* => no or unknown flash */
	}

	/* set up sector start address table */
	if (info->flash_id & FLASH_BTYPE) {
		/* set sector offsets for bottom boot block type */
		info->start[0] = base + 0x00000000;
		info->start[1] = base + 0x00004000;
		info->start[2] = base + 0x00006000;
		info->start[3] = base + 0x00008000;
		for (i = 4; i < info->sector_count; i++) {
			info->start[i] =
				base + (i * 0x00010000) - 0x00030000;
		}
	} else {
		/* set sector offsets for top boot block type */
		i = info->sector_count - 1;
		info->start[i--] = base + info->size - 0x00004000;
		info->start[i--] = base + info->size - 0x00006000;
		info->start[i--] = base + info->size - 0x00008000;
		for (; i >= 0; i--) {
			info->start[i] = base + i * 0x00010000;
		}
	}

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		/* D0 = 1 if protected */
		addr2 = (volatile CONFIG_SYS_FLASH_1_WORD_SIZE *)(info->start[i]);

		info->protect[i] = addr2[4] & 1;
	}

	/* issue bank reset to return to read mode */
	addr2[0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00F000F0;

	return (info->size);
}

/*
 * The following code cannot be run from FLASH!
 */
static ulong flash_get_size_2(vu_long * addr, flash_info_t * info)
{
	short i;
	CONFIG_SYS_FLASH_2_WORD_SIZE value;
	ulong base = (ulong) addr;
	volatile CONFIG_SYS_FLASH_2_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_2_WORD_SIZE *) addr;

	DEBUGF("   ADDR: %08x\n", (unsigned)addr);

	/* issue bank reset to return to read mode */
	addr2[0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00F000F0;
	/* Write auto select command: read Manufacturer ID */
	addr2[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00AA00AA;
	addr2[CONFIG_SYS_FLASH_2_ADDR1] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00550055;
	addr2[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00900090;
	udelay(1000);

	value = addr2[0];
	DEBUGF("   MANUFACT: %x\n", value);

	switch (value) {
		case (CONFIG_SYS_FLASH_1_WORD_SIZE) INTEL_MANUFACT:
			info->flash_id = FLASH_MAN_NUMONYX;
			break;
		case (CONFIG_SYS_FLASH_2_WORD_SIZE) AMD_MANUFACT:
			info->flash_id = FLASH_MAN_AMD;
			break;
		default:
			info->flash_id = FLASH_UNKNOWN;
			info->sector_count = 0;
			info->size = 0;
			return (0);	/* no or unknown flash  */
	}

	value = addr2[1]; /* device ID */
	DEBUGF("   DEVICEID: %x %x %x\n", value, addr2[0xe], addr2[0xf]);

	switch (value) {
		case (CONFIG_SYS_FLASH_2_WORD_SIZE)AMD_ID_DL640:

			switch (addr2[0xe])
			{
				case 0x2223:
			
					info->flash_id += FLASH_AMDL640;
					info->sector_count = 512;
					info->size = 0x04000000;
					break;
				case 0x2228:

					info->flash_id += FLASH_AMDL640;
					info->sector_count = 1024;
					info->size = 0x08000000;
				break;
				default:
					info->flash_id = FLASH_UNKNOWN;
					return (0); /* => no or unknown flash */
			}
			break;
		default:
			info->flash_id = FLASH_UNKNOWN;
			return (0); /* => no or unknown flash */
	}

	/* set up sector start address table */

	for (i = 0; i < info->sector_count; i++)
		info->start[i] = base + (i * 0x00020000); /* S29GL512N and S29GL01GP has 128K sector size. */

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {

		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		/* D0 = 1 if protected */
		addr2 = (volatile CONFIG_SYS_FLASH_2_WORD_SIZE *)(info->start[i]);

		info->protect[i] = addr2[2] & 1;
	}

	/* issue bank reset to return to read mode */
	addr2[0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00F000F0;

	return (info->size);
}

static int flash_erase_1(flash_info_t * info, int s_first, int s_last)
{
	volatile CONFIG_SYS_FLASH_1_WORD_SIZE *addr = (CONFIG_SYS_FLASH_1_WORD_SIZE *) (info->start[0]);
	volatile CONFIG_SYS_FLASH_1_WORD_SIZE *addr2;
	int flag, prot, sect, l_sect;
	int i;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN)
			printf("- missing\n");
		else
			printf("- no sectors to erase\n");
		return 1;
	}

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("Can't erase unknown flash type - aborted\n");
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect])
			prot++;
	}

	if (prot)
		printf("- Warning: %d protected sectors will not be erased!", prot);

	printf("\n");

	l_sect = -1;

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect] == 0) {	/* not protected */
			addr2 = (CONFIG_SYS_FLASH_1_WORD_SIZE *) (info->start[sect]);

			if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) {
				addr[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_1_ADDR1] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_1_ADDR1] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00500050;	/* block erase */
				for (i = 0; i < 50; i++)
					udelay(1000);	/* wait 1 ms */
			} else {
				addr[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_1_ADDR1] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_1_ADDR1] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00300030;	/* sector erase */
			}
			l_sect = sect;
			/*
			 * Wait for each sector to complete, it's more
			 * reliable.  According to AMD Spec, you must
			 * issue all erase commands within a specified
			 * timeout.  This has been seen to fail, especially
			 * if printf()s are included (for debug)!!
			 */
			wait_for_DQ7_1(info, sect);
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* wait at least 80us - let's wait 1 ms */
	udelay(1000);

	/* reset to read mode */
	addr = (CONFIG_SYS_FLASH_1_WORD_SIZE *) info->start[0];
	addr[0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00F000F0;	/* reset bank */

	printf(" done\n");
	return 0;
}

static int flash_erase_2(flash_info_t * info, int s_first, int s_last)
{
	volatile CONFIG_SYS_FLASH_2_WORD_SIZE *addr = (CONFIG_SYS_FLASH_2_WORD_SIZE *) (info->start[0]);
	volatile CONFIG_SYS_FLASH_2_WORD_SIZE *addr2;
	int flag, prot, sect, l_sect;
	int i;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN)
			printf("- missing\n");
		else
			printf("- no sectors to erase\n");
		return 1;
	}

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("Can't erase unknown flash type - aborted\n");
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect])
			prot++;
	}

	if (prot)
		printf("- Warning: %d protected sectors will not be erased!",	prot);

	printf("\n");

	l_sect = -1;

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect] == 0) {	/* not protected */
			addr2 = (CONFIG_SYS_FLASH_2_WORD_SIZE *) (info->start[sect]);

			if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) {
				addr[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_2_ADDR1] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_2_ADDR1] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00500050;	/* block erase */
				for (i = 0; i < 50; i++)
					udelay(1000);	/* wait 1 ms */
			} else {
				addr[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_2_ADDR1] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_2_ADDR1] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00300030;	/* sector erase */
			}
			l_sect = sect;
			/*
			 * Wait for each sector to complete, it's more
			 * reliable.  According to AMD Spec, you must
			 * issue all erase commands within a specified
			 * timeout.  This has been seen to fail, especially
			 * if printf()s are included (for debug)!!
			 */
			wait_for_DQ7_2(info, sect);
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* wait at least 80us - let's wait 1 ms */
	udelay(1000);

	/* reset to read mode */
	addr = (CONFIG_SYS_FLASH_2_WORD_SIZE *) info->start[0];
	addr[0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00F000F0;	/* reset bank */

	printf(" done\n");
	return 0;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word(flash_info_t * info, ulong dest, ulong data)
{
	if (((info->flash_id & FLASH_TYPEMASK) == FLASH_AM800B) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM800T)) {
		return write_word_1(info, dest, data);
	} else {
		return write_word_2(info, dest, data);
	}
}

static int write_word_1(flash_info_t * info, ulong dest, ulong data)
{
	volatile CONFIG_SYS_FLASH_1_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_1_WORD_SIZE *) (info->start[0]);
	volatile CONFIG_SYS_FLASH_1_WORD_SIZE *dest2 = (CONFIG_SYS_FLASH_1_WORD_SIZE *) dest;
	volatile CONFIG_SYS_FLASH_1_WORD_SIZE *data2 = (CONFIG_SYS_FLASH_1_WORD_SIZE *) & data;
	ulong start;
	int i, flag;

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_long *)dest) & data) != data)
		return (2);

	for (i = 0; i < 4 / sizeof(CONFIG_SYS_FLASH_1_WORD_SIZE); i++) {
		/* Disable interrupts which might cause a timeout here */
		flag = disable_interrupts();

		addr2[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00AA00AA;
		addr2[CONFIG_SYS_FLASH_1_ADDR1] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00550055;
		addr2[CONFIG_SYS_FLASH_1_ADDR0] = (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00A000A0;

		dest2[i] = data2[i];

		/* re-enable interrupts if necessary */
		if (flag)
			enable_interrupts();

		/* data polling for D7 */
		start = get_timer(0);
		while ((dest2[i] & (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00800080) !=
				(data2[i] & (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00800080)) {

			if (get_timer(start) > CONFIG_SYS_FLASH_WRITE_TOUT)
				return (1);
		}
	}

	return (0);
}

static int write_word_2(flash_info_t * info, ulong dest, ulong data)
{
	volatile CONFIG_SYS_FLASH_2_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_2_WORD_SIZE *) (info->start[0]);
	volatile CONFIG_SYS_FLASH_2_WORD_SIZE *dest2 = (CONFIG_SYS_FLASH_2_WORD_SIZE *) dest;
	volatile CONFIG_SYS_FLASH_2_WORD_SIZE *data2 = (CONFIG_SYS_FLASH_2_WORD_SIZE *) & data;
	ulong start;
	int i;

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_long *)dest) & data) != data)
		return (2);

	for (i = 0; i < 4 / sizeof(CONFIG_SYS_FLASH_2_WORD_SIZE); i++) {
		int flag;

		/* Disable interrupts which might cause a timeout here */
		flag = disable_interrupts();

		addr2[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00AA00AA;
		addr2[CONFIG_SYS_FLASH_2_ADDR1] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00550055;
		addr2[CONFIG_SYS_FLASH_2_ADDR0] = (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00A000A0;

		dest2[i] = data2[i];

		/* re-enable interrupts if necessary */
		if (flag)
			enable_interrupts();

		/* data polling for D7 */
		start = get_timer(0);
		while ((dest2[i] & (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00800080) !=
				(data2[i] & (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00800080)) {

			if (get_timer(start) > CONFIG_SYS_FLASH_WRITE_TOUT)
				return (1);
		}
	}

	return (0);
}

static int wait_for_DQ7_1(flash_info_t * info, int sect)
{
	ulong start, now, last;
	volatile CONFIG_SYS_FLASH_1_WORD_SIZE *addr =
		(CONFIG_SYS_FLASH_1_WORD_SIZE *) (info->start[sect]);

	start = get_timer(0);
	last = start;
	while ((addr[0] & (CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00800080) !=
			(CONFIG_SYS_FLASH_1_WORD_SIZE) 0x00800080) {
		if ((now = get_timer(start)) > CONFIG_SYS_FLASH_ERASE_TOUT) {
			printf("Timeout\n");
			return -1;
		}
		/* show that we're waiting */
		if ((now - last) > 1000) {	/* every second */
			putc('.');
			last = now;
		}
	}
	return 0;
}

static int wait_for_DQ7_2(flash_info_t * info, int sect)
{
	ulong start, now, last;
	volatile CONFIG_SYS_FLASH_2_WORD_SIZE *addr =
		(CONFIG_SYS_FLASH_2_WORD_SIZE *) (info->start[sect]);

	start = get_timer(0);
	last = start;
	while ((addr[0] & (CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00800080) !=
			(CONFIG_SYS_FLASH_2_WORD_SIZE) 0x00800080) {
		if ((now = get_timer(start)) > CONFIG_SYS_FLASH_ERASE_TOUT) {
			printf("Timeout\n");
			return -1;
		}
		/* show that we're waiting */
		if ((now - last) > 1000) {	/* every second */
			putc('.');
			last = now;
		}
	}
	return 0;
}
