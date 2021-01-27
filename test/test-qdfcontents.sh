#!/bin/sh

TESTFILE="$1"
BASENAME=`basename ${TESTFILE} | sed -e "s/-testqdf\.qdfcontents$//"`
EXPECTED=${BASENAME}-qdfcontents.txt

if [ ! -e "${EXPECTED}" ]; then
    EXPECTED=${srcdir}/${BASENAME}-qdfcontents.txt
fi

NOTFOUND=no

function check () {
    if "${GREP}" -q "$1" "${TESTFILE}"; then
        echo \"$1\" is found
    else
        echo \"$1\" is not found
        NOTFOUND=yes
    fi
}

cat "${EXPECTED}" | while read line; do
    check "${line}"
done

if [ "x${NOTFOUND}" = "xyes" ]; then
    exit 1
fi

exit 0
