/*
 * Copyright (C) 2011 Collabora Ltd.
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
 * Authors: Danielle Madeley <danielle.madeley@collabora.co.uk>
 */

#include <config.h>

#include <string.h>

#include <glib/gi18n-lib.h>

#include <extensions/extensions.h>

#include <libempathy/empathy-utils.h>

#include "empathy-account-widget-private.h"
#include "empathy-ui-utils.h"

#define DEBUG_FLAG EMPATHY_DEBUG_ACCOUNT
#include <libempathy/empathy-debug.h>

#define GET_PRIV(obj) EMPATHY_GET_PRIV (obj, EmpathyAccountWidget)

enum {
    PS_COL_ENUM_VALUE,
    PS_COL_DISPLAY_NAME,
    NUM_PS_COLS
};

static void
account_widget_skype_combo_changed_cb (GtkComboBox *combo,
    EmpathyAccountWidget *self)
{
  EmpathyAccountWidgetPriv *priv = GET_PRIV (self);
  const char *prop_name = g_object_get_data (G_OBJECT (combo), "dbus-property");
  TpConnection *conn;
  EmpPrivacySetting prop_value;
  GtkTreeIter iter;
  GValue value = { 0, };

  gtk_combo_box_get_active_iter (combo, &iter);
  gtk_tree_model_get (gtk_combo_box_get_model (combo), &iter,
      PS_COL_ENUM_VALUE, &prop_value,
      -1);

  DEBUG ("Combo changed: %s", prop_name);

  g_value_init (&value, G_TYPE_UINT);
  g_value_set_uint (&value, prop_value);

  conn = tp_account_get_connection (
      empathy_account_settings_get_account (priv->settings));
  tp_cli_dbus_properties_call_set (conn, -1,
      EMP_IFACE_CONNECTION_INTERFACE_PRIVACY_SETTINGS,
      prop_name, &value, NULL, NULL, NULL, NULL);

  g_value_unset (&value);
}

static void
account_widget_skype_toggle_changed_cb (GtkToggleButton *toggle,
    EmpathyAccountWidget *self)
{
  EmpathyAccountWidgetPriv *priv = GET_PRIV (self);
  const char *prop_name = g_object_get_data (G_OBJECT (toggle),
      "dbus-property");
  TpConnection *conn;
  GValue value = { 0, };

  DEBUG ("Toggle changed: %s", prop_name);

  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, gtk_toggle_button_get_active (toggle));

  conn = tp_account_get_connection (
      empathy_account_settings_get_account (priv->settings));
  tp_cli_dbus_properties_call_set (conn, -1,
      EMP_IFACE_CONNECTION_INTERFACE_PRIVACY_SETTINGS,
      prop_name, &value, NULL, NULL, NULL, NULL);

  g_value_unset (&value);
}

static void
account_widget_skype_set_value (EmpathyAccountWidget *self,
    GtkWidget *widget,
    GValue *value)
{
  g_return_if_fail (value != NULL);

  if (GTK_IS_COMBO_BOX (widget))
    {
      GtkTreeModel *model =
        gtk_combo_box_get_model (GTK_COMBO_BOX (widget));
      guint prop_value;
      GtkTreeIter iter;
      gboolean valid;

      g_return_if_fail (G_VALUE_HOLDS_UINT (value));

      prop_value = g_value_get_uint (value);

      g_signal_handlers_block_by_func (widget,
          account_widget_skype_combo_changed_cb, self);

      for (valid = gtk_tree_model_get_iter_first (model, &iter);
           valid;
           valid = gtk_tree_model_iter_next (model, &iter))
        {
          guint v;

          gtk_tree_model_get (model, &iter,
              PS_COL_ENUM_VALUE, &v,
              -1);

          if (v == prop_value)
            {
              gtk_combo_box_set_active_iter (GTK_COMBO_BOX (widget), &iter);
              break;
            }
        }

      g_signal_handlers_unblock_by_func (widget,
          account_widget_skype_combo_changed_cb, self);
    }
  else if (GTK_IS_TOGGLE_BUTTON (widget))
    {
      g_return_if_fail (G_VALUE_HOLDS_BOOLEAN (value));

      g_signal_handlers_block_by_func (widget,
          account_widget_skype_toggle_changed_cb, self);

      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget),
          g_value_get_boolean (value));

      g_signal_handlers_unblock_by_func (widget,
          account_widget_skype_toggle_changed_cb, self);
    }
  else
    {
      g_assert_not_reached ();
    }
}

static void
account_widget_build_skype_get_privacy_settings_cb (TpProxy *cm,
    GHashTable *props,
    const GError *in_error,
    gpointer user_data,
    GObject *weak_obj)
{
  EmpathyAccountWidget *self = EMPATHY_ACCOUNT_WIDGET (weak_obj);
  GtkBuilder *gui = user_data;
  guint i;

  static const char *widgets[] = {
      "allow-text-chats-from",
      "allow-skype-calls-from",
      "allow-outside-calls-from",
      "show-my-avatar-to",
      "show-my-web-status"
  };

  if (in_error != NULL)
    {
      GtkWidget *table, *infobar, *label;

      DEBUG ("Failed to get properties: %s", in_error->message);

      table = GTK_WIDGET (gtk_builder_get_object (gui,
            "privacy-settings-table"));
      gtk_widget_set_sensitive (table, FALSE);

      infobar = gtk_info_bar_new ();
      gtk_box_pack_start (
          GTK_BOX (gtk_builder_get_object (gui, "privacy-settings-vbox")),
          infobar, FALSE, TRUE, 0);
      gtk_info_bar_set_message_type (GTK_INFO_BAR (infobar), GTK_MESSAGE_ERROR);
      label = gtk_label_new (_("Failed to retrieve privacy settings."));
      gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
      gtk_container_add (GTK_CONTAINER (
          gtk_info_bar_get_content_area (GTK_INFO_BAR (infobar))),
        label);
      gtk_widget_show (label);

      return;
    }

  for (i = 0; i < G_N_ELEMENTS (widgets); i++)
    {
      GtkWidget *widget = GTK_WIDGET (gtk_builder_get_object (gui, widgets[i]));
      const char *prop_name = g_object_get_data (G_OBJECT (widget),
          "dbus-property");

      DEBUG ("Widget '%s' (%s), prop = %s",
          widgets[i], G_OBJECT_TYPE_NAME (widget), prop_name);

      account_widget_skype_set_value (self, widget,
          g_hash_table_lookup (props, prop_name));
    }
}

static void
account_widget_skype_properties_changed_cb (TpProxy *conn,
    const char *interface,
    GHashTable *props,
    const char **invalidated,
    gpointer user_data,
    GObject *weak_obj)
{
  EmpathyAccountWidget *self = EMPATHY_ACCOUNT_WIDGET (weak_obj);
  GtkWidget *table = user_data;
  GHashTableIter iter;
  const char *prop;
  GValue *value;

  g_hash_table_iter_init (&iter, props);
  while (g_hash_table_iter_next (&iter, (gpointer) &prop, (gpointer) &value))
    {
      GList *children, *ptr;

      DEBUG ("Property changed: %s", prop);

      /* find this value in the widget tree */
      children = gtk_container_get_children (GTK_CONTAINER (table));

      for (ptr = children; ptr != NULL; ptr = ptr->next)
        {
          GtkWidget *widget = ptr->data;
          const char *prop_name = g_object_get_data (G_OBJECT (widget),
              "dbus-property");

          if (!tp_strdiff (prop_name, prop))
            {
              DEBUG ("Got child %p (%s)", widget, G_OBJECT_TYPE_NAME (widget));

              account_widget_skype_set_value (self, widget, value);
              break;
            }
        }

      g_list_free (children);
    }
}

/**
 * account_widget_build_skype_setup_combo:
 * @gui:
 * @widget:
 * @first_option: a list of options from the enum, terminated by -1
 */
static void
account_widget_build_skype_setup_combo (EmpathyAccountWidget *self,
    GtkBuilder *gui,
    const char *widget,
    const char *prop_name,
    int first_option,
    ...)
{
  GtkWidget *combo;
  GtkListStore *store;
  va_list var_args;
  int option;

  static const char *options[NUM_EMP_PRIVACY_SETTINGS] = {
        N_("Nobody"),
        N_("Contacts"),
        N_("Anyone"),
        N_("Known Numbers")
  };

  combo = GTK_WIDGET (gtk_builder_get_object (gui, widget));

  g_return_if_fail (combo != NULL);

  store = gtk_list_store_new (NUM_PS_COLS,
      G_TYPE_UINT, /* PS_COL_ENUM_VALUE */
      G_TYPE_STRING /* PS_COL_DISPLAY_NAME */
      );
  gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));

  va_start (var_args, first_option);

  for (option = first_option; option != -1; option = va_arg (var_args, int))
    {
      gtk_list_store_insert_with_values (store, NULL, -1,
          PS_COL_ENUM_VALUE, option,
          PS_COL_DISPLAY_NAME, gettext(options[option]),
          -1);
    }

  va_end (var_args);

  g_object_set_data (G_OBJECT (combo), "dbus-property", (gpointer) prop_name);
  g_signal_connect (combo, "changed",
      G_CALLBACK (account_widget_skype_combo_changed_cb), self);
}

static void
account_widget_skype_privacy_settings (GtkWidget *button,
    EmpathyAccountWidget *self)
{
  EmpathyAccountWidgetPriv *priv = GET_PRIV (self);
  TpConnection *conn;
  char *filename;
  GtkBuilder *gui;
  GtkWidget *dialog, *show_my_web_status, *table, *vbox;
  GtkWidget *toplevel, *infobar, *label;

  DEBUG ("Loading privacy settings");

  filename = empathy_file_lookup ("empathy-account-widget-skype.ui",
      "libempathy-gtk");
  gui = empathy_builder_get_file (filename,
      "privacy-settings-dialog", &dialog,
      "privacy-settings-table", &table,
      "privacy-settings-vbox", &vbox,
      "show-my-web-status", &show_my_web_status,
      NULL);

  toplevel = gtk_widget_get_toplevel (self->ui_details->widget);
  if (gtk_widget_is_toplevel (toplevel) && GTK_IS_WINDOW (toplevel))
    gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (toplevel));

  /* make the settings insensitive when the account is disconnected */
  tp_account_bind_connection_status_to_property (
      empathy_account_settings_get_account (priv->settings),
      table, "sensitive", FALSE);

  /* set up an informative info bar */
  infobar = gtk_info_bar_new ();
  gtk_box_pack_start (GTK_BOX (vbox), infobar, FALSE, TRUE, 0);
  gtk_info_bar_set_message_type (GTK_INFO_BAR (infobar), GTK_MESSAGE_INFO);
  label = gtk_label_new (_("Privacy settings can only be changed while the "
                           "account is connected."));
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_container_add (
      GTK_CONTAINER (gtk_info_bar_get_content_area (GTK_INFO_BAR (infobar))),
      label);
  gtk_widget_show (label);
  g_object_bind_property (table, "sensitive", infobar, "visible",
      G_BINDING_SYNC_CREATE | G_BINDING_INVERT_BOOLEAN);

  account_widget_build_skype_setup_combo (self, gui,
      "allow-text-chats-from", "AllowTextChannelsFrom",
      EMP_PRIVACY_SETTING_ANYONE,
      EMP_PRIVACY_SETTING_CONTACTS,
      -1);
  account_widget_build_skype_setup_combo (self, gui,
      "allow-skype-calls-from", "AllowCallChannelsFrom",
      EMP_PRIVACY_SETTING_ANYONE,
      EMP_PRIVACY_SETTING_CONTACTS,
      -1);
  account_widget_build_skype_setup_combo (self, gui,
      "allow-outside-calls-from", "AllowOutsideCallsFrom",
      EMP_PRIVACY_SETTING_ANYONE,
      EMP_PRIVACY_SETTING_KNOWN_NUMBERS,
      EMP_PRIVACY_SETTING_CONTACTS,
      -1);
  account_widget_build_skype_setup_combo (self, gui,
      "show-my-avatar-to", "ShowMyAvatarTo",
      EMP_PRIVACY_SETTING_ANYONE,
      EMP_PRIVACY_SETTING_CONTACTS,
      -1);

  g_object_set_data (G_OBJECT (show_my_web_status), "dbus-property",
      "ShowMyWebStatus");
  g_signal_connect (show_my_web_status, "toggled",
      G_CALLBACK (account_widget_skype_toggle_changed_cb), self);

  /* get the current parameter values from psyke */
  conn = tp_account_get_connection (
      empathy_account_settings_get_account (priv->settings));

  tp_cli_dbus_properties_call_get_all (conn, -1,
      EMP_IFACE_CONNECTION_INTERFACE_PRIVACY_SETTINGS,
      account_widget_build_skype_get_privacy_settings_cb,
      g_object_ref (gui), g_object_unref,
      G_OBJECT (self));
  tp_cli_dbus_properties_connect_to_properties_changed (conn,
      account_widget_skype_properties_changed_cb,
      table, NULL, G_OBJECT (self), NULL);

  g_object_unref (gui);

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void
empathy_account_widget_build_skype (EmpathyAccountWidget *self,
    const char *filename)
{
  EmpathyAccountWidgetPriv *priv = GET_PRIV (self);

  if (priv->simple || priv->creating_account)
    {
      /* if we don't have an account it means we're doing the initial setup */
      self->ui_details->gui = empathy_builder_get_file (filename,
          "table_common_skype_settings_setup", &priv->table_common_settings,
          "vbox_skype_settings_setup", &self->ui_details->widget,
          NULL);

      empathy_account_widget_handle_params (self,
          "entry_id_setup", "account",
          "entry_password_setup", "password",
          NULL);

      self->ui_details->default_focus = g_strdup ("entry_id_setup");
    }
  else
    {
      TpAccount *account =
        empathy_account_settings_get_account (priv->settings);
      GtkWidget *edit_privacy_settings_button;
      GtkWidget *plugged_into_skype_logo, *canonical_logo;
      char *logo;

      self->ui_details->gui = empathy_builder_get_file (filename,
          "table_common_skype_settings", &priv->table_common_settings,
          "vbox_skype_settings", &self->ui_details->widget,
          "edit-privacy-settings-button", &edit_privacy_settings_button,
          "plugged-into-skype-logo", &plugged_into_skype_logo,
          "canonical-logo", &canonical_logo,
          NULL);

      empathy_builder_connect (self->ui_details->gui, self,
          "edit-privacy-settings-button", "clicked",
              account_widget_skype_privacy_settings,
          NULL);

      if (account != NULL)
        tp_account_bind_connection_status_to_property (account,
            edit_privacy_settings_button, "sensitive", FALSE);
      else
        gtk_widget_set_sensitive (edit_privacy_settings_button, FALSE);

      logo = empathy_file_lookup ("plugged-into-skype.png", "data");
      gtk_image_set_from_file (GTK_IMAGE (plugged_into_skype_logo), logo);
      g_free (logo);

      logo = empathy_file_lookup ("canonical-logo.png", "data");
      gtk_image_set_from_file (GTK_IMAGE (canonical_logo), logo);
      g_free (logo);

      empathy_account_widget_handle_params (self,
          "entry_id", "account",
          "entry_password", "password",
          NULL);

      self->ui_details->default_focus = g_strdup ("entry_id");
    }
}
