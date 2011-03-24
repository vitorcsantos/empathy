/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2007-2010 Collabora Ltd.
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
 *
 * Authors: Xavier Claessens <xclaesse@gmail.com>
 */

#include <config.h>

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <glib/gi18n-lib.h>

#include <telepathy-glib/util.h>
#include <folks/folks.h>
#include <folks/folks-telepathy.h>

#include <libempathy/empathy-individual-manager.h>
#include <libempathy/empathy-utils.h>

#include "empathy-individual-dialogs.h"
#include "empathy-contact-widget.h"
#include "empathy-ui-utils.h"

#define BULLET_POINT "\342\200\242"

static GtkWidget *new_individual_dialog = NULL;

/*
 *  New contact dialog
 */

static void
can_add_contact_to_account (TpAccount *account,
    EmpathyAccountChooserFilterResultCallback callback,
    gpointer callback_data,
    gpointer user_data)
{
  EmpathyIndividualManager *individual_manager;
  TpConnection *connection;
  gboolean result;

  connection = tp_account_get_connection (account);
  if (connection == NULL)
    {
      callback (FALSE, callback_data);
      return;
    }

  individual_manager = empathy_individual_manager_dup_singleton ();
  result = empathy_individual_manager_get_flags_for_connection (
    individual_manager, connection) & EMPATHY_INDIVIDUAL_MANAGER_CAN_ADD;
  g_object_unref (individual_manager);

  callback (result, callback_data);
}

static void
new_individual_response_cb (GtkDialog *dialog,
    gint response,
    GtkWidget *contact_widget)
{
  EmpathyIndividualManager *individual_manager;
  EmpathyContact *contact;

  individual_manager = empathy_individual_manager_dup_singleton ();
  contact = empathy_contact_widget_get_contact (contact_widget);

  if (contact && response == GTK_RESPONSE_OK)
    empathy_individual_manager_add_from_contact (individual_manager, contact);

  new_individual_dialog = NULL;
  gtk_widget_destroy (GTK_WIDGET (dialog));
  g_object_unref (individual_manager);
}

void
empathy_new_individual_dialog_show (GtkWindow *parent)
{
  empathy_new_individual_dialog_show_with_individual (parent, NULL);
}

void
empathy_new_individual_dialog_show_with_individual (GtkWindow *parent,
    FolksIndividual *individual)
{
  GtkWidget *dialog;
  GtkWidget *button;
  EmpathyContact *contact = NULL;
  GtkWidget *contact_widget;

  g_return_if_fail (individual == NULL || FOLKS_IS_INDIVIDUAL (individual));

  if (new_individual_dialog)
    {
      gtk_window_present (GTK_WINDOW (new_individual_dialog));
      return;
    }

  /* Create dialog */
  dialog = gtk_dialog_new ();
  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
  gtk_window_set_title (GTK_WINDOW (dialog), _("New Contact"));

  /* Cancel button */
  button = gtk_button_new_with_label (GTK_STOCK_CANCEL);
  gtk_button_set_use_stock (GTK_BUTTON (button), TRUE);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button,
      GTK_RESPONSE_CANCEL);
  gtk_widget_show (button);

  /* Add button */
  button = gtk_button_new_with_label (GTK_STOCK_ADD);
  gtk_button_set_use_stock (GTK_BUTTON (button), TRUE);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button, GTK_RESPONSE_OK);
  gtk_widget_show (button);

  /* Contact info widget */
  if (individual != NULL)
    contact = empathy_contact_dup_from_folks_individual (individual);

  contact_widget = empathy_contact_widget_new (contact,
      EMPATHY_CONTACT_WIDGET_EDIT_ALIAS |
      EMPATHY_CONTACT_WIDGET_EDIT_ACCOUNT |
      EMPATHY_CONTACT_WIDGET_EDIT_ID |
      EMPATHY_CONTACT_WIDGET_EDIT_GROUPS);
  gtk_container_set_border_width (GTK_CONTAINER (contact_widget), 8);
  gtk_box_pack_start (
      GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))),
      contact_widget, TRUE, TRUE, 0);
  empathy_contact_widget_set_account_filter (contact_widget,
      can_add_contact_to_account, NULL);
  gtk_widget_show (contact_widget);

  new_individual_dialog = dialog;

  g_signal_connect (dialog, "response", G_CALLBACK (new_individual_response_cb),
      contact_widget);

  if (parent != NULL)
    gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

  gtk_widget_show (dialog);

  tp_clear_object (&contact);
}

static char *
build_account_list (GHashTable *set)
{
  GHashTableIter iter;
  const char *key;
  const char **accounts;
  char *str;
  guint i;

  accounts = g_malloc0 (sizeof (char *) * (g_hash_table_size (set) + 1));

  g_hash_table_iter_init (&iter, set);
  i = 0;
  while (g_hash_table_iter_next (&iter, (gpointer) &key, NULL))
    accounts[i++] = key;

  str = g_strjoinv (", ", (char **) accounts);

  g_free (accounts);

  return str;
}

/*
 * Block contact dialog
 */
gboolean
empathy_block_individual_dialog_show (GtkWindow *parent,
    FolksIndividual *individual,
    GdkPixbuf *avatar,
    gboolean *abusive)
{
  EmpathyIndividualManager *manager =
    empathy_individual_manager_dup_singleton ();
  GtkWidget *dialog;
  GtkWidget *abusive_check = NULL;
  GList *personas, *l;
  GString *text = g_string_new ("");
  GHashTable *blocked = g_hash_table_new (g_str_hash, g_str_equal);
  GHashTable *not_blocked = g_hash_table_new (g_str_hash, g_str_equal);
  gboolean can_report_abuse = FALSE;
  int res;

  dialog = gtk_message_dialog_new (parent,
      GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
      _("Block %s?"),
      folks_alias_details_get_alias (FOLKS_ALIAS_DETAILS (individual)));

  if (avatar != NULL)
    {
      GtkWidget *image = gtk_image_new_from_pixbuf (avatar);
      gtk_message_dialog_set_image (GTK_MESSAGE_DIALOG (dialog), image);
      gtk_widget_show (image);
    }

  /* build a list of personas that support blocking */
  personas = folks_individual_get_personas (individual);

  for (l = personas; l != NULL; l = l->next)
    {
      TpfPersona *persona = l->data;
      EmpathyContact *contact;
      EmpathyIndividualManagerFlags flags;
      GHashTable *set;

      if (!empathy_folks_persona_is_interesting (FOLKS_PERSONA (persona)))
          continue;

      contact = empathy_contact_dup_from_tp_contact (
          tpf_persona_get_contact (persona));
      flags = empathy_individual_manager_get_flags_for_connection (manager,
          empathy_contact_get_connection (contact));

      if (flags & EMPATHY_INDIVIDUAL_MANAGER_CAN_BLOCK)
        set = blocked;
      else
        set = not_blocked;

      if (flags & EMPATHY_INDIVIDUAL_MANAGER_CAN_REPORT_ABUSIVE)
        can_report_abuse = TRUE;

      g_hash_table_insert (set, (gpointer)
          tp_account_get_display_name (empathy_contact_get_account (contact)),
          NULL);
      g_object_unref (contact);
    }

  g_string_append_printf (text,
      _("Are you sure you want to block '%s' from contacting you again?"),
      folks_alias_details_get_alias (FOLKS_ALIAS_DETAILS (individual)));

  /* if there are any accounts the contact can not be blocked from */
  if (g_hash_table_size (not_blocked) > 0)
    {
      char *blocked_str = build_account_list (blocked);
      char *not_blocked_str = build_account_list (not_blocked);
      int nblocked = g_hash_table_size (blocked);

      g_string_append (text, "\n\n");
      g_string_append_printf (text,
          ngettext (
            "Blocking will only block calls and chats on the following"
            " account: %s. It will not block calls and chats on %s.",
            "Blocking will only block calls and chats on the following"
            " accounts: %s. It will not block calls and chats on %s.",
            nblocked),
            blocked_str, not_blocked_str);

      g_free (blocked_str);
      g_free (not_blocked_str);
    }

  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
    "%s", text->str);

  gtk_dialog_add_buttons (GTK_DIALOG (dialog),
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      _("_Block"), GTK_RESPONSE_REJECT,
      NULL);

  if (can_report_abuse)
    {
      GtkWidget *vbox;

      vbox = gtk_message_dialog_get_message_area (GTK_MESSAGE_DIALOG (dialog));
      abusive_check = gtk_check_button_new_with_mnemonic (
          _("_Report this contact as abusive"));

      gtk_box_pack_start (GTK_BOX (vbox), abusive_check, FALSE, TRUE, 0);
      gtk_widget_show (abusive_check);
    }

  g_object_unref (manager);
  g_string_free (text, TRUE);
  g_hash_table_destroy (blocked);
  g_hash_table_destroy (not_blocked);

  res = gtk_dialog_run (GTK_DIALOG (dialog));

  if (abusive != NULL)
    {
      if (abusive_check != NULL)
        *abusive = gtk_toggle_button_get_active (
            GTK_TOGGLE_BUTTON (abusive_check));
      else
        *abusive = FALSE;
    }

  gtk_widget_destroy (dialog);

  return res == GTK_RESPONSE_REJECT;
}
