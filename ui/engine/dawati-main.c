/*
 * dawati-gtk-engine - A GTK+ theme engine for Dawati
 *
 * Copyright (c) 2008, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <gtk/gtk.h>
#include <gmodule.h>
#include <stdio.h>

#include "dawati-style.h"
#include "dawati-rc-style.h"



G_MODULE_EXPORT void
theme_init (GTypeModule *module)
{
  _dawati_rc_style_register_type (module);
  _dawati_style_register_type (module);
}

G_MODULE_EXPORT void
theme_exit(void)
{
}

G_MODULE_EXPORT GtkRcStyle *
theme_create_rc_style (void)
{
  return GTK_RC_STYLE (g_object_new (DAWATI_TYPE_RC_STYLE, NULL));
}
