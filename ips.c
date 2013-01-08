/*
    ips - A utility for appyling & creating IPS patches.
    Copyright (c) 2006, Utkan Güngördü <utkan@freeconsole.org>


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ips.h"
#include "config.h"

#define HEADER "PATCH"
#define FOOTER "EOF"
#define SFREE(p) if(p) free(p)

/* IPS address */
static unsigned int ra(const void *p)
{
	unsigned char *s = (unsigned char *)p;
	return s[2] | (s[1]<<8) | (s[0]<<16);
}

static void wa(void *p, unsigned int adr)
{
	unsigned char *s = (unsigned char *)p;
	*s++ = (adr>>16) & 0xff;
	*s++ = (adr>>8 ) & 0xff;
	*s++ = (adr    ) & 0xff;
}

/* IPS block size */
static unsigned int rs(const void *p)
{
	unsigned char *s = (unsigned char *)p;
	return s[1] | (s[0]<<8);
}

static void ws(void *p, unsigned int size)
{
	unsigned char *s = (unsigned char *)p;
	*s++ = (size>>8) & 0xff;
	*s++ = (size   ) & 0xff;
}


int ips_apply_mem(void *rom, int rom_size, const void *ips, int ips_size)
{
	char *p=(char*)ips, *end, *dst=(char*)dst;
	unsigned int adr,size;
	end = p + (ips_size - strlen(HEADER) + strlen(FOOTER)) ;
	p += strlen(HEADER);

	while(p<end)
	{
		adr = ra(p);
		p+=2;
		size = rs(p);
		p+=3;
		memcpy(dst+adr,p,size);
	}

	return 0;
}

int ips_create_mem(const void *rom, int rom_size, const void *modified_rom, void *_ips)
{
	char *ips = (char*)_ips;

	strcpy(ips, HEADER);
	ips += strlen(HEADER);



	strcpy(ips,FOOTER);

	return 0;
}

int ips_estimate(const void *rom, int romsize, const void *modified_rom)
{
	return ips_create_mem(rom,romsize,modified_rom,0);
}

/* load file into memory are pointed by rom */
static void *load_file(const char *file, int *pfsize)
{
	char *mem;
	FILE *fp;
	long fsize;

	fp = fopen(file, "r");
	if(fp == NULL) return NULL;
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	rewind(fp);
	mem = (char*)malloc(fsize);
	if(!mem) return NULL;
	fread(mem, 1, fsize, fp);
	fclose(fp);

	if(pfsize) *pfsize = fsize;
	return mem;
}

int ips_apply(const char *romfile, const char *ipsfile)
{
	char *rom=0, *ips=0;
	int romsize, ipssize;
	FILE *fp_rom;
	int ret=0;

	rom = load_file(romfile, &romsize);
	if(!rom) return 1;

	ips = load_file(ipsfile, &ipssize);
	if(!ips) return 1;

 	if(ips_apply_mem(rom,romsize,ips,ipssize)) { ret=1; goto ips_apply_end; }

	fp_rom = fopen(romfile, "w");
	if(!fp_rom) { ret=1; goto ips_apply_end; }
	fwrite(rom, 1, romsize, fp_rom);
	fclose(fp_rom);

ips_apply_end:
	SFREE(rom);
	SFREE(ips);

	return ret;
}

int ips_create(const char *romfile, const char *modified_romfile, const char *ipsfile)
{
	char *rom=0, *mrom=0, *ips=0;
	int romsize, mromsize, ipssize;
	int ret = 0;
	FILE *fp_ips;

	rom = load_file(romfile, &romsize);
	if(!rom) { ret=1; goto ips_create_end; }

	ips = load_file(modified_romfile, &mromsize);
	if(!ips) { ret=1; goto ips_create_end; }

	if(romsize != mromsize) { ret=-1; goto ips_create_end; }

	ipssize = ips_estimate(rom, romsize, mrom);
	ips = (char*)malloc(ipssize);
	if(!ips) { ret=1; goto ips_create_end; }

	if(ips_create_mem(rom, romsize, mrom, ips)) { ret=1; goto ips_create_end; }

	fp_ips = fopen(ipsfile, "w");
	if(!fp_ips) { ret=1; goto ips_create_end; }
	fwrite(ips, 1, ipssize, fp_ips);
	fclose(fp_ips);

ips_create_end:
	SFREE(rom);
	SFREE(mrom);
	SFREE(ips);

	return ret;
}

const char *ips_version()
{
	return VERSION;
}
