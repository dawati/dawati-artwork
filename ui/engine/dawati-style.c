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

#include "config.h"

#include "dawati-style.h"
#include "dawati-utils.h"
#include "dawati-rc-style.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


static int do_debug = 0;
static void print_widget_path (GtkWidget *widget);


#define DEBUG \
  if (do_debug == 1) \
    printf ("%s: detail = '%s'; state = %d; x:%d; y:%d; w:%d; h:%d;\n", __FUNCTION__, detail, state_type, x, y, width, height); \
  else if (do_debug == 2 && widget) print_widget_path (widget);

#define DETAIL(foo) (detail && strcmp (foo, detail) == 0)

#define LINE_WIDTH 1

G_DEFINE_DYNAMIC_TYPE (DawatiStyle, dawati_style,
                       GTK_TYPE_STYLE)

void
_dawati_style_register_type (GTypeModule *module)
{
  dawati_style_register_type (module);
}

static void
print_widget_path (GtkWidget *widget)
{
  static GtkWidget *previous = NULL;
  gchar *path;

  /* prevent printing lots of duplicates */
  if (previous == widget)
    return;
  previous = widget;

  gtk_widget_path (widget, NULL, &path, NULL);
  printf ("widget: %s\n", path);
  g_free (path);

  gtk_widget_class_path (widget, NULL, &path, NULL);
  printf ("widget_class: %s\n\n", path);
  g_free (path);
}

static cairo_t*
dawati_cairo_create (GdkWindow    *window,
                             GdkRectangle *area)
{
  cairo_t *cr;

  cr = gdk_cairo_create (window);

  if (area)
    {
      cairo_rectangle (cr, area->x, area->y, area->width, area->height);
      cairo_clip (cr);
    }

  return cr;
}

static inline void
dawati_set_border_color (cairo_t     *cr,
                                 GtkStyle    *style,
                                 GtkStateType state)
{
  if (DAWATI_STYLE (style)->border_color)
    gdk_cairo_set_source_color (cr,
                                &(DAWATI_STYLE (style)->border_color[
                                    state]));
}


static void
dawati_rounded_rectangle (cairo_t *cr,
                                  gdouble  x,
                                  gdouble  y,
                                  gdouble  width,
                                  gdouble  height,
                                  gdouble  radius)
{
  if (width < 1 || height < 1)
    return;

  if (radius == 0)
    {
      cairo_rectangle (cr, x, y, width, height);
      return;
    }

  if (width < radius * 2)
    {
      radius = width / 2;
    }
  else if (height < radius * 2)
    {
      radius = height / 2;
    }

  cairo_move_to (cr, x, y + height - radius);
  cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
  cairo_arc (cr, x + width - radius, y + radius, radius, M_PI * 1.5, 0);
  cairo_arc (cr, x + width - radius, y + height - radius, radius, 0,
             M_PI * 0.5);
  cairo_arc (cr, x + radius, y + height - radius, radius, M_PI * 0.5, M_PI);
}

static void
dawati_draw_grip (cairo_t *cr,
                          gboolean vertical,
                          gdouble  x,
                          gdouble  y,
                          gdouble  width,
                          gdouble  height)
{
  gdouble cx, cy;
  gint n_strips = 3;
  gint strip_w = 3;
  gint strip_h = 8 - strip_w;
  gint strip_padding = 2;
  gint grip_w = (n_strips * strip_w) + ((n_strips - 1) * strip_padding);
  gint i;

  cairo_save (cr);

  cairo_set_line_width (cr, strip_w);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  if (!vertical)
    {
      cx = x + (width / 2.0) - grip_w / 2.0 + strip_w / 2.0;
      cy = y + (height / 2.0) - (strip_h / 2.0);

      for (i = 0; i < n_strips; i++)
        {
          gdouble position;

          position = cx + (i * (strip_w + strip_padding));

          cairo_move_to (cr, position, cy);
          cairo_line_to (cr, position, cy + strip_h);

          cairo_stroke (cr);
        }
    }
  else
    {
      cy = y + (height / 2.0) - grip_w / 2.0 + strip_w / 2.0;
      cx = x + (width / 2.0) - (strip_h / 2.0);

      for (i = 0; i < n_strips; i++)
        {
          gdouble position;

          position = cy + (i * (strip_w + strip_padding));

          cairo_move_to (cr, cx, position);
          cairo_line_to (cr, cx + strip_h, position);

          cairo_stroke (cr);
        }
    }

  cairo_restore (cr);
}

static void
dawati_draw_box (GtkStyle     *style,
                         GdkWindow    *window,
                         GtkStateType  state_type,
                         GtkShadowType shadow_type,
                         GdkRectangle *area,
                         GtkWidget    *widget,
                         const gchar  *detail,
                         gint          x,
                         gint          y,
                         gint          width,
                         gint          height)
{
  cairo_t *cr;
  DawatiStyle *mb_style = DAWATI_STYLE (style);
  gint radius = mb_style->radius;

  DEBUG;

  if (DETAIL ("paned") || DETAIL ("buttondefault"))
    return;


  /* we want hover and focused widgets to look the same */
  if (widget && GTK_WIDGET_HAS_FOCUS (widget))
    state_type = GTK_STATE_PRELIGHT;


  /* scrollbar troughs are a plain rectangle */
  if (widget && GTK_IS_SCROLLBAR (widget) && DETAIL ("trough"))
    {
      cr = dawati_cairo_create (window, area);

      cairo_rectangle (cr, x, y, width, height);
      gdk_cairo_set_source_color (cr, &style->base[state_type]);
      cairo_fill (cr);

      cairo_destroy (cr);

      return;
    }

  SANITIZE_SIZE;

  /*** treeview headers ***/
  if (widget && GTK_IS_TREE_VIEW (widget->parent))
    {
      cr = dawati_cairo_create (window, area);

      cairo_rectangle (cr, x, y, width, height);
      gdk_cairo_set_source_color (cr, &style->bg[state_type]);
      cairo_fill (cr);
      cairo_destroy (cr);

      gtk_paint_vline (style, window, state_type, area, widget, detail,
                       y + 5, y + height - 5, x + width - 1);
      return;
    }

  /*** spin buttons ***/
  if (DETAIL ("spinbutton_down") || DETAIL ("spinbutton_up"))
    return;

  if (DETAIL ("spinbutton"))
    {
      /* FIXME: for RTL */
      width += 10;
      x -= 10;

      /* always draw button as shadow out to match the entry */
      shadow_type = GTK_SHADOW_OUT;

      /* draw the buttons with the same state type as the entry */
      state_type = widget->state;
    }

  /*** combo boxes ***/
  if (DETAIL ("button") && widget && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
    {
      GtkWidget *entry;

      entry = g_object_get_data (G_OBJECT (widget->parent),
                                 "dawati-combo-entry");
      g_object_set_data (G_OBJECT (widget->parent),
                         "dawati-combo-button", widget);

      /* always draw combo box entry buttons as shadow out to match the entry */
      shadow_type = GTK_SHADOW_OUT;

      if (GTK_IS_ENTRY (entry))
        {
          gtk_widget_queue_draw (entry);
        }

      /* FIXME: RTL */
      width += 10;
      x -= 10;
    }

  if (widget
      && (DETAIL ("trough")
          || DETAIL ("trough-fill-level")
          || DETAIL ("trough-fill-level-full"))
      && GTK_IS_SCALE (widget))
    {
      if (mb_style->shadow)
        {
          width--;
          height--;
        }
      if (width > height)
        {
          y = y + (height / 2.0 - 2);
          height = 4;
        }
      else
        {
          x = x + (width / 2.0 - 2);
          width = 4;
        }
    }


  /* scrollbar buttons */
  if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar"))
    {
      x += 2;
      y += 2;
      width -= 4;
      height -= 4;
    }

  /* scrollbar slider */
  if (DETAIL ("slider"))
    {
      if (widget && GTK_IS_HSCROLLBAR (widget))
        {
          y += 2;
          height -= 4;
        }
      else
        {
          x += 2;
          width -= 4;
        }
    }

  cr = dawati_cairo_create (window, area);

  cairo_set_line_width (cr, LINE_WIDTH);


  /* special "fill" indicator */
  if (DETAIL ("trough-fill-level-full")
      || DETAIL ("trough-fill-level"))
    {
      gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_SELECTED]);

      if (width > height)
        cairo_rectangle (cr, x, y + 1, width, height - 2);
      else
        cairo_rectangle (cr, x + 1, y, width - 2, height);

      cairo_fill (cr);
      cairo_destroy (cr);
      return;
    }

  if (mb_style->shadow)
    {
      if (shadow_type == GTK_SHADOW_OUT)
        {
          /* outer shadow */
          dawati_rounded_rectangle (cr, x, y, width, height,
                                            radius + 1);
          cairo_set_source_rgba (cr, 0, 0, 0, mb_style->shadow);
          cairo_fill (cr);

          /* reduce size for outer shadow */
          height--;
          width--;
        }
      else if (shadow_type == GTK_SHADOW_IN)
        {
          x++;
          y++;
          width--;
          height--;
        }
    }


  /* fill */
  dawati_rounded_rectangle (cr, x, y, width, height, radius);

  if (DETAIL ("light-switch-trough"))
    {
      cairo_pattern_t *crp;

      crp = cairo_pattern_create_linear (x, y, x, height);

      /* FIXME: these colours really should not be defined here */

      if (state_type == GTK_STATE_SELECTED)
        {
          cairo_pattern_add_color_stop_rgb (crp, 0, 0.365, 0.820, 0.953);
          cairo_pattern_add_color_stop_rgb (crp, 1, 0.627, 0.894, 0.973);
        }
      else
        {
          cairo_pattern_add_color_stop_rgb (crp, 0, 0.937, 0.941, 0.929);
          cairo_pattern_add_color_stop_rgb (crp, 1, 1.0, 1.0, 1.0);
        }

      cairo_set_source (cr, crp);
    }
  else
    gdk_cairo_set_source_color (cr, &style->bg[state_type]);

  cairo_fill (cr);

  /* extra hilight for "button" widgets, also used as focus rectangle since
   * state_type is set to prelight for focused widgets (see above) */
  if (state_type == GTK_STATE_PRELIGHT && DETAIL ("button")
      && !(widget && GTK_IS_COMBO_BOX_ENTRY (widget->parent)))
    {
      cairo_set_line_width (cr, 2.0);
      dawati_rounded_rectangle (cr, x + 2, y + 2, width - 4,
                                        height - 4, radius - 1);
      gdk_cairo_set_source_color (cr, &style->bg[GTK_STATE_SELECTED]);
      cairo_stroke (cr);
      cairo_set_line_width (cr, 1.0);
    }

  if (shadow_type != GTK_SHADOW_NONE)
    {
      /* border */
      dawati_rounded_rectangle (cr, x + 0.5, y + 0.5,
                                        width - 1, height - 1, radius);
      dawati_set_border_color (cr, style, state_type);
      cairo_stroke (cr);
    }

  /* add a grip to handles */
  if (DETAIL ("light-switch-handle") || DETAIL ("hscale"))
    {
      gdk_cairo_set_source_color (cr, &style->mid[state_type]);
      dawati_draw_grip (cr, FALSE, x, y, width, height);
    }
  else if (DETAIL ("vscale"))
    {
      gdk_cairo_set_source_color (cr, &style->mid[state_type]);
      dawati_draw_grip (cr, TRUE, x, y, width, height);
    }
  cairo_destroy (cr);

}

static void
dawati_draw_shadow (GtkStyle     *style,
                            GdkWindow    *window,
                            GtkStateType  state_type,
                            GtkShadowType shadow_type,
                            GdkRectangle *area,
                            GtkWidget    *widget,
                            const gchar  *detail,
                            gint          x,
                            gint          y,
                            gint          width,
                            gint          height)
{
  cairo_t *cr;
  DawatiStyle *mb_style = DAWATI_STYLE (style);
  gdouble radius = mb_style->radius;

  DEBUG;

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  SANITIZE_SIZE;

  cr = dawati_cairo_create (window, area);

  /* initilise the background in the corners to the colour of the widget */
  if (widget)
    {
      cairo_rectangle (cr, x, y, width, height);
      if (mb_style->shadow)
        dawati_rounded_rectangle (cr, x, y, width - 1, height - 1,
                                          radius);
      else
        dawati_rounded_rectangle (cr, x, y, width, height, radius);

      gdk_cairo_set_source_color (cr, &widget->style->bg[state_type]);
      cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
      cairo_fill (cr);
    }

  /* FIXME: for RTL */
  if (widget && DETAIL ("entry")
      && (GTK_IS_SPIN_BUTTON (widget)
          || GTK_IS_COMBO_BOX_ENTRY (widget->parent)))
    width += 10;

  if (widget && DETAIL ("entry") && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
    {
      GtkWidget *button;

      g_object_set_data (G_OBJECT (widget->parent),
                         "dawati-combo-entry", widget);

      button = g_object_get_data (G_OBJECT (widget->parent),
                                  "dawati-combo-button");


      if (button)
        {
          if (GTK_WIDGET_HAS_FOCUS (button))
            state_type = GTK_STATE_PRELIGHT;
          else
            /* try and keep the button and entry in the same state */
            gtk_widget_set_state (button, state_type);
        }


      width += 10;
    }

  cairo_translate (cr, 0.5, 0.5);
  width--;
  height--;

  cairo_set_line_width (cr, 1.0);
  if (mb_style->shadow != 0.0)
    {
      /* outer shadow */
      dawati_rounded_rectangle (cr, x, y, width, height,
                                        radius + 1.0);
      cairo_set_source_rgba (cr, 0, 0, 0, mb_style->shadow);
      cairo_stroke (cr);


      /* reduce size for outer shadow */
      height--;
      width--;
    }

  /* border */
  dawati_rounded_rectangle (cr, x, y, width, height, radius);
  dawati_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
dawati_draw_check (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GtkShadowType shadow_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          x,
                           gint          y,
                           gint          width,
                           gint          height)
{
  cairo_t *cr;
  gint radius = DAWATI_STYLE (style)->radius;

  DEBUG;

  cr = dawati_cairo_create (window, area);

  cairo_set_line_width (cr, 1.0);

  if (shadow_type == GTK_SHADOW_IN && state_type != GTK_STATE_INSENSITIVE)
    {
      state_type = GTK_STATE_SELECTED;
    }

  /* we don't support anything other than 15x15 */
  width = 15;
  height = 15;

  dawati_rounded_rectangle (cr, x + 0.5, y + 0.5,
                                    width - 1, height - 1, radius);

  /* fill the background */
  gdk_cairo_set_source_color (cr, &style->base[state_type]);
  cairo_fill_preserve (cr);

  /* draw the border */
  dawati_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  gdk_cairo_set_source_color (cr, &style->text[state_type]);

  /* draw a tick when checked */
  if (shadow_type == GTK_SHADOW_IN)
    {
      cairo_translate (cr, x, y);

      cairo_move_to (cr, 3, 6);
      cairo_line_to (cr, 6, 9);
      cairo_line_to (cr, 12, 3);
      cairo_line_to (cr, 12, 6);
      cairo_line_to (cr, 6, 12);
      cairo_line_to (cr, 3, 9);
      cairo_line_to (cr, 3, 6);

      cairo_fill (cr);
    }
  cairo_destroy (cr);

}


static void
dawati_draw_option (GtkStyle     *style,
                            GdkWindow    *window,
                            GtkStateType  state_type,
                            GtkShadowType shadow_type,
                            GdkRectangle *area,
                            GtkWidget    *widget,
                            const gchar  *detail,
                            gint          x,
                            gint          y,
                            gint          width,
                            gint          height)
{
  cairo_t *cr;
  gint cx, cy, radius;

  DEBUG;

  cr = dawati_cairo_create (window, area);

  cairo_set_line_width (cr, 1);
  cairo_translate (cr, 0.5, 0.5);
  width--;
  height--;

  if (shadow_type == GTK_SHADOW_IN && state_type != GTK_STATE_INSENSITIVE)
    {
      state_type = GTK_STATE_SELECTED;
    }

  /* define radius and centre coordinates */
  if (width % 2) width--;
  radius = width / 2;
  cx = x + radius;
  cy = y + radius;

  /* fill the background */
  gdk_cairo_set_source_color (cr, &style->base[state_type]);
  cairo_arc (cr, cx, cy, radius, 0, M_PI * 2);
  cairo_fill (cr);

  /* draw the border */
  cairo_arc (cr, cx, cy, radius, 0, M_PI * 2);
  dawati_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  /*** draw check mark ***/
  if (shadow_type == GTK_SHADOW_IN)
    {
      cairo_arc (cr, cx, cy, radius - 4,  0, M_PI * 2);
      gdk_cairo_set_source_color (cr, &style->text[state_type]);
      cairo_fill (cr);
    }
  cairo_destroy (cr);
}

static void
dawati_draw_box_gap (GtkStyle       *style,
                             GdkWindow      *window,
                             GtkStateType    state_type,
                             GtkShadowType   shadow_type,
                             GdkRectangle   *area,
                             GtkWidget      *widget,
                             const gchar    *detail,
                             gint            x,
                             gint            y,
                             gint            width,
                             gint            height,
                             GtkPositionType gap_side,
                             gint            gap_x,
                             gint            gap_width)
{
  cairo_t *cr;
  GdkRectangle rect = { 0, };

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  cr = dawati_cairo_create (window, area);

  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_translate (cr, 0.5, 0.5);

  dawati_set_border_color (cr, style, state_type);

  /* start off with a rectangle... */
  cairo_rectangle (cr, x, y, width -1, height -1);
  cairo_stroke (cr);
  cairo_destroy (cr);

  switch (gap_side)
    {
    case GTK_POS_TOP:
      rect.x = x + gap_x + LINE_WIDTH;
      rect.y = y;
      rect.width = gap_width - LINE_WIDTH * 2;
      rect.height = LINE_WIDTH;
      break;
    case GTK_POS_BOTTOM:
      rect.x = x + gap_x + LINE_WIDTH;
      rect.y = y + height - 2;
      rect.width = gap_width - LINE_WIDTH * 2;
      rect.height = 2;
      break;
    case GTK_POS_LEFT:
      rect.x = x;
      rect.y = y + gap_x + LINE_WIDTH;
      rect.width = 2;
      rect.height = gap_width - LINE_WIDTH * 2;
      break;
    case GTK_POS_RIGHT:
      rect.x = x + width - 2;
      rect.y = y + gap_x + LINE_WIDTH;
      rect.width = 2;
      rect.height = gap_width - LINE_WIDTH * 2;
      break;
    }

  /* and finally blank out the gap */
  gtk_style_apply_default_background (style, window, TRUE, state_type, area,
                                      rect.x, rect.y, rect.width,
                                      rect.height);


}


static void
dawati_draw_extension (GtkStyle       *style,
                               GdkWindow      *window,
                               GtkStateType    state_type,
                               GtkShadowType   shadow_type,
                               GdkRectangle   *area,
                               GtkWidget      *widget,
                               const gchar    *detail,
                               gint            x,
                               gint            y,
                               gint            width,
                               gint            height,
                               GtkPositionType gap_side)
{
  cairo_t *cr;
  cairo_pattern_t *pattern;
  gint radius = DAWATI_STYLE (style)->radius;

  /* initialise the background */
  gtk_style_apply_default_background (style, window, TRUE, state_type, area,
                                      x, y, width, height);

  cr = dawati_cairo_create (window, area);

  /* set up for line drawing */
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_translate (cr, 0.5, 0.5);

  /* reduce with and height since we are using them for co-ordinate values */
  width--; height--;

  /* tab border */
  switch (gap_side)
    {
    case GTK_POS_TOP:     /* bottom tab */
      cairo_move_to (cr, x, y);
      cairo_arc_negative (cr, x + radius, y + height - radius, radius, M_PI,
                          M_PI * 0.5);
      cairo_arc_negative (cr, x + width - radius, y + height - radius, radius,
                          M_PI * 0.5, 0);
      cairo_line_to (cr, x + width, y);
      break;
    case GTK_POS_BOTTOM: /* top tab */
      height++; /* add overlap */
      if (state_type == GTK_STATE_NORMAL)
        cairo_arc_negative (cr, x, y + height - radius, radius, M_PI / 2.0, 0);
      else
        cairo_move_to (cr, x + radius, y + height);
      cairo_arc (cr, x + radius * 2, y + radius, radius, M_PI, M_PI * 1.5);
      cairo_arc (cr, x + width - radius * 2, y + radius, radius, M_PI * 1.5, 0);
      if (state_type == GTK_STATE_NORMAL)
        cairo_arc_negative (cr, x + width, y + height - radius, radius, M_PI, M_PI / 2.0);
      else
        cairo_line_to (cr, x + width - radius, y + height);
      break;
    case GTK_POS_LEFT:  /* right tab */
      cairo_move_to (cr, x, y);
      cairo_arc (cr, x + width - radius, y + radius, radius, M_PI * 1.5, 0);
      cairo_arc (cr, x + width - radius, y + height - radius, radius, 0,
                 M_PI * 0.5);
      cairo_line_to (cr, x, y + height);
      break;
    case GTK_POS_RIGHT: /* left tab */
      cairo_move_to (cr, x + width, y);
      cairo_arc_negative (cr, x + radius, y + radius, radius, M_PI * 1.5, M_PI);
      cairo_arc_negative (cr, x + radius, y + height - radius, radius, M_PI,
                          M_PI * 0.5);
      cairo_line_to (cr, x + width, y + height);
      break;
    }

  if (state_type == GTK_STATE_NORMAL)
    {
      pattern = cairo_pattern_create_linear (x, y, x, y +height);
      cairo_pattern_add_color_stop_rgb (pattern, 0,
                                        0xf9/255.0, 0xf9/255.0,0xf9/255.0);
      cairo_pattern_add_color_stop_rgb (pattern, 1,
                                        0xe6/255.0, 0xe6/255.0,0xe6/255.0);
      cairo_set_source (cr, pattern);
      cairo_fill_preserve (cr);
      cairo_pattern_destroy (pattern);
    }

  dawati_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  cairo_destroy (cr);

}

static void
dawati_draw_vline (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          y1,
                           gint          y2,
                           gint          x)
{
  cairo_t *cr;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  if (DETAIL ("vseparator"))
    return;

  cr = gdk_cairo_create (window);

  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  cairo_move_to (cr, x + LINE_WIDTH / 2.0, y1);
  cairo_line_to (cr, x + LINE_WIDTH / 2.0, y2);

  dawati_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
dawati_draw_hline (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          x1,
                           gint          x2,
                           gint          y)
{
  cairo_t *cr;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  cr = gdk_cairo_create (window);

  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  dawati_set_border_color (cr, style, state_type);
  cairo_move_to (cr, x1, y + LINE_WIDTH / 2.0);
  cairo_line_to (cr, x2, y + LINE_WIDTH / 2.0);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
dawati_draw_focus (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          x,
                           gint          y,
                           gint          width,
                           gint          height)
{
  cairo_t *cr;
  gint line_width;
  DawatiStyle *mb_style = DAWATI_STYLE (style);

  /* button draws it's own focus */
  if (DETAIL ("button"))
      return;

  cr = gdk_cairo_create (window);

  if (widget)
    gtk_widget_style_get (widget, "focus-line-width", &line_width, NULL);
  else
    line_width = 1;

  cairo_translate (cr, line_width / 2.0, line_width / 2.0);
  width -= line_width;
  height -= line_width;

  if (mb_style->shadow)
    {
      width -= 1;
      height -= 1;
    }

  dawati_rounded_rectangle (cr, x, y, width, height, line_width);
  cairo_set_line_width (cr, line_width);
  gdk_cairo_set_source_color (cr, &style->bg[GTK_STATE_SELECTED]);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
dawati_draw_arrow (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GtkShadowType shadow_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           GtkArrowType  arrow_type,
                           gboolean      fill,
                           gint          x,
                           gint          y,
                           gint          width,
                           gint          height)
{
  cairo_t *cr;

  DEBUG;

  cr = dawati_cairo_create (window, area);

  cairo_set_line_width (cr, 2);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  gdk_cairo_set_source_color (cr, &style->fg[state_type]);

  /* add padding around scrollbar buttons */
  if (DETAIL ("vscrollbar") || DETAIL ("hscrollbar"))
    {
      x += 3;
      width -= 4;
      y += 3;
      height -= 4;
    }

  /* ensure we have odd number of pixels for width or height to allow for
   * correct centering
   */
  if (width % 2) width--;
  if (height % 2) height--;


  switch (arrow_type)
    {
    case GTK_ARROW_UP:
      y = y + height / 2 - (width * 0.12) - 1;
      height = width * 0.6;
      cairo_move_to (cr, x, y + height);
      cairo_line_to (cr, x + width / 2, y);
      cairo_line_to (cr, x + width, y + height);
      break;
    case GTK_ARROW_DOWN:
      y = y + height / 2 - (width * 0.12);
      height = width * 0.6;
      cairo_move_to (cr, x, y);
      cairo_line_to (cr, x + width / 2, y + height);
      cairo_line_to (cr, x + width, y);
      break;
    case GTK_ARROW_LEFT:
      x = x + width / 2 - (height * 0.12) - 1;
      width = height * 0.6;
      cairo_move_to (cr, x + width, y);
      cairo_line_to (cr, x, y + height / 2);
      cairo_line_to (cr, x + width, y + height);
      break;
    case GTK_ARROW_RIGHT:
      x = x + width / 2 - (height * 0.12);
      width = height * 0.6;
      cairo_move_to (cr, x, y);
      cairo_line_to (cr, x + width, y + height / 2);
      cairo_line_to (cr, x, y + height);
      break;
    case GTK_ARROW_NONE:
      break;
    }
  cairo_stroke (cr);
  cairo_destroy (cr);
}

static void
dawati_draw_handle (GtkStyle           *style,
                            GdkWindow          *window,
                            GtkStateType        state_type,
                            GtkShadowType       shadow_type,
                            GdkRectangle       *area,
                            GtkWidget          *widget,
                            const gchar        *detail,
                            gint                x,
                            gint                y,
                            gint                width,
                            gint                height,
                            GtkOrientation      orientation)
{
  cairo_t *cr;
  gdouble cx, cy, radius;

  DEBUG;

  cr = dawati_cairo_create (window, area);

  cx = x + width / 2;
  cy = y + height / 2;

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      radius = height / 2 - 2;

      cairo_arc (cr, cx, cy, radius, 0, M_PI * 360);
      cairo_arc (cr, cx - radius * 3, cy, radius, 0, M_PI * 360);
      cairo_arc (cr, cx + radius * 3, cy, radius, 0, M_PI * 360);
    }
  else
    {
      radius = width / 2 - 2;

      cairo_arc (cr, cx, cy, radius, 0, M_PI * 360);
      cairo_arc (cr, cx, cy - radius * 3, radius, 0, M_PI * 360);
      cairo_arc (cr, cx, cy + radius * 3, radius, 0, M_PI * 360);
    }

  dawati_set_border_color (cr, style, state_type);
  cairo_fill (cr);
  cairo_destroy (cr);
}

static void
dawati_draw_resize_grip (GtkStyle           *style,
                                 GdkWindow          *window,
                                 GtkStateType        state_type,
                                 GdkRectangle       *area,
                                 GtkWidget          *widget,
                                 const gchar        *detail,
                                 GdkWindowEdge       edge,
                                 gint                x,
                                 gint                y,
                                 gint                width,
                                 gint                height)
{
  cairo_t *cr;
  gdouble cx, cy, radius, spacing;

  DEBUG;

  /* FIXME: fix other window edge types */

  cr = dawati_cairo_create (window, area);

  radius = 2;

  /* bottom right */
  cx = x + width - radius;
  cy = y + height - radius;

  spacing = radius * 2 + 2;
  gdk_cairo_set_source_color (cr, &style->bg[GTK_STATE_ACTIVE]);

  cairo_arc (cr, cx, cy - spacing * 2, radius, 0, M_PI * 360);
  cairo_fill (cr);

  cairo_arc (cr, cx, cy - spacing, radius, 0, M_PI * 360);
  cairo_fill (cr);

  cairo_arc (cr, cx, cy, radius, 0, M_PI * 360);
  cairo_fill (cr);

  cairo_arc (cr, cx - spacing, cy - spacing, radius, 0, M_PI * 360);
  cairo_fill (cr);

  cairo_arc (cr, cx - spacing, cy, radius, 0, M_PI * 360);
  cairo_fill (cr);

  cairo_arc (cr, cx - spacing * 2, cy, radius, 0, M_PI * 360);
  cairo_fill (cr);

  cairo_destroy (cr);
}

/* this function is copied from the mist gtk engine */
static void
dawati_draw_layout (GtkStyle        *style,
                            GdkWindow       *window,
                            GtkStateType     state_type,
                            gboolean         use_text,
                            GdkRectangle    *area,
                            GtkWidget       *widget,
                            const char      *detail,
                            int              x,
                            int              y,
                            PangoLayout      *layout)
{
  GdkGC *gc;

  gc = use_text ? style->text_gc[state_type] : style->fg_gc[state_type];

  if (area)
    {
      gdk_gc_set_clip_rectangle (gc, area);
    }

  if (DETAIL ("accellabel") && state_type == GTK_STATE_NORMAL)
    {
      cairo_t *cr;

      cr = dawati_cairo_create (window, area);

      cairo_set_source_rgba (cr,
                             style->fg[state_type].red / 65535.0,
                             style->fg[state_type].green / 65535.0,
                             style->fg[state_type].blue / 65535.0,
                             0.5);
      cairo_move_to (cr, x, y);
      pango_cairo_show_layout (cr, layout);
      cairo_stroke (cr);

      cairo_destroy (cr);

    }
  else
    gdk_draw_layout (window, gc, x, y, layout);

  if (area)
    {
      gdk_gc_set_clip_rectangle (gc, NULL);
    }
}

/* this function is copied from the mist gtk engine */
static GdkPixbuf *
set_transparency (const GdkPixbuf *pixbuf, gdouble alpha_percent)
{
  GdkPixbuf *target;
  guchar *data, *current;
  guint x, y, rowstride, height, width;

  g_return_val_if_fail (pixbuf != NULL, NULL);
  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

  /* Returns a copy of pixbuf with it's non-completely-transparent pixels to
     have an alpha level "alpha_percent" of their original value. */

  target = gdk_pixbuf_add_alpha (pixbuf, FALSE, 0, 0, 0);

  if (alpha_percent == 1.0)
    return target;
  width = gdk_pixbuf_get_width (target);
  height = gdk_pixbuf_get_height (target);
  rowstride = gdk_pixbuf_get_rowstride (target);
  data = gdk_pixbuf_get_pixels (target);

  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          /* The "4" is the number of chars per pixel, in this case, RGBA,
             the 3 means "skip to the alpha" */
          current = data + (y * rowstride) + (x * 4) + 3;
          *(current) = (guchar) (*(current) * alpha_percent);
        }
    }

  return target;
}

/* this function is copied from the mist gtk engine */
static GdkPixbuf*
scale_or_ref (GdkPixbuf *src,
              int width,
              int height)
{
  if (width == gdk_pixbuf_get_width (src) &&
      height == gdk_pixbuf_get_height (src))
    {
      return g_object_ref (src);
    }
  else
    {
      return gdk_pixbuf_scale_simple (src,
                                      width, height,
                                      GDK_INTERP_BILINEAR);
    }
}

/* this function is copied from the mist gtk engine */
static GdkPixbuf *
dawati_render_icon (GtkStyle            *style,
                            const GtkIconSource *source,
                            GtkTextDirection     direction,
                            GtkStateType         state,
                            GtkIconSize          size,
                            GtkWidget           *widget,
                            const char          *detail)
{
  int width = 1;
  int height = 1;
  GdkPixbuf *scaled;
  GdkPixbuf *stated;
  GdkPixbuf *base_pixbuf;
  GdkScreen *screen;
  GtkSettings *settings;

  /* Oddly, style can be NULL in this function, because
   * GtkIconSet can be used without a style and if so
   * it uses this function.
   */

  base_pixbuf = gtk_icon_source_get_pixbuf (source);

  g_return_val_if_fail (base_pixbuf != NULL, NULL);

  if (widget && gtk_widget_has_screen (widget))
    {
      screen = gtk_widget_get_screen (widget);
      settings = gtk_settings_get_for_screen (screen);
    }
  else if (style->colormap)
    {
      screen = gdk_colormap_get_screen (style->colormap);
      settings = gtk_settings_get_for_screen (screen);
    }
  else
    {
      settings = gtk_settings_get_default ();
      GTK_NOTE (MULTIHEAD,
                g_warning ("Using the default screen for gtk_default_render_icon()"));
    }


  if (size != (GtkIconSize) -1
      && !gtk_icon_size_lookup_for_settings (settings, size, &width, &height))
    {
      g_warning (G_STRLOC ": invalid icon size '%d'", size);
      return NULL;
    }

  /* If the size was wildcarded, and we're allowed to scale, then scale;
   * otherwise, leave it alone.
   */
  if (size != (GtkIconSize)-1 && gtk_icon_source_get_size_wildcarded (source))
    scaled = scale_or_ref (base_pixbuf, width, height);
  else
    scaled = g_object_ref (base_pixbuf);

  /* If the state was wildcarded, then generate a state. */
  if (gtk_icon_source_get_state_wildcarded (source))
    {
      if (state == GTK_STATE_INSENSITIVE)
        {
          stated = set_transparency (scaled, 0.3);
          gdk_pixbuf_saturate_and_pixelate (stated, stated,
                                            0.1, FALSE);

          g_object_unref (scaled);
        }
      else if (state == GTK_STATE_PRELIGHT)
        {
          stated = gdk_pixbuf_copy (scaled);

          gdk_pixbuf_saturate_and_pixelate (scaled, stated,
                                            1.2, FALSE);

          g_object_unref (scaled);
        }
      else
        {
          stated = scaled;
        }
    }
  else
    stated = scaled;

  return stated;
}

static void
dawati_draw_expander (GtkStyle         *style,
                              GdkWindow        *window,
                              GtkStateType      state_type,
                              GdkRectangle     *area,
                              GtkWidget        *widget,
                              const gchar      *detail,
                              gint              x,
                              gint              y,
                              GtkExpanderStyle  expander_style)
{
  cairo_t *cr;

  cr = dawati_cairo_create (window, area);

  x -= 6;
  y -= 6;


  cairo_set_source_rgb (cr, 0.6, 0.6, 0.6);
  cairo_set_line_width (cr, 1.0);
  dawati_rounded_rectangle (cr, x, y, 12, 12, 2 );
  cairo_stroke_preserve (cr);

  if (state_type == GTK_STATE_PRELIGHT || state_type == GTK_STATE_ACTIVE)
    cairo_set_source_rgba (cr, 0, 0.6, 0.8, 1);
  else
    cairo_set_source_rgba (cr, 1, 1, 1, 1);

  cairo_fill (cr);

  if (state_type == GTK_STATE_PRELIGHT || state_type == GTK_STATE_ACTIVE)
    cairo_set_source_rgba (cr, 1, 1, 1, 1);
  else
    cairo_set_source_rgba (cr, 0, 0.6, 0.8, 1);

  cairo_set_line_width (cr, 2.0);
  cairo_move_to (cr, x + 2, y + 6);
  cairo_line_to (cr, x + 10, y + 6);
  cairo_stroke (cr);

  if (expander_style != GTK_EXPANDER_EXPANDED)
    {
      if (expander_style == GTK_EXPANDER_SEMI_COLLAPSED
          || expander_style == GTK_EXPANDER_SEMI_EXPANDED)
        {
          if (state_type == GTK_STATE_PRELIGHT
              || state_type == GTK_STATE_ACTIVE)
            cairo_set_source_rgba (cr, 1, 1, 1, 0.5);
          else
            cairo_set_source_rgba (cr, 0, 0.6, 0.8, 0.5);
        }

      cairo_move_to (cr, x + 6, y + 2);
      cairo_line_to (cr, x + 6, y + 10);
      cairo_stroke (cr);
    }

  cairo_destroy (cr);
}


static void
dawati_init_from_rc (GtkStyle   *style,
                             GtkRcStyle *rc_style)
{
  int i;
  DawatiStyle *mb_style;
  DawatiRcStyle *mb_rc_style;

  GTK_STYLE_CLASS (dawati_style_parent_class)->init_from_rc (style,
                                                                     rc_style);

  mb_rc_style = DAWATI_RC_STYLE (rc_style);
  mb_style = DAWATI_STYLE (style);

  mb_style->radius = mb_rc_style->radius;

  for (i = 0; i < 5; i++)
    mb_style->border_color[i] = mb_rc_style->border_color[i];

  mb_style->shadow = mb_rc_style->shadow;
}

static void
dawati_style_copy (GtkStyle *dest,
                           GtkStyle *src)
{
  int i;
  DawatiStyle *mb_dest = DAWATI_STYLE (dest);
  DawatiStyle *mb_src = DAWATI_STYLE (src);

  mb_dest->radius = mb_src->radius;

  for (i = 0; i < 5; i++)
    mb_dest->border_color[i] = mb_src->border_color[i];

  mb_dest->shadow = mb_src->shadow;

  GTK_STYLE_CLASS (dawati_style_parent_class)->copy (dest, src);
}

static void
dawati_style_class_init (DawatiStyleClass *klass)
{
  GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);
  const gchar *debug;

  /* Set debugging if required. We only need to do this once per instance, so
   * it is safe to do in the class-init */
  debug = getenv ("DAWATI_ENGINE_DEBUG");
  if (debug)
    do_debug = atoi (debug);


  style_class->init_from_rc = dawati_init_from_rc;
  style_class->copy = dawati_style_copy;

  style_class->draw_shadow = dawati_draw_shadow;
  style_class->draw_box = dawati_draw_box;
  style_class->draw_check = dawati_draw_check;
  style_class->draw_option = dawati_draw_option;
  style_class->draw_box_gap = dawati_draw_box_gap;
  style_class->draw_shadow_gap = dawati_draw_box_gap;
  style_class->draw_extension = dawati_draw_extension;
  style_class->draw_hline = dawati_draw_hline;
  style_class->draw_vline = dawati_draw_vline;
  style_class->draw_focus = dawati_draw_focus;
  style_class->draw_arrow = dawati_draw_arrow;
  style_class->draw_handle = dawati_draw_handle;
  style_class->draw_resize_grip = dawati_draw_resize_grip;
  style_class->draw_layout = dawati_draw_layout;
  style_class->render_icon = dawati_render_icon;
  style_class->draw_expander = dawati_draw_expander;
}

static void
dawati_style_class_finalize (DawatiStyleClass *klass)
{
}

static void
dawati_style_init (DawatiStyle *style)
{
}
