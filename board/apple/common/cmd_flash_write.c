/*
 * (C) Copyright 2001
 * Denis Peter, MPL AG Switzerland, d.peter@mpl.ch
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

#include <common.h>
#include <command.h>
#include "flash.h"

#define FLASH_COPY_BLOCK_DEFAULT 512

extern ulong flash_size(ulong baseaddr);

/* ------------------------------------------------------------------------- */
int do_fhfill(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char src[FLASH_COPY_BLOCK_DEFAULT];
	ulong addr;
	ulong end;
	ulong pattern;
	ulong i;
	int rc = ERR_OK;

	if (argc < 4) {
		cmd_usage(cmdtp);
		goto exit;
	}
	
	addr = simple_strtoul(argv[1], NULL, 16);
	end = simple_strtoul(argv[2], NULL, 16);
	pattern = simple_strtoul(argv[3], NULL, 16);

	if (((addr >= CONFIG_SYS_FLASH_BASE) && (addr <= (CONFIG_ENV_ADDR_REDUND -1)) &&
	     (end >= CONFIG_SYS_FLASH_BASE) && (end <= (CONFIG_ENV_ADDR_REDUND -1)))
	||  ((addr >= CONFIG_SYS_FLASH_2_BASE) && (addr <= (CONFIG_SYS_FLASH_2_BASE + flash_size(CONFIG_SYS_FLASH_2_BASE) - 1)) &&
	     (end >= CONFIG_SYS_FLASH_2_BASE) && (end <= (CONFIG_SYS_FLASH_2_BASE + flash_size(CONFIG_SYS_FLASH_2_BASE) - 1))))
	{
		int index;

		for (i=0; i<FLASH_COPY_BLOCK_DEFAULT; i+=4)
		{
			*(ulong *)(src + i) = pattern;
		}
		
		#define START_STRING "Start filling flash "

		printf("\n");
		printf(START_STRING);
		index = strlen(START_STRING);

		for(i=end-addr; i>=FLASH_COPY_BLOCK_DEFAULT; i-=FLASH_COPY_BLOCK_DEFAULT)
		{
			if ((rc = flash_write (src, addr, FLASH_COPY_BLOCK_DEFAULT)) != ERR_OK)
			{
				printf("Write failure somewhere in [%p, %p]\n", (void *)addr, (void *)(addr + FLASH_COPY_BLOCK_DEFAULT - 1));
				goto exit;
			}
			else
			{
				addr += FLASH_COPY_BLOCK_DEFAULT;
			}

			printf(".");

			if (++index == 40 )
			{
				index = 0;
				printf("\n");
			}
		}
		if (i>0)
		{
			if ((rc = flash_write (src, addr, i)) != ERR_OK)
			{
				printf("Write failure somewhere in [%p, %p]\n", (void *)addr, (void *)(addr + i - 1));
				goto exit;
			}
		}
		printf(" COMPLETED!\n");
	}
	else
	{
		if (((addr >= CONFIG_ENV_ADDR_REDUND) && (addr <= (CONFIG_SYS_FLASH_BASE + flash_size(CONFIG_SYS_FLASH_BASE) - 1)))
		 || ((end >= CONFIG_ENV_ADDR_REDUND) && (end <= (CONFIG_SYS_FLASH_BASE + flash_size(CONFIG_SYS_FLASH_BASE) - 1)))
		 || ((addr < CONFIG_ENV_ADDR_REDUND) && (end > (CONFIG_SYS_FLASH_BASE + flash_size(CONFIG_SYS_FLASH_BASE) - 1))))
		{
			printf("ERROR: the range [%p, %p] overlaps with the U-Boot code [%p, %p].\n",
				(void *)addr, (void *)end, (void *)CONFIG_SYS_FLASH_BASE, (void *)(flash_size(CONFIG_SYS_FLASH_BASE) - 1));
		}
		else
		{
			printf("ERROR: the range [%p, %p] doesn't belong to a flash device\n", (void *)addr, (void *)end);
		}
	}

exit:

	return 0;
}

U_BOOT_CMD(
	fhfill,	4,	1,	do_fhfill,
	"fill flash memory range with a 32-bit pattern",
	"<start address> <end address> <32-bit pattern> - fill flash memory range with a 32-bit pattern\n"
);
