#!/usr/bin/env python3

#
# Fix ToUnicode CMap in PDF
# https://github.com/trueroad/pdf-fix-tuc
#
# build_bfrange2.py:
#   Build bfrange2 PDF.
#
# Copyright (C) 2021 Masamichi Hosoda.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

import re
import sys

def main ():
    re_begin = re.compile (r"(\d+)\s+beginbfrange\r?\n?")

    for line in sys.stdin.buffer:
        try:
            line = line.decode ("utf-8")
        except UnicodeDecodeError:
            sys.stdout.buffer.write (line)
            continue

        m = re_begin.match (line)
        if m:
            print ("{} beginbfrange".format (int (m.group (1)) + 5))
            print ("""\
<0001> <0003> <2E81>
<0004> <0006> <2E99>
<0007> <0009> <2EF2>
<000A> <000C> <2EFF>
<000D> <000F> <2FD4>\
""")
            continue

        print (line, end = "", flush = True)

if __name__ == "__main__":
    main ()
