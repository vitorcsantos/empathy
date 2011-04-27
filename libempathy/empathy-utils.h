/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2003-2007 Imendio AB
 * Copyright (C) 2007-2011 Collabora Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * Authors: Richard Hult <richard@imendio.com>
 *          Martyn Russell <martyn@imendio.com>
 *          Xavier Claessens <xclaesse@gmail.com>
 */

#ifndef __EMPATHY_UTILS_H__
#define __EMPATHY_UTILS_H__

#include <glib.h>
#include <glib-object.h>

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <folks/folks.h>
#include <telepathy-glib/account-manager.h>

#include "empathy-contact.h"

#define EMPATHY_GET_PRIV(obj,type) ((type##Priv *) ((type *) obj)->priv)
#define EMP_STR_EMPTY(x) ((x) == NULL || (x)[0] == '\0')

G_BEGIN_DECLS

void         empathy_init                           (void);
/* Strings */
gchar *      empathy_substring                      (const gchar     *str,
						    gint             start,
						    gint             end);
gint         empathy_strcasecmp                     (const gchar     *s1,
						    const gchar     *s2);
gint         empathy_strncasecmp                    (const gchar     *s1,
						    const gchar     *s2,
						    gsize            n);

/* XML */
gboolean     empathy_xml_validate                   (xmlDoc          *doc,
						    const gchar     *dtd_filename);
xmlNodePtr   empathy_xml_node_get_child             (xmlNodePtr       node,
						    const gchar     *child_name);
xmlChar *    empathy_xml_node_get_child_content     (xmlNodePtr       node,
						    const gchar     *child_name);
xmlNodePtr   empathy_xml_node_find_child_prop_value (xmlNodePtr       node,
						    const gchar     *prop_name,
						    const gchar     *prop_value);

/* Others */
const gchar * empathy_presence_get_default_message  (TpConnectionPresenceType presence);
const gchar * empathy_presence_to_str               (TpConnectionPresenceType presence);
TpConnectionPresenceType empathy_presence_from_str  (const gchar     *str);
gchar *       empathy_file_lookup                   (const gchar     *filename,
						     const gchar     *subdir);
gboolean     empathy_proxy_equal                    (gconstpointer    a,
						     gconstpointer    b);
guint        empathy_proxy_hash                     (gconstpointer    key);
gboolean     empathy_check_available_state          (void);
gint        empathy_uint_compare                    (gconstpointer a,
						     gconstpointer b);

const gchar * empathy_account_get_error_message (TpAccount *account,
                                                 gboolean *user_requested);

gchar *empathy_protocol_icon_name (const gchar *protocol);
const gchar *empathy_protocol_name_to_display_name (const gchar *proto_name);

#define EMPATHY_ARRAY_TYPE_OBJECT (empathy_type_dbus_ao ())
GType empathy_type_dbus_ao (void);

TpAccount * empathy_get_account_for_connection (TpConnection *connection);

gboolean empathy_account_manager_get_accounts_connected (gboolean *connecting);

void empathy_connect_new_account (TpAccount *account,
    TpAccountManager *account_manager);

TpConnectionPresenceType empathy_folks_presence_type_to_tp (FolksPresenceType type);
gboolean empathy_folks_individual_contains_contact (FolksIndividual *individual);
EmpathyContact * empathy_contact_dup_from_folks_individual (FolksIndividual *individual);
TpChannelGroupChangeReason tp_channel_group_change_reason_from_folks_groups_change_reason (FolksGroupDetailsChangeReason reason);
gboolean empathy_folks_persona_is_interesting (FolksPersona *persona);

gchar * empathy_get_x509_certificate_hostname (gnutls_x509_crt_t cert);

gchar *empathy_format_currency (gint amount, guint scale, const gchar *currency);

/* this enum is taken from SkypeKit */
enum {
  EMP_SKYPE_LOGOUTREASON_NONE = 0, /* No reason given */ 
  EMP_SKYPE_LOGOUTREASON_LOGOUT_CALLED = 1, /* manual logout (or unknown reason from previous session) */ 
  EMP_SKYPE_LOGOUTREASON_HTTPS_PROXY_AUTH_FAILED = 2, /* sync errors at login/registration */ 
  EMP_SKYPE_LOGOUTREASON_SOCKS_PROXY_AUTH_FAILED = 3, /* sync errors at login/registration */ 
  EMP_SKYPE_LOGOUTREASON_P2P_CONNECT_FAILED = 4, /* sync errors at login/registration */ 
  EMP_SKYPE_LOGOUTREASON_SERVER_CONNECT_FAILED = 5, /* sync errors at login/registration */ 
  EMP_SKYPE_LOGOUTREASON_SERVER_OVERLOADED = 6, /* sync errors at login/registration */ 
  EMP_SKYPE_LOGOUTREASON_DB_IN_USE = 7, /* sync errors at login/registration */ 
  EMP_SKYPE_LOGOUTREASON_INVALID_SKYPENAME = 8, /* sync errors at registration */ 
  EMP_SKYPE_LOGOUTREASON_INVALID_EMAIL = 9, /* sync errors at registration */ 
  EMP_SKYPE_LOGOUTREASON_UNACCEPTABLE_PASSWORD = 10, /* sync errors at registration */ 
  EMP_SKYPE_LOGOUTREASON_SKYPENAME_TAKEN = 11, /* sync errors at registration */ 
  EMP_SKYPE_LOGOUTREASON_REJECTED_AS_UNDERAGE = 12, /* sync errors at registration */ 
  EMP_SKYPE_LOGOUTREASON_NO_SUCH_IDENTITY = 13, /* sync errors at login */ 
  EMP_SKYPE_LOGOUTREASON_INCORRECT_PASSWORD = 14, /* sync errors at login */ 
  EMP_SKYPE_LOGOUTREASON_TOO_MANY_LOGIN_ATTEMPTS = 15, /* sync errors at login */ 
  EMP_SKYPE_LOGOUTREASON_PASSWORD_HAS_CHANGED = 16, /* async errors (can happen anytime while logged in) */ 
  EMP_SKYPE_LOGOUTREASON_PERIODIC_UIC_UPDATE_FAILED = 17, /* async errors (can happen anytime while logged in) */ 
  EMP_SKYPE_LOGOUTREASON_DB_DISK_FULL = 18, /* async errors (can happen anytime while logged in) */ 
  EMP_SKYPE_LOGOUTREASON_DB_IO_ERROR = 19, /* async errors (can happen anytime while logged in) */ 
  EMP_SKYPE_LOGOUTREASON_DB_CORRUPT = 20, /* async errors (can happen anytime while logged in) */ 
  EMP_SKYPE_LOGOUTREASON_DB_FAILURE = 21, /* deprecated (superceded by more detailed DB_* errors) */ 
  EMP_SKYPE_LOGOUTREASON_INVALID_APP_ID = 22, /* platform sdk */ 
  EMP_SKYPE_LOGOUTREASON_APP_ID_BLACKLISTED = 23, /* platform sdk */ 
  EMP_SKYPE_LOGOUTREASON_UNSUPPORTED_VERSION = 24, /* forced upgrade/discontinuation */ 
};

const gchar *empathy_skype_reason_to_string (guint skype_reason);

G_END_DECLS

#endif /*  __EMPATHY_UTILS_H__ */
