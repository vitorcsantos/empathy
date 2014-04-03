/*
 * Copyright (C) 2009 Collabora Ltd.
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
 * Authors: Cosimo Alfarano <cosimo.alfarano@collabora.co.uk>
 */

#ifndef __TPL_ACTION_CHAIN_H__
#define __TPL_ACTION_CHAIN_H__

#include <glib-object.h>
#include <gio/gio.h>

/* Avoid accidentally using the symbols exported by the telepathy-logger
 * library (GNOME#727162) */
#define _tpl_action_chain_new_async   _empathy_action_chain_new_async
#define _tpl_action_chain_free        _empathy_action_chain_free
#define _tpl_action_chain_append      _empathy_action_chain_append
#define _tpl_action_chain_prepend     _empathy_action_chain_prepend
#define _tpl_action_chain_start       _empathy_action_chain_start
#define _tpl_action_chain_continue    _empathy_action_chain_continue
#define _tpl_action_chain_terminate   _empathy_action_chain_terminate
#define _tpl_action_chain_clear       _empathy_action_chain_clear
#define _tpl_action_chain_get_object  _empathy_action_chain_get_object
#define _tpl_action_chain_new_finish  _empathy_action_chain_new_finish

typedef struct {
    GQueue *chain;
    GSimpleAsyncResult *simple;
    gboolean running;
} TplActionChain;

TplActionChain *_tpl_action_chain_new_async (GObject *obj,
    GAsyncReadyCallback cb,
    gpointer user_data);
void _tpl_action_chain_free (TplActionChain *self);
typedef void (*TplPendingAction) (TplActionChain *ctx, gpointer user_data);
void _tpl_action_chain_append (TplActionChain *self, TplPendingAction func,
    gpointer user_data);
void _tpl_action_chain_prepend (TplActionChain *self, TplPendingAction func,
    gpointer user_data);
void _tpl_action_chain_start (TplActionChain *self);
void _tpl_action_chain_continue (TplActionChain *self);
void _tpl_action_chain_terminate (TplActionChain *self, const GError *error);
void _tpl_action_chain_clear (TplActionChain *self);

gpointer _tpl_action_chain_get_object (TplActionChain *self);
gboolean _tpl_action_chain_new_finish (GObject *source,
    GAsyncResult *result, GError **error);

#endif // __TPL_ACTION_CHAIN_H__
