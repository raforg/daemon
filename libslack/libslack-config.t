#!/bin/bash
#
# libslack - http://libslack.org/
#
# Copyright (C) 1999-2001 raf <raf@raf.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# or visit http://www.gnu.org/copyleft/gpl.html
#
# 20011109 raf <raf@raf.org>

var() { eval $1='$2'; export $1; }

var url     "@@URL@@"
var name    "@@NAME@@"
var version "@@VERSION@@"
var prefix  "@@PREFIX@@"
var cflags  "@@CFLAGS@@"
var libs    "@@LIBS@@"

if test "$prefix" != "/usr" -a "$prefix" != "/usr/local"
then
	cflags="-I$prefix/include $cflags"
	libs="-L$prefix/lib $libs"
fi

usage()
{
	cat <<EOF
usage: libslack-config [options]
options:
    -h, --help      - Print this help and exit
    -v, --version   - Print the version of the currently installed libslack
    -L, --latest    - Print the latest version of libslack (uses wget and perl)
    -D, --download  - Download the latest version of libslack (uses wget)
    -p, --prefix    - Print the prefix directory of the libslack installation
    -c, --cflags    - Print CFLAGS needed to compile clients of libslack
    -l, --libs      - Print LDFLAGS needed to link against libslack
    -u, --uninstall - Uninstall libslack

Command line example:
    gcc -o app app.c \`libslack-config --cflags --libs\`

Makefile example:
    APP_CFLAGS  += \$(shell libslack-config --cflags)
    APP_LDFLAGS += \$(shell libslack-config --libs)

EOF
	exit $1
}

latest()
{
	wget -q -O- "${url}download/" | \
	perl -e '

		$ENV{suffix} =~ s/\./\\./g;

		while (<>)
		{
			$version{$1} = 1 if /HREF="$ENV{name}-([.0-9a-z]+)\.tar\.gz"/;
		}

		sub version_sort
		{
			my @anum = split /\./, $a;
			my @bnum = split /\./, $b;

			while ($#anum != -1 && $#bnum != -1)
			{
				return $x if $x = $bnum[0] - $anum[0];
				shift @anum;
				shift @bnum;
			}

			return -1 if $#anum != -1;
			return  1 if $#bnum != -1;
			return 0;
		}

		@version = sort { version_sort } keys %version;
		die "No versions found at $ENV{url}download/\n" if $#version == -1;
		print "$ENV{url}download/$ENV{name}-$version[0].tar.gz\n";
		exit 0;
	'
}

die() { echo "$@" >&2; exit 1; }

download()
{
	latest="`latest 2>&1`"
	test "$latest" = "No versions found at ${url}/download" && die "$latest"
	file="${latest##*/}"
	test -f "$file" && die "The file $file already exists"
	wget "$latest"
}

uninstall()
{
@@UNINSTALL@@
}

test $# -eq 0 && usage 1 1>&2

while test $# -gt 0
do
	case $1 in
		-h|--help)
			usage 0
			;;

		-v|--version)
			echo "$version"
			;;

		-L|--latest)
			latest
			;;

		-D|--download)
			download
			;;

		-p|--prefix)
			echo "$prefix"
			;;

		-c|--cflags)
			echo "$cflags"
			;;

		-l|--libs)
			echo "$libs"
			;;

		-u|--uninstall)
			uninstall
			;;

		*)
			usage 1 >&2
			;;
	esac
	shift
done

exit 0

# vi:set ts=4 sw=4
