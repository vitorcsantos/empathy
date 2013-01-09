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
  <plugin>empathy-accounts-plugin</plugin>
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

    generate_providers(plugins)
    generate_services(plugins)

if __name__ == '__main__':
    generate_all()
