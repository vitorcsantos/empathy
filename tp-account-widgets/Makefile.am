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
	tpaw-account-widgets-resources.c		\
	tpaw-account-widgets-resources.h		\
	$(NULL)

libtp_account_widgets_sources =            	\
	tpaw-account-settings.c			\
	tpaw-account-widget.c			\
	tpaw-account-widget-irc.c		\
	tpaw-account-widget-private.h		\
	tpaw-account-widget-sip.c		\
	tpaw-connection-managers.c		\
	tpaw-keyring.c				\
	tpaw-irc-network-chooser.c		\
	tpaw-irc-network-chooser-dialog.c	\
	tpaw-irc-network-dialog.c		\
	tpaw-irc-network-manager.c		\
	tpaw-irc-network.c			\
	tpaw-irc-server.c			\
	totem-subtitle-encoding.c		\
	$(NULL)

libtp_account_widgets_headers =			\
	tpaw-account-settings.h			\
	tpaw-account-widget.h			\
	tpaw-account-widget-irc.h		\
	tpaw-account-widget-sip.h		\
	tpaw-connection-managers.h		\
	tpaw-keyring.h				\
	tpaw-irc-network-chooser-dialog.h	\
	tpaw-irc-network-chooser.h		\
	tpaw-irc-network-dialog.h		\
	tpaw-irc-network-manager.h		\
	tpaw-irc-network.h			\
	tpaw-irc-server.h			\
	tpaw-utils.h				\
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

# these are sources that depend on Ubuntu Online Accounts
uoa_sources = \
	tpaw-uoa-utils.c \
	tpaw-uoa-utils.h \
	$(NULL)

if HAVE_UOA
libtp_account_widgets_la_SOURCES += $(uoa_sources)
EXTRA_DIST =
else
EXTRA_DIST = $(uoa_sources)
endif

# do not distribute generated files
nodist_libtp_account_widgets_la_SOURCES = \
	$(BUILT_SOURCES)

check_c_sources = \
    $(libtp_account_widgets_sources) \
    $(libtp_account_widgets_headers) \
    $(uoa_sources) \
    $(NULL)
include $(top_srcdir)/tools/check-coding-style.mk
check-local: check-coding-style

account_widgets_dtd_files =		\
	tpaw-irc-networks.dtd		\
	$(NULL)

ircnetworksdir = $(datadir)/empathy
ircnetworks_DATA =		\
	irc-networks.xml

account_widgets_ui_files =			\
	tpaw-account-widget-generic.ui		\
	tpaw-account-widget-jabber.ui		\
	tpaw-account-widget-msn.ui		\
	tpaw-account-widget-sip.ui		\
	tpaw-account-widget-local-xmpp.ui	\
	tpaw-account-widget-irc.ui		\
	tpaw-account-widget-icq.ui		\
	tpaw-account-widget-yahoo.ui		\
	tpaw-account-widget-groupwise.ui	\
	tpaw-account-widget-aim.ui		\
	$(NULL)

account_widgets_resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir=$(srcdir) $(srcdir)/tpaw-account-widgets.gresource.xml)

tpaw-account-widgets-resources.c: tpaw-account-widgets.gresource.xml $(account_widgets_resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $<

tpaw-account-widgets-resources.h: tpaw-account-widgets.gresource.xml $(account_widgets_resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-header $<

EXTRA_DIST +=					\
	tpaw-account-widgets.gresource.xml	\
	$(account_widgets_dtd_files)		\
	$(account_widgets_ui_files)		\
	$(ircnetworks_DATA)			\
	$(NULL)

CLEANFILES =				\
	$(BUILT_SOURCES)		\
	$(NULL)
