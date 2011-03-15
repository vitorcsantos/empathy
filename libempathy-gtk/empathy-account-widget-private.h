/*
 * Copyright (C) 2009 Collabora Ltd.
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
 * Authors: Cosimo Cecchi <cosimo.cecchi@collabora.co.uk>
 */

#ifndef __EMPATHY_ACCOUNT_WIDGET_PRIVATE_H__
#define __EMPATHY_ACCOUNT_WIDGET_PRIVATE_H__

#include <libempathy-gtk/empathy-account-widget.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "empathy-account-widget-irc.h"

G_BEGIN_DECLS

struct _EmpathyAccountWidgetUIDetails {
  GtkWidget *widget;

  GtkBuilder *gui;

  char *default_focus;

  /* an optional callback to call after calling
   * empathy_account_settings_apply_async () -- must use a GSimpleAsyncResult
   * which sets a gboolean indicating reconnect_required */
  void (* additional_apply_async) (EmpathyAccountWidget *,
      GAsyncReadyCallback callback,
      gpointer user_data);
};

typedef struct {
  EmpathyAccountSettings *settings;

  GtkWidget *table_common_settings;
  GtkWidget *apply_button;
  GtkWidget *cancel_button;
  GtkWidget *entry_password;
  GtkWidget *spinbutton_port;
  GtkWidget *enabled_checkbox;
  GtkWidget *radiobutton_reuse;

  gboolean simple;
  gboolean enabled;

  gboolean contains_pending_changes;

  /* An EmpathyAccountWidget can be used to either create an account or
   * modify it. When we are creating an account, this member is set to TRUE */
  gboolean creating_account;

  /* whether there are any other real accounts. Necessary so we know whether
   * it's safe to dismiss this widget in some cases (eg, whether the Cancel
   * button should be sensitive) */
  gboolean other_accounts_exist;

  /* if TRUE, the GTK+ destroy signal has been fired and so the widgets
   * embedded in this account widget can't be used any more
   * workaround because some async callbacks can be called after the
   * widget has been destroyed */
  gboolean destroyed;

  TpAccountManager *account_manager;

  GtkWidget *param_account_widget;
  GtkWidget *param_password_widget;

  gboolean automatic_change;
  GtkWidget *remember_password_widget;

  /* Used only for IRC accounts */
  EmpathyIrcNetworkChooser *irc_network_chooser;

  gboolean dispose_run;
} EmpathyAccountWidgetPriv;

void empathy_account_widget_handle_params (EmpathyAccountWidget *self,
    const gchar *first_widget,
    ...);

void empathy_account_widget_setup_widget (EmpathyAccountWidget *self,
    GtkWidget *widget,
    const gchar *param_name);

G_END_DECLS

#endif /* __EMPATHY_ACCOUNT_WIDGET_PRIVATE_H__ */
