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

#ifndef _IPS_H
#define _IPS_H

/* library is re-entrant */

int ips_apply(const char *romfile, const char *ipsfile);
int ips_create(const char *romfile, const char *modified_romfile, const char *ipsfile);
int ips_check(const char *ipsfile);

/* Following are internal library functions that might be useful.
   Functions above are just filename-based frontends to them. */

/* when ips_apply returns, rom will point to the patched rom */
int ips_apply_mem(void *rom, int romsize, const void *ips, int ipssize);
/* make sure memory space ips has enough space. use ips_estimate() for this. */
int ips_create_mem(const void *rom, int romsize, const void *modified_rom, void *ips);

/* estimates the size of ips patch that would be generated.
   useful for allocating ips space before calling ips_create() */
int ips_estimate(const void *rom, int rom_size, const void *modified_rom);

const char *ips_version();

#endif /* _IPS_H */
