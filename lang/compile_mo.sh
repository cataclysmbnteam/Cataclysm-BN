#!/bin/sh

set -x

if [ ! -d lang/po ]
then
    if [ -d ../lang/po ]
    then
        cd ..
    else
        echo "Error: Could not find lang/po subdirectory."
        exit 1
    fi
fi

# Check output dir here
# Backward compatibility
if [ -z $LOCALE_DIR ]
then
    LOCALE_DIR="lang/mo"
fi

# compile .mo file for each specified language
if [ $# -gt 0 ] && [ $1 != "all" ]
then
    for n in $@
    do
        f="lang/po/${n}.po"
        mkdir -p $LOCALE_DIR/${n}/LC_MESSAGES
        msgfmt -f -o $LOCALE_DIR/${n}/LC_MESSAGES/cataclysm-bn.mo ${f}
    done
else
    # if nothing specified, compile .mo file for every .po file in lang/po
    lang/update_pot.sh
    for f in lang/po/*.po
    do
        n=`basename $f .po`
        mkdir -p $LOCALE_DIR/${n}/LC_MESSAGES
        msgfmt -f -o $LOCALE_DIR/${n}/LC_MESSAGES/cataclysm-bn.mo ${f}
    done
fi
