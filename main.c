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


#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>

#include "ips.h"
#include "config.h"

static void display_usage()
{
	fprintf(stdout, "%s\n", PACKAGE);
	fprintf(stdout, "A utility for appyling & creating IPS patches.\n\n");

	fprintf(stdout, "usage: %s [-hLV] -a|c -r <romfile> -m <modified_romfile> -i <ipsfile>\n\n", PACKAGE);

	fprintf(stderr, "-a             apply IPS patch\n");
	fprintf(stderr, "-c             create IPS patch\n");
	fprintf(stderr, "-h             create IPS patch\n");
	fprintf(stderr, "-i <ipsfile>   set IPS patch filename\n");
	fprintf(stderr, "-L             display software license and quit\n");
	fprintf(stderr, "-m <mromfile>  set modified rom filename (only with -c)\n");
	fprintf(stderr, "-r <romfile>   set original rom filename\n");
	fprintf(stderr, "-v             verify integrity of IPS file\n");
	fprintf(stderr, "-V             display version info and quit\n");
}


static void display_version()
{
	fprintf(stdout, "%s %s (%s)\n", PACKAGE, VERSION, __DATE__);
}


static void display_license()
{
	fprintf(stdout, "You may redistribute copies of this program\n");
	fprintf(stdout, "under the terms of the GNU General Public License.\n");
	fprintf(stdout, "For more information about these matters, see the file named COPYING.\n");
	fprintf(stdout, "Report bugs to <bug@freeconsole.org>.\n");
}

static void error(const char *s)
{
	fprintf(stderr, "%s", s);
	exit(1);
}

int main(int argc, char* argv[])
{
	enum {
		ACT_NONE,
		ACT_APPLY,
		ACT_CREATE
	} action = ACT_NONE;
	int c, err;
	char *ipsfile=0, *romfile=0, *modified_romfile=0;
	int verify=0;

	while((c=getopt(argc, argv, "acvr:m:i:hLV")) != EOF)
	{
		switch(c)
		{
			case 'a':
				action = ACT_APPLY;
			break;

			case 'c':
				action = ACT_CREATE;
			break;

			case 'v':
				verify = 1;

			case 'r':
				romfile = optarg;
			break;

			case 'm':
				modified_romfile = optarg;
			break;

			case 'i':
				ipsfile = optarg;
			break;

			case 'h':
				display_usage();
				exit(0);
			case 'L':
				display_license();
				exit(0);
			break;

			case 'V':
				display_version();
				exit(0);
		}
	}

	if(action == ACT_APPLY)
	{
		if(!romfile) error("no rom file specified\n");
		if(!ipsfile) error("no IPS file specified\n");

		if((err = ips_apply(romfile, ipsfile)))
		{
			fprintf(stderr, "error: %s\n", strerror(errno));
			exit(err);
		}
	}
	else if(action == ACT_CREATE)
	{
		if(!romfile) error("no rom file specified\n");
		if(!modified_romfile) error("no modified rom file specified\n");
		if(!ipsfile) error("no IPS file specified\n");

		if((err = ips_create(romfile, modified_romfile, ipsfile)))
		{
			if(err==-1) fprintf(stderr, "rom lengths do not match.\n");
			else fprintf(stderr, "error: %s\n", strerror(errno));
			exit(err);
		}
	}

	return 0;
}
