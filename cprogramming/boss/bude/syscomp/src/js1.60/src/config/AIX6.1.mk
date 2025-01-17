# -*- Mode: makefile -*-
#
# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is Mozilla Communicator client code, released
# March 31, 1998.
#
# The Initial Developer of the Original Code is
# Netscape Communications Corporation.
# Portions created by the Initial Developer are Copyright (C) 1998
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

#
# Config stuff for AIX
#

#CC = xlC_r
CC = xlC -brtl -q64
CCC = xlC_r
CFLAGS += -qarch=com -qnoansialias -qinline+$(INLINES) -DXP_UNIX -DAIX -DAIXV3 -DSYSV -DAIX4_3 -DHAVE_LOCALTIME_R

RANLIB = ranlib

#.c.o:
#	$(CC) -c -MD $*.d $(CFLAGS) $<
ARCH := aix
CPU_ARCH = rs6000
GFX_ARCH = x
INLINES = js_compare_and_swap:js_fast_lock1:js_fast_unlock1:js_lock_get_slot:js_lock_set_slot:js_lock_scope1

#-lpthreads -lc_r

#MKSHLIB_BIN = /usr/ibmcxx/bin/makeC++SharedLib_r
MKSHLIB_BIN = /usr/vacpp/bin/makeC++SharedLib_r
#MKSHLIB = $(MKSHLIB_BIN) -p 0 -G -berok -bM:UR
MKSHLIB = $(MKSHLIB_BIN) -X64 -p 0 -G -berok -bM:UR

ifdef JS_THREADSAFE
XLDFLAGS += -ldl
endif

