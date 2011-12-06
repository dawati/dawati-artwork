#! /bin/sh
autoreconf -v --install || exit 1
if test -z "$NOCONFIGURE"; then
    ./configure "$@" || exit 1
fi
