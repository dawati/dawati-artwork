backgrounddescdir = $(datadir)/gnome-background-properties
backgrounddesc_DATA = \
	dawati-backgrounds.xml \
	$(NULL)

backgrounddir = $(datadir)/backgrounds/dawati
dist_background_DATA = \
	linus.jpg \
	myzone-default-background.png \
	paper.png \
	system-background.png \
	wood.png \
	$(NULL)

EXTRA_DIST = \
	dawati-backgrounds.xml.in \
	$(NULL)

CLEANFILES = $(backgrounddesc_DATA)

%.xml: %.xml.in
	$(AM_V_GEN)sed -e "s|\@datadir\@|$(datadir)|" $< > $@
