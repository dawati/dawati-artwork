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

#ifndef DAWATI_RC_STYLE_H
#define DAWATI_RC_STYLE_H

#include <gtk/gtk.h>
#include <gmodule.h>

G_BEGIN_DECLS

#define DAWATI_TYPE_RC_STYLE                    \
  dawati_rc_style_get_type()
#define DAWATI_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST (( \
                                                                        object), \
                                                                       DAWATI_TYPE_RC_STYLE, \
                                                                       DawatiRcStyle))
#define DAWATI_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((   \
                                                                     klass), \
                                                                    DAWATI_TYPE_RC_STYLE, \
                                                                    DawatiRcStyleClass))
#define DAWATI_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE (( \
                                                                        object), \
                                                                       DAWATI_TYPE_RC_STYLE))
#define DAWATI_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((   \
                                                                     klass), \
                                                                    DAWATI_TYPE_RC_STYLE))
#define DAWATI_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS (( \
                                                                       obj), \
                                                                      DAWATI_TYPE_RC_STYLE, \
                                                                      DawatiRcStyleClass))

typedef struct _DawatiRcStyle DawatiRcStyle;
typedef struct _DawatiRcStyleClass DawatiRcStyleClass;

struct _DawatiRcStyle
{
  GtkRcStyle parent_instance;

  /* engine options */
  gint radius;
  GdkColor border_color[5];
  gdouble shadow;

  /* flags for merge */
  gboolean radius_set : 1;
  gboolean shadow_set : 1;
  gboolean border_color_set[5];
};

struct _DawatiRcStyleClass
{
  GtkRcStyleClass parent_class;
};

GType dawati_rc_style_get_type (void);
void _dawati_rc_style_register_type (GTypeModule *engine);

G_END_DECLS

#endif
