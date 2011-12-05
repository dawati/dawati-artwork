INCLUDES = $(GTK_CFLAGS) $(DEVELOPMENT_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = libdawati.la

libdawati_la_SOURCES = \
	dawati-style.c \
	dawati-style.h \
	dawati-rc-style.c \
	dawati-rc-style.h \
	dawati-main.c \
	dawati-utils.c \
	dawati-utils.h \
	$(NULL)

libdawati_la_LDFLAGS = -module -avoid-version -no-undefined -Werror
libdawati_la_LIBADD = $(GTK_LIBS)
