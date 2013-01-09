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

plugin_LTLIBRARIES = libempathy-accounts-plugin.la

libempathy_accounts_plugin_la_LDFLAGS = \
	-module -avoid-version

libempathy_accounts_plugin_la_SOURCES = \
	empathy-accounts-plugin.c \
	empathy-accounts-plugin.h \
	empathy-accounts-plugin-widget.c \
	empathy-accounts-plugin-widget.h

libempathy_accounts_plugin_la_LIBADD = \
	$(UOA_LIBS) \
	$(top_builddir)/libempathy/libempathy.la \
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la
