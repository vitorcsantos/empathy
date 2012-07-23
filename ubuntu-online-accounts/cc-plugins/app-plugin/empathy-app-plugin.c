/*
 * empathy-app-plugin.c
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

#include "empathy-app-plugin.h"


G_DEFINE_TYPE (EmpathyAppPlugin, empathy_app_plugin, AP_TYPE_APPLICATION_PLUGIN)

static GtkWidget *
empathy_app_plugin_build_widget (ApApplicationPlugin *plugin)
{
  /* TODO */
  return NULL;
}

static void
empathy_app_plugin_class_init (EmpathyAppPluginClass *klass)
{
  ApApplicationPluginClass *app_class = AP_APPLICATION_PLUGIN_CLASS (klass);

  app_class->build_widget = empathy_app_plugin_build_widget;
}

static void
empathy_app_plugin_init (EmpathyAppPlugin *self)
{
}

GType
ap_module_get_object_type (void)
{
  return EMPATHY_TYPE_APP_PLUGIN;
}
