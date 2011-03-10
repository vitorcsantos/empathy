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
 *          Emilio Pozuelo Monfort <emilio.pozuelo@collabora.co.uk>
 */

#include <config.h>

#include <string.h>

#include <glib/gi18n-lib.h>

#include <extensions/extensions.h>

#include <libempathy/empathy-utils.h>
#include <libempathy/empathy-server-sasl-handler.h>

#include "empathy-account-widget-skype.h"
#include "empathy-account-widget-private.h"
#include "empathy-ui-utils.h"

#define DEBUG_FLAG EMPATHY_DEBUG_ACCOUNT
#include <libempathy/empathy-debug.h>

#define GET_PRIV(obj) EMPATHY_GET_PRIV (obj, EmpathyAccountWidget)

typedef struct
{
  TpAccount *account;
  TpChannel *channel;
  char *password;
} ObserveChannelsData;

static ObserveChannelsData *
observe_channels_data_new (TpAccount *account,
    TpChannel *channel,
    const char *password)
{
  ObserveChannelsData *data = g_slice_new0 (ObserveChannelsData);

  data->account = g_object_ref (account);
  data->channel = g_object_ref (channel);
  data->password = g_strdup (password);

  return data;
}

static void
observe_channels_data_free (ObserveChannelsData *data)
{
  g_object_unref (data->account);
  g_object_unref (data->channel);
  g_free (data->password);

  g_slice_free (ObserveChannelsData, data);
}

static void
auth_observer_sasl_handler_invalidated (EmpathyServerSASLHandler *sasl_handler,
    gpointer user_data)
{
  DEBUG ("SASL Handler done");

  g_object_unref (sasl_handler);
}

static void
auth_observer_new_sasl_handler_cb (GObject *obj,
    GAsyncResult *result,
    gpointer user_data)
{
  ObserveChannelsData *data = user_data;
  EmpathyServerSASLHandler *sasl_handler;
  GError *error = NULL;

  sasl_handler = empathy_server_sasl_handler_new_finish (result, &error);
  if (error != NULL)
    {
      DEBUG ("Failed to create SASL handler: %s", error->message);

      tp_channel_close_async (data->channel, NULL, NULL);

      g_error_free (error);
      goto finally;
    }

  DEBUG ("providing password");

  g_signal_connect (sasl_handler, "invalidated",
      G_CALLBACK (auth_observer_sasl_handler_invalidated), NULL);
  empathy_server_sasl_handler_provide_password (sasl_handler,
      data->password, TRUE);

finally:
  observe_channels_data_free (data);
}

static void
auth_observer_claim_cb (GObject *dispatch_operation,
    GAsyncResult *result,
    gpointer user_data)
{
  ObserveChannelsData *data = user_data;
  GError *error = NULL;

  if (!tp_channel_dispatch_operation_claim_finish (
        TP_CHANNEL_DISPATCH_OPERATION (dispatch_operation), result, &error))
    {
      DEBUG ("Failed to claim auth channel");

      g_error_free (error);
      observe_channels_data_free (data);
      return;
    }

  empathy_server_sasl_handler_new_async (data->account, data->channel,
      auth_observer_new_sasl_handler_cb, data);
}

static void
auth_observer_observe_channels (TpSimpleObserver *auth_observer,
    TpAccount *account,
    TpConnection *connection,
    GList *channels,
    TpChannelDispatchOperation *dispatch_operation,
    GList *requests,
    TpObserveChannelsContext *context,
    gpointer user_data)
{
  TpChannel *channel;
  GHashTable *props;
  GStrv available_mechanisms;
  GtkWidget *password_entry = user_data;
  const char *password = NULL;

  /* we only do this for Psyke */
  if (tp_strdiff (
        tp_connection_get_connection_manager_name (connection),
        "psyke"))
    goto except;

  /* can only deal with one channel */
  if (g_list_length (channels) != 1)
    goto except;

  channel = channels->data;
  props = tp_channel_borrow_immutable_properties (channel);
  available_mechanisms = tp_asv_get_boxed (props,
      TP_PROP_CHANNEL_INTERFACE_SASL_AUTHENTICATION_AVAILABLE_MECHANISMS,
      G_TYPE_STRV);

  /* must support X-TELEPATHY-PASSWORD */
  if (!tp_strv_contains ((const char * const *) available_mechanisms,
        "X-TELEPATHY-PASSWORD"))
    goto except;

  /* do we have a password */
  if (g_object_get_data (G_OBJECT (password_entry), "fake-password") == NULL)
    password = gtk_entry_get_text (GTK_ENTRY (password_entry));

  if (tp_str_empty (password))
    goto except;

  DEBUG ("claiming auth channel");

  tp_channel_dispatch_operation_claim_async (dispatch_operation,
      auth_observer_claim_cb,
      observe_channels_data_new (account, channel, password));

  tp_observe_channels_context_accept (context);
  return;

except:
  tp_observe_channels_context_accept (context);
}

static TpBaseClient *
auth_observer_new (GtkWidget *password_entry)
{
  TpDBusDaemon *dbus;
  TpBaseClient *auth_observer;
  GError *error = NULL;

  dbus = tp_dbus_daemon_dup (&error);

  if (error != NULL)
    {
      g_warning ("Failed to get DBus daemon: %s", error->message);
      g_error_free (error);
      return NULL;
    }

  auth_observer = tp_simple_observer_new (dbus, FALSE, "Empathy.PsykePreAuth",
      FALSE, auth_observer_observe_channels, password_entry, NULL);

  tp_base_client_set_observer_delay_approvers (auth_observer, TRUE);
  tp_base_client_take_observer_filter (auth_observer, tp_asv_new (
        TP_PROP_CHANNEL_CHANNEL_TYPE,
        G_TYPE_STRING,
        TP_IFACE_CHANNEL_TYPE_SERVER_AUTHENTICATION,

        TP_PROP_CHANNEL_TYPE_SERVER_AUTHENTICATION_AUTHENTICATION_METHOD,
        G_TYPE_STRING,
        TP_IFACE_CHANNEL_INTERFACE_SASL_AUTHENTICATION,

        NULL));

  if (!tp_base_client_register (auth_observer, &error))
    {
      DEBUG ("Failed to register Psyke pre-auth observer: %s", error->message);
      g_error_free (error);
    }

  g_object_unref (dbus);

  return auth_observer;
}

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
account_widget_build_skype_get_password_saved_cb (TpProxy *account,
    const GValue *value,
    const GError *in_error,
    gpointer user_data,
    GObject *password_entry)
{
  gboolean password_saved;

  if (in_error != NULL)
    {
      DEBUG ("Failed to get PasswordSaved: %s", in_error->message);
      return;
    }

  g_return_if_fail (G_VALUE_HOLDS_BOOLEAN (value));

  password_saved = g_value_get_boolean (value);

  DEBUG ("PasswordSaved: %s", password_saved ? "yes" : "no");

  gtk_entry_set_text (GTK_ENTRY (password_entry),
      password_saved ? "xxxxxxxx": "");

  g_object_set_data (password_entry, "fake-password",
      GUINT_TO_POINTER (password_saved));
}

static void
account_widget_build_skype_account_properties_changed_cb (TpProxy *account,
    const char *iface,
    GHashTable *changed,
    const char **invalidated,
    gpointer user_data,
    GObject *password_entry)
{
  GValue *value;

  if (tp_strdiff (iface,
        EMP_IFACE_ACCOUNT_INTERFACE_EXTERNAL_PASSWORD_STORAGE))
    return;

  value = g_hash_table_lookup (changed, "PasswordSaved");

  if (value == NULL)
    return;

  account_widget_build_skype_get_password_saved_cb (account, value, NULL,
      NULL, password_entry);
}

static void
account_widget_build_skype_password_entry_focus (GtkWidget *password_entry,
    GdkEventFocus *event,
    gpointer user_data)
{
  if (g_object_get_data (G_OBJECT (password_entry), "fake-password") == NULL)
    return;

  DEBUG ("Clearing fake password for editing");

  gtk_entry_set_text (GTK_ENTRY (password_entry), "");
  g_object_set_data (G_OBJECT (password_entry), "fake-password",
      GUINT_TO_POINTER (FALSE));

  /* FIXME: need to light up the apply/cancel buttons */
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
      "show-my-web-status",
      "show-i-have-video-to"
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
          PS_COL_DISPLAY_NAME, gettext (options[option]),
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
      "show-i-have-video-to", "ShowIHaveVideoTo",
      EMP_PRIVACY_SETTING_ANYONE,
      EMP_PRIVACY_SETTING_CONTACTS,
      EMP_PRIVACY_SETTING_NOBODY,
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

static void
account_widget_build_skype_set_pixmap (GtkBuilder *gui,
    const char *widget,
    const char *file)
{
  GtkImage *image = GTK_IMAGE (gtk_builder_get_object (gui, widget));
  char *filename = empathy_file_lookup (file, "data");

  gtk_image_set_from_file (image, filename);

  g_free (filename);
}

void
empathy_account_widget_build_skype (EmpathyAccountWidget *self,
    const char *filename)
{
  EmpathyAccountWidgetPriv *priv = GET_PRIV (self);
  TpAccount *account = empathy_account_settings_get_account (priv->settings);
  GtkWidget *password_entry;

  if (priv->simple || priv->creating_account)
    {
      GtkWidget *skype_info;

      /* if we don't have an account it means we're doing the initial setup */
      self->ui_details->gui = empathy_builder_get_file (filename,
          "table_common_skype_settings_setup", &priv->table_common_settings,
          "vbox_skype_settings_setup", &self->ui_details->widget,
          "skype-info-vbox", &skype_info,
          "entry_password_setup", &password_entry,
          NULL);

      account_widget_build_skype_set_pixmap (self->ui_details->gui,
          "plugged-into-skype-logo", "plugged-into-skype.png");
      account_widget_build_skype_set_pixmap (self->ui_details->gui,
          "canonical-logo", "canonical-logo.png");

      gtk_box_pack_end (GTK_BOX (self->ui_details->widget), skype_info,
          TRUE, TRUE, 0);

      empathy_account_widget_handle_params (self,
          "entry_id_setup", "account",
          NULL);

      self->ui_details->default_focus = g_strdup ("entry_id_setup");
    }
  else
    {
      GtkWidget *edit_privacy_settings_button, *skype_info;

      self->ui_details->gui = empathy_builder_get_file (filename,
          "table_common_skype_settings", &priv->table_common_settings,
          "vbox_skype_settings", &self->ui_details->widget,
          "skype-info-vbox", &skype_info,
          "edit-privacy-settings-button", &edit_privacy_settings_button,
          "entry_password", &password_entry,
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

      account_widget_build_skype_set_pixmap (self->ui_details->gui,
          "plugged-into-skype-logo", "plugged-into-skype.png");
      account_widget_build_skype_set_pixmap (self->ui_details->gui,
          "canonical-logo", "canonical-logo.png");

      gtk_box_pack_end (GTK_BOX (self->ui_details->widget), skype_info,
          TRUE, TRUE, 0);

      empathy_account_widget_handle_params (self,
          "entry_id", "account",
          NULL);

      self->ui_details->default_focus = g_strdup ("entry_id");
    }

  /* create the Psyke pre-authentication observer --
   * tie the lifetime of the observer to the lifetime of the widget */
  g_object_set_data_full (G_OBJECT (self->ui_details->widget), "auth-observer",
      auth_observer_new (password_entry), g_object_unref);

  /* find out if we know the password */
  if (tp_proxy_has_interface_by_id (account,
        EMP_IFACE_QUARK_ACCOUNT_INTERFACE_EXTERNAL_PASSWORD_STORAGE))
    {
      tp_cli_dbus_properties_call_get (account, -1,
          EMP_IFACE_ACCOUNT_INTERFACE_EXTERNAL_PASSWORD_STORAGE,
          "PasswordSaved",
          account_widget_build_skype_get_password_saved_cb,
          NULL, NULL, G_OBJECT (password_entry));
      tp_cli_dbus_properties_connect_to_properties_changed (account,
          account_widget_build_skype_account_properties_changed_cb,
          NULL, NULL, G_OBJECT (password_entry), NULL);
    }

  /* if the user changes the password, it's probably no longer a fake
   * password */
  g_signal_connect (password_entry, "focus-in-event",
      G_CALLBACK (account_widget_build_skype_password_entry_focus), NULL);
}

gboolean
empathy_account_widget_skype_show_eula (GtkWindow *parent)
{
  GtkWidget *dialog, *textview, *vbox, *scrolledwindow;
  GtkTextBuffer *buffer;
  gchar *filename, *l10n_filename;
  const gchar * const * langs;
  GError *error = NULL;
  gchar *eula;
  gint result;
  gsize len;
  gint i;

  filename = empathy_file_lookup ("skype-eula.txt", "data");

  langs = g_get_language_names ();

  for (i = 0; langs[i] != NULL; i++)
    {
      l10n_filename = g_strconcat (filename, ".", langs[i], NULL);
      g_file_get_contents (l10n_filename, &eula, &len, NULL);
      g_free (l10n_filename);

      if (eula != NULL)
        break;
    }

  if (eula == NULL)
    {
      DEBUG ("Could not open translated Skype EULA");
      g_file_get_contents (filename, &eula, &len, &error);
    }

  g_free (filename);

  if (error != NULL)
    {
      g_warning ("Could not open Skype EULA: %s", error->message);
      g_error_free (error);
      return FALSE;
    }

  dialog = gtk_dialog_new_with_buttons (_("End User License Agreement"),
      parent, GTK_DIALOG_MODAL,
      _("Decline"), GTK_RESPONSE_CANCEL,
      _("Accept"), GTK_RESPONSE_ACCEPT,
      NULL);

  buffer = gtk_text_buffer_new (NULL);
  gtk_text_buffer_set_text (buffer, eula, len);
  g_free (eula);
  textview = gtk_text_view_new_with_buffer (buffer);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD_CHAR);

  vbox = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_add_with_viewport (
      GTK_SCROLLED_WINDOW (scrolledwindow), textview);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
      GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
  gtk_window_set_default_size (GTK_WINDOW (dialog), 400, 250);
  gtk_widget_show_all (dialog);

  result = gtk_dialog_run (GTK_DIALOG (dialog));

  gtk_widget_destroy (dialog);

  return (result == GTK_RESPONSE_ACCEPT);
}

