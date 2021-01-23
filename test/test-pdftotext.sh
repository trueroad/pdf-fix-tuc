#!/bin/sh

TESTFILE="$1"
BASENAME=`basename ${TESTFILE} | sed -e "s/-testtext\.pdftotext$//"`
EXPECTED=${BASENAME}-expected.txt

if [ ! -e "${EXPECTED}" ]; then
    EXPECTED=${srcdir}/${BASENAME}-expected.txt
fi   

${DIFF} -ubwBE ${EXPECTED} ${TESTFILE}
