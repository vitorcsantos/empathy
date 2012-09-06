# Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
# Do NOT edit manually
plugindir = $(ACCOUNTS_PROVIDER_PLUGIN_DIR)

INCLUDES =					\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-DLOCALEDIR=\""$(datadir)/locale"\"	\
	$(UOA_CFLAGS)				\
	$(WARN_CFLAGS)				\
	$(ERROR_CFLAGS)				\
	$(DISABLE_DEPRECATED)			\
	$(EMPATHY_CFLAGS)

pkglib_LTLIBRARIES = libempathy-uoa-account-plugin.la

# this API is not stable and will never be, so use -release to make the
# SONAME of the plugin library change with every Empathy release.
libempathy_uoa_account_plugin_la_LDFLAGS = \
   -no-undefined \
   -release $(VERSION)

libempathy_uoa_account_plugin_la_SOURCES = \
	empathy-accounts-plugin.c \
	empathy-accounts-plugin.h \
	empathy-accounts-plugin-widget.c \
	empathy-accounts-plugin-widget.h

libempathy_uoa_account_plugin_la_LIBADD = \
	$(UOA_LIBS)

plugin_LTLIBRARIES = \
	libaim.la\
	libgadugadu.la\
	libgroupwise.la\
	libicq.la\
	libirc.la\
	libjabber.la\
	libmxit.la\
	libmyspace.la\
	libsip.la\
	liblocal-xmpp.la\
	libsametime.la\
	libyahoojp.la\
	libyahoo.la\
	libzephyr.la \
	$(NULL)

libaim_la_SOURCES = \
	empathy-accounts-plugin-aim.c \
	empathy-accounts-plugin-aim.h
libaim_la_LDFLAGS = -module -avoid-version
libaim_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libgadugadu_la_SOURCES = \
	empathy-accounts-plugin-gadugadu.c \
	empathy-accounts-plugin-gadugadu.h
libgadugadu_la_LDFLAGS = -module -avoid-version
libgadugadu_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libgroupwise_la_SOURCES = \
	empathy-accounts-plugin-groupwise.c \
	empathy-accounts-plugin-groupwise.h
libgroupwise_la_LDFLAGS = -module -avoid-version
libgroupwise_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libicq_la_SOURCES = \
	empathy-accounts-plugin-icq.c \
	empathy-accounts-plugin-icq.h
libicq_la_LDFLAGS = -module -avoid-version
libicq_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libirc_la_SOURCES = \
	empathy-accounts-plugin-irc.c \
	empathy-accounts-plugin-irc.h
libirc_la_LDFLAGS = -module -avoid-version
libirc_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libjabber_la_SOURCES = \
	empathy-accounts-plugin-jabber.c \
	empathy-accounts-plugin-jabber.h
libjabber_la_LDFLAGS = -module -avoid-version
libjabber_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libmxit_la_SOURCES = \
	empathy-accounts-plugin-mxit.c \
	empathy-accounts-plugin-mxit.h
libmxit_la_LDFLAGS = -module -avoid-version
libmxit_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libmyspace_la_SOURCES = \
	empathy-accounts-plugin-myspace.c \
	empathy-accounts-plugin-myspace.h
libmyspace_la_LDFLAGS = -module -avoid-version
libmyspace_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libsip_la_SOURCES = \
	empathy-accounts-plugin-sip.c \
	empathy-accounts-plugin-sip.h
libsip_la_LDFLAGS = -module -avoid-version
libsip_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


liblocal_xmpp_la_SOURCES = \
	empathy-accounts-plugin-local-xmpp.c \
	empathy-accounts-plugin-local-xmpp.h
liblocal_xmpp_la_LDFLAGS = -module -avoid-version
liblocal_xmpp_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libsametime_la_SOURCES = \
	empathy-accounts-plugin-sametime.c \
	empathy-accounts-plugin-sametime.h
libsametime_la_LDFLAGS = -module -avoid-version
libsametime_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libyahoojp_la_SOURCES = \
	empathy-accounts-plugin-yahoojp.c \
	empathy-accounts-plugin-yahoojp.h
libyahoojp_la_LDFLAGS = -module -avoid-version
libyahoojp_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libyahoo_la_SOURCES = \
	empathy-accounts-plugin-yahoo.c \
	empathy-accounts-plugin-yahoo.h
libyahoo_la_LDFLAGS = -module -avoid-version
libyahoo_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la


libzephyr_la_SOURCES = \
	empathy-accounts-plugin-zephyr.c \
	empathy-accounts-plugin-zephyr.h
libzephyr_la_LDFLAGS = -module -avoid-version
libzephyr_la_LIBADD = \
	libempathy-uoa-account-plugin.la \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la
