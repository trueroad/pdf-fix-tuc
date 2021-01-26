#!/usr/bin/env python3

#
# Fix ToUnicode CMap in PDF
# https://github.com/trueroad/pdf-fix-tuc
#
# build_bfrange.py:
#   Build bfrange PDF.
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
    b_bfchar = False
    re_begin = re.compile (r"(\d+)\s+beginbfchar\r?\n?")
    re_bfchar = re.compile (r"<([\da-fA-F]+)>\s*<([\da-fA-F]+)>\r?\n?")
    re_end = re.compile (r"endbfchar\r?\n?")

    for line in sys.stdin.buffer:
        try:
            line = line.decode ("utf-8")
        except UnicodeDecodeError:
            sys.stdout.buffer.write (line)
            continue

        if b_bfchar:
            if re_end.match (line):
                line = "endbfrange\n"
                b_bfchar = False
            else:
                m = re_bfchar.match (line)
                if m:
                    line = "<{}> <{}> <{}>\n".\
                        format (m.group (1), m.group (1), m.group (2))
        else:
            m = re_begin.match (line)
            if m:
                line = "{} beginbfrange\n".format (m.group (1))
                b_bfchar = True

        print (line, end = "", flush = True)

if __name__ == "__main__":
    main ()
