NULL =

sounddir = $(datadir)/sounds/dawati/stereo

single_beep =				\
	audio-volume-change.wav		\
	battery-caution.wav		\
	battery-low.wav			\
	bell-terminal.wav		\
	bell-window-system.wav		\
	camera-shutter.wav		\
	count-down.wav			\
	device-added.wav		\
	device-removed.wav		\
	dialog-error.wav		\
	dialog-question.wav		\
	dialog-warning.wav		\
	drag-fail.wav			\
	item-deleted.wav		\
	message-new-email.wav		\
	message-new-instant.wav		\
	phone-hangup.wav		\
	phone-outgoing-busy.wav		\
	power-unplug.wav		\
	screen-capture.wav		\
	suspend-resume.wav		\
	suspend-start.wav		\
	trash-empty.wav			\
	window-attention-inactive.wav	\
	$(NULL)

double_beep =				\
	alarm-clock-elapsed.wav		\
	phone-failure.wav		\
	phone-incoming-call.wav		\
	power-unplug-battery-low.wav	\
	suspend-error.wav		\
	$(NULL)

desktop_login = audio-test-signal.ogg
	
sound_DATA =                            \
	desktop-login.ogg		\
	double-very-low.wav		\
	single-very-low.wav		\
	$(NULL)

EXTRA_DIST =            \
	$(sound_DATA)	\
	$(NULL)

install-data-hook:
	cd $(DESTDIR)$(sounddir) &&			\
	for i in $(single_beep); do			\
		$(LN_S) single-very-low.wav $$i;	\
	done
	cd $(DESTDIR)$(sounddir) &&			\
	for i in $(double_beep); do			\
		$(LN_S) double-very-low.wav $$i;	\
	done
	cd $(DESTDIR)$(sounddir) &&			\
	for i in $(desktop_login); do			\
		$(LN_S) desktop-login.ogg $$i;		\
	done

DISTCLEANFILES = \
	$(NULL)

MAINTAINERCLEANFILES =		\
	*~			\
	Makefile.in		\
	$(NULL)
