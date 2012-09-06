#!/usr/bin/env python

# (name, CM, protocol, icon)
ALL = [
        ('AIM', 'haze', 'aim', 'aim'),
        ('GaduGadu', 'haze', 'gadugadu', 'gadugadu'),
        ('Groupwise', 'haze', 'groupwise', 'groupwise'),
        ('ICQ', 'haze', 'icq', 'icq'),
        ('IRC', 'idle', 'irc', 'irc'),
        ('Jabber', 'gabble', 'jabber', 'jabber'),
        ('Mxit', 'haze', 'mxit', 'mxit'),
        ('Myspace', 'haze', 'myspace', 'myspace'),
        ('SIP', 'sofiasip', 'sip', 'sip'),
        ('Salut', 'salut', 'local-xmpp', 'people-nearby'),
        ('Sametime', 'haze', 'sametime', 'sametime'),
        ('Yahoo Japan', 'haze', 'yahoojp', 'yahoo'),
        ('Yahoo!', 'haze', 'yahoo', 'yahoo'),
        ('Zephyr', 'haze', 'zephyr', 'zephyr'),
      ]

class Plugin:
    def __init__(self, name, cm, protocol, icon):
        self.name = name
        self.cm = cm
        self.protocol = protocol
        self.icon = icon

##### account-plugins/ #####

def magic_replace(text, protocol):
    p = protocol.replace('-', '_')

    l = protocol.split('-')
    l = map(str.title, l)
    camel = ''.join(l)

    text = text.replace('$lower', p)
    text = text.replace('$UPPER', p.upper())
    text = text.replace('$Camel', camel)

    return text

def generate_plugin_header(p):
    # header
    f = open('account-plugins/empathy-accounts-plugin-%s.h' % p.protocol, 'w')

    tmp = '''/* # Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
 * Do NOT edit manually */

/*
 * empathy-accounts-plugin-%s.h
 *
 * Copyright (C) 2012 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef __EMPATHY_ACCOUNTS_PLUGIN_$UPPER_H__
#define __EMPATHY_ACCOUNTS_PLUGIN_$UPPER_H__

#include "empathy-accounts-plugin.h"

G_BEGIN_DECLS

typedef struct _EmpathyAccountsPlugin$Camel EmpathyAccountsPlugin$Camel;
typedef struct _EmpathyAccountsPlugin$CamelClass EmpathyAccountsPlugin$CamelClass;

struct _EmpathyAccountsPlugin$CamelClass
{
  /*<private>*/
  EmpathyAccountsPluginClass parent_class;
};

struct _EmpathyAccountsPlugin$Camel
{
  /*<private>*/
  EmpathyAccountsPlugin parent;
};

GType empathy_accounts_plugin_$lower_get_type (void);

/* TYPE MACROS */
#define EMPATHY_TYPE_ACCOUNTS_PLUGIN_$UPPER \\
  (empathy_accounts_plugin_$lower_get_type ())
#define EMPATHY_ACCOUNTS_PLUGIN_$UPPER(obj) \\
  (G_TYPE_CHECK_INSTANCE_CAST((obj), \\
    EMPATHY_TYPE_ACCOUNTS_PLUGIN_$UPPER, \\
    EmpathyAccountsPlugin$Camel))
#define EMPATHY_ACCOUNTS_PLUGIN_$UPPER_CLASS(klass) \\
  (G_TYPE_CHECK_CLASS_CAST((klass), \\
    EMPATHY_TYPE_ACCOUNTS_PLUGIN_$UPPER, \\
    EmpathyAccountsPlugin$CamelClass))
#define EMPATHY_IS_ACCOUNTS_PLUGIN_$UPPER(obj) \\
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), \\
    EMPATHY_TYPE_ACCOUNTS_PLUGIN_$UPPER))
#define EMPATHY_IS_ACCOUNTS_PLUGIN_$UPPER_CLASS(klass) \\
  (G_TYPE_CHECK_CLASS_TYPE((klass), \\
    EMPATHY_TYPE_ACCOUNTS_PLUGIN_$UPPER))
#define EMPATHY_ACCOUNTS_PLUGIN_$UPPER_GET_CLASS(obj) \\
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \\
    EMPATHY_TYPE_ACCOUNTS_PLUGIN_$UPPER, \\
    EmpathyAccountsPlugin$CamelClass))

GType ap_module_get_object_type (void);

G_END_DECLS

#endif /* #ifndef __EMPATHY_ACCOUNTS_PLUGIN_$UPPER_H__*/''' % (p.protocol)

    f.write(magic_replace (tmp, p.protocol))

def generate_plugin_code(p):
    # header
    f = open('account-plugins/empathy-accounts-plugin-%s.c' % p.protocol, 'w')

    tmp = '''/* # Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
 * Do NOT edit manually */

/*
 * empathy-accounts-plugin-%s.c
 *
 * Copyright (C) 2012 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"

#include "empathy-accounts-plugin-%s.h"

G_DEFINE_TYPE (EmpathyAccountsPlugin$Camel, empathy_accounts_plugin_$lower,\\
        EMPATHY_TYPE_ACCOUNTS_PLUGIN)

static void
empathy_accounts_plugin_$lower_class_init (
    EmpathyAccountsPlugin$CamelClass *klass)
{
}

static void
empathy_accounts_plugin_$lower_init (EmpathyAccountsPlugin$Camel *self)
{
}

GType
ap_module_get_object_type (void)
{
  return EMPATHY_TYPE_ACCOUNTS_PLUGIN_$UPPER;
}''' % (p.protocol, p.protocol)

    f.write(magic_replace (tmp, p.protocol))

def generate_build_block(p):
    la = 'lib%s_la' % p.protocol.replace('-', '_')

    output = '''%s_SOURCES = \\
	empathy-accounts-plugin-%s.c \\
	empathy-accounts-plugin-%s.h
%s_LDFLAGS = -module -avoid-version
%s_LIBADD = \\
	libempathy-uoa-account-plugin.la \\
	$(top_builddir)/libempathy/libempathy.la \\
	$(top_builddir)/libempathy-gtk/libempathy-gtk.la
''' % (la, p.protocol, p.protocol, la, la)

    return output


def generate_account_plugins(plugins):
    '''account-plugins/'''
    libs = []
    build_blocks = []

    for p in plugins:
        # empathy-accounts-plugin-$protocol.[ch]'''
        generate_plugin_header(p)
        generate_plugin_code(p)

        name = '	lib%s.la' % p.protocol
        libs.append(name)

        build_blocks.append(generate_build_block(p))

    # Makefile.am
    f = open('account-plugins/Makefile.am', 'w')

    f.write(
'''# Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
# Do NOT edit manually
plugindir = $(ACCOUNTS_PROVIDER_PLUGIN_DIR)

INCLUDES =					\\
	-I$(top_builddir)			\\
	-I$(top_srcdir)				\\
	-DLOCALEDIR=\\""$(datadir)/locale"\\"	\\
	$(UOA_CFLAGS)				\\
	$(WARN_CFLAGS)				\\
	$(ERROR_CFLAGS)				\\
	$(DISABLE_DEPRECATED)			\\
	$(EMPATHY_CFLAGS)

pkglib_LTLIBRARIES = libempathy-uoa-account-plugin.la

# this API is not stable and will never be, so use -release to make the
# SONAME of the plugin library change with every Empathy release.
libempathy_uoa_account_plugin_la_LDFLAGS = \\
   -no-undefined \\
   -release $(VERSION)

libempathy_uoa_account_plugin_la_SOURCES = \\
	empathy-accounts-plugin.c \\
	empathy-accounts-plugin.h \\
	empathy-accounts-plugin-widget.c \\
	empathy-accounts-plugin-widget.h

libempathy_uoa_account_plugin_la_LIBADD = \\
	$(UOA_LIBS)

plugin_LTLIBRARIES = \\
%s \\
	$(NULL)

%s''' % ('\\\n'.join(libs), '\n\n'.join(build_blocks)))

##### providers/ #####

def generate_provider_file(p):
    f = open('providers/%s.provider' % p.protocol, 'w')

    f.write(
'''<?xml version="1.0" encoding="UTF-8" ?>
<!-- Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
     Do NOT edit manually -->
<provider id="%s">
  <name>%s</name>
  <icon>%s</icon>
</provider>
''' % (p.protocol, p.name, p.icon))

def generate_providers(plugins):
    '''generate providers/*.provider files and providers/Makefile.am'''

    providers = []
    for p in plugins:
        providers.append('	%s.provider' % p.protocol)

        generate_provider_file(p)

    # providers/Makefile.am
    f = open('providers/Makefile.am', 'w')
    f.write(
'''# Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
# Do NOT edit manually
providersdir = $(ACCOUNTS_PROVIDER_FILES_DIR)

providers_DATA = \\
%s \\
	$(NULL)

EXTRA_DIST = $(providers_DATA)
''' % ('\\\n'.join(providers)))

##### services/ #####

def generate_service_file(p):
    f = open('services/%s-im.service' % p.protocol, 'w')

    f.write(
'''<?xml version="1.0" encoding="UTF-8" ?>
<!-- Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
     Do NOT edit manually -->
<service id="%s-im">
  <type>IM</type>
  <name>%s</name>
  <icon>%s</icon>
  <provider>%s</provider>

  <!-- default settings (account settings have precedence over these) -->
  <template>
    <group name="telepathy">
      <setting name="manager">%s</setting>
      <setting name="protocol">%s</setting>
    </group>
    <group name="auth">
      <setting name="method">password</setting>
      <setting name="mechanism">password</setting>
    </group>
  </template>

</service>
''' % (p.protocol, p.name, p.icon, p.protocol, p.cm, p.protocol))

def generate_services(plugins):
    '''generate services/*-im.service files and services/Makefile.am'''

    services = []
    for p in plugins:
        services.append('	%s-im.service' % p.protocol)

        generate_service_file(p)

    # providers/Makefile.am
    f = open('services/Makefile.am', 'w')
    f.write(
'''# Generated using empathy/ubuntu-online-accounts/cc-plugins/generate-plugins.py
# Do NOT edit manually
servicesdir = $(ACCOUNTS_SERVICE_FILES_DIR)

services_DATA = \\
%s \\
	$(NULL)

EXTRA_DIST = $(services_DATA)
''' % ('\\\n'.join(services)))

def generate_all():
    plugins = []

    for name, cm, protocol, icon in ALL:
        plugins.append(Plugin(name, cm, protocol, icon))

    generate_account_plugins(plugins)
    generate_providers(plugins)
    generate_services(plugins)

if __name__ == '__main__':
    generate_all()
