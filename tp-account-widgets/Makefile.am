include $(top_srcdir)/tools/flymake.mk

AM_CPPFLAGS =                                           \
	$(ERROR_CFLAGS)					\
	-I$(top_srcdir)/libempathy			\
	-I$(top_srcdir)/libempathy-gtk			\
	-DDATADIR=\""$(datadir)"\"              	\
	-DPKGDATADIR=\""$(pkgdatadir)"\"		\
	-DG_LOG_DOMAIN=\"empathy\"			\
	-DGCR_API_SUBJECT_TO_CHANGE			\
	$(EMPATHY_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED)

BUILT_SOURCES = 					\
	empathy-account-widgets-resources.c		\
	empathy-account-widgets-resources.h		\
	$(NULL)

libtp_account_widgets_sources =            	\
	empathy-account-widget.c		\
	empathy-account-widget-irc.c		\
	empathy-account-widget-private.h	\
	empathy-account-widget-sip.c		\
	empathy-irc-network-chooser.c		\
	empathy-irc-network-chooser-dialog.c	\
	empathy-irc-network-dialog.c		\
	totem-subtitle-encoding.c		\
	$(NULL)

libtp_account_widgets_headers =			\
	empathy-account-widget.h		\
	empathy-account-widget-irc.h		\
	empathy-account-widget-sip.h		\
	empathy-irc-network-chooser-dialog.h	\
	empathy-irc-network-chooser.h		\
	empathy-irc-network-dialog.h		\
	totem-subtitle-encoding.h		\
	$(NULL)

pkglib_LTLIBRARIES = libtp-account-widgets.la

# libtp-account-widgets's API is not stable and will never be, so use -release to make the
# SONAME of the plugin library change with every Empathy release.
libtp_account_widgets_la_LDFLAGS = \
   -no-undefined \
   -release $(VERSION) \
   $(NULL)

libtp_account_widgets_la_SOURCES =			\
	$(libtp_account_widgets_sources)		\
	$(libtp_account_widgets_headers)		\
	$(NULL)

# do not distribute generated files
nodist_libtp_account_widgets_la_SOURCES = \
	$(BUILT_SOURCES)

check_c_sources = \
    $(libtp_account_widgets_sources) \
    $(libtp_account_widgets_headers) \
    $(NULL)
include $(top_srcdir)/tools/check-coding-style.mk
check-local: check-coding-style

account_widgets_ui_files =			\
	empathy-account-widget-generic.ui	\
	empathy-account-widget-jabber.ui	\
	empathy-account-widget-msn.ui		\
	empathy-account-widget-sip.ui		\
	empathy-account-widget-local-xmpp.ui	\
	empathy-account-widget-irc.ui		\
	empathy-account-widget-icq.ui		\
	empathy-account-widget-yahoo.ui		\
	empathy-account-widget-groupwise.ui	\
	empathy-account-widget-aim.ui		\
	$(NULL)

account_widgets_resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir=$(srcdir) $(srcdir)/empathy-account-widgets.gresource.xml)

empathy-account-widgets-resources.c: empathy-account-widgets.gresource.xml $(account_widgets_resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $<

empathy-account-widgets-resources.h: empathy-account-widgets.gresource.xml $(account_widgets_resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-header $<

EXTRA_DIST =					\
	empathy-account-widgets.gresource.xml	\
	$(account_widgets_ui_files)		\
	$(NULL)

CLEANFILES =				\
	$(BUILT_SOURCES)		\
	$(NULL)
