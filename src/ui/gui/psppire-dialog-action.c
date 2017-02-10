/* PSPPIRE - a graphical user interface for PSPP.
   Copyright (C) 2012, 2016  Free Software Foundation

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */


#include <config.h>

#include "psppire-dialog-action.h"
#include "psppire-dialog.h"
#include "executor.h"
#include "helper.h"
#include "psppire-data-window.h"

static void psppire_dialog_action_init            (PsppireDialogAction      *act);
static void psppire_dialog_action_class_init      (PsppireDialogActionClass *class);

G_DEFINE_ABSTRACT_TYPE (PsppireDialogAction, psppire_dialog_action, GTK_TYPE_ACTION);

/* Properties */
enum
{
  PROP_0,
  PROP_MANAGER,
  PROP_TOPLEVEL,
};

static void
psppire_dialog_action_set_property (GObject         *object,
			       guint            prop_id,
			       const GValue    *value,
			       GParamSpec      *pspec)
{
  PsppireDialogAction *act = PSPPIRE_DIALOG_ACTION (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      {

	GObject *p = g_value_get_object (value);
	act->uim = GTK_UI_MANAGER (p);
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    };
}


static void
psppire_dialog_action_get_property (GObject    *object,
			       guint            prop_id,
			       GValue          *value,
			       GParamSpec      *pspec)
{
  PsppireDialogAction *dialog_action = PSPPIRE_DIALOG_ACTION (object);

  switch (prop_id)
    {
    case PROP_MANAGER:
      g_value_take_object (value, dialog_action->uim);
      break;
  case PROP_TOPLEVEL:
      g_value_take_object (value, dialog_action->toplevel);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    };
}


static void
set_toplevel (PsppireDialogAction *act)
{
  if (act->toplevel)
    return;

  GSList *sl = gtk_ui_manager_get_toplevels (act->uim, GTK_UI_MANAGER_MENUBAR | GTK_UI_MANAGER_TOOLBAR);
  g_return_if_fail (sl);
  
  act->toplevel = gtk_widget_get_toplevel (GTK_WIDGET (sl->data));
  g_slist_free (sl);
}

static void
on_destroy_dataset (GObject *w)
{
  GHashTable *t = g_object_get_data (w, "thing-table");
  GSList *dl = g_object_get_data (w, "widget-list");
  
  g_slist_free_full (dl, (GDestroyNotify) gtk_widget_destroy);
  g_hash_table_unref (t);
}

/* Each toplevel widget - that is the data window, which generally has a 1-1 association
   with a dataset - has an associated GHashTable.
   
   This GHashTable is keyed by the address of a PsppireDialogAction, and its values
   are user determined pointers (typically a GtkBuilder*).

   This is useful for storing the state of dialogs so they can persist between invocations.
*/
GHashTable *
psppire_dialog_action_get_hash_table (PsppireDialogAction *act)
{
  set_toplevel (act);
  
  GHashTable *t = g_object_get_data (G_OBJECT (act->toplevel), "thing-table");
  if (t == NULL)
    {
      t = g_hash_table_new_full (g_direct_hash, g_direct_equal, 0, g_object_unref);
      g_object_set_data (G_OBJECT (act->toplevel), "thing-table", t);
      g_object_set_data (G_OBJECT (act->toplevel), "widget-list", NULL);
      g_signal_connect (act->toplevel, "destroy", G_CALLBACK (on_destroy_dataset), NULL);
    }

  return t;
}

static void
psppire_dialog_action_activate (PsppireDialogAction *act)
{
  gint response;

  PsppireDialogActionClass *class = PSPPIRE_DIALOG_ACTION_GET_CLASS (act);

  gboolean first_time = ! act->toplevel;

  set_toplevel (act);

  act->dict = PSPPIRE_DATA_WINDOW(act->toplevel)->dict;
  if (act->source)
    g_object_set (act->source, "model", act->dict, NULL);

  GSList *wl = g_object_get_data (G_OBJECT (act->toplevel), "widget-list");
  wl = g_slist_prepend (wl, act->dialog);
  g_object_set_data (G_OBJECT (act->toplevel), "widget-list", wl);

  gtk_window_set_transient_for (GTK_WINDOW (act->dialog), GTK_WINDOW (act->toplevel));

  if (GTK_ACTION_CLASS (psppire_dialog_action_parent_class)->activate)
    GTK_ACTION_CLASS (psppire_dialog_action_parent_class)->activate ( GTK_ACTION (act));

  if (act->source)
    gtk_widget_grab_focus (act->source);

  if (first_time)
    psppire_dialog_reload (PSPPIRE_DIALOG (act->dialog));

  response = psppire_dialog_run (PSPPIRE_DIALOG (act->dialog));

  if ( class->generate_syntax )
    {
      switch (response)
	{
	case GTK_RESPONSE_OK:
	  g_free (execute_syntax_string (PSPPIRE_DATA_WINDOW (act->toplevel),
					 class->generate_syntax (act)));
	  break;
	case PSPPIRE_RESPONSE_PASTE:
	  g_free (paste_syntax_to_window (class->generate_syntax (act)));
	  break;
	default:
	  break;
	}
    }
}

static void
psppire_dialog_action_class_init (PsppireDialogActionClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  GParamSpec *manager_spec =
    g_param_spec_object ("manager",
			 "Manager",
			 "The GtkUIManager which created this object",
			 GTK_TYPE_UI_MANAGER,
			 G_PARAM_READWRITE);

  GParamSpec *toplevel_spec =
    g_param_spec_object ("top-level",
			 "Top Level",
			 "The top level widget to which this dialog action belongs",
			 GTK_TYPE_WINDOW,
			 G_PARAM_READABLE);

  object_class->set_property = psppire_dialog_action_set_property;
  object_class->get_property = psppire_dialog_action_get_property;

  class->generate_syntax = NULL;

  class->activate = psppire_dialog_action_activate;

  g_object_class_install_property (object_class,
                                   PROP_MANAGER,
                                   manager_spec);

  g_object_class_install_property (object_class,
                                   PROP_TOPLEVEL,
                                   toplevel_spec);
}


static void
psppire_dialog_action_init (PsppireDialogAction *act)
{
  act->toplevel = NULL;
  act->dict = NULL;
}


void
psppire_dialog_action_set_valid_predicate (PsppireDialogAction *act, 
					   ContentsAreValid dialog_state_valid)
{
  psppire_dialog_set_valid_predicate (PSPPIRE_DIALOG (act->dialog),
                                      dialog_state_valid, act);
}

void
psppire_dialog_action_set_refresh (PsppireDialogAction *pda, 
				   PsppireDialogActionRefresh refresh)
{
  g_signal_connect_swapped (pda->dialog, "refresh", G_CALLBACK (refresh),  pda);
}


void 
psppire_dialog_action_set_activation (gpointer class, activation activate)
{
  GTK_ACTION_CLASS (class)->activate = (void (*)(GtkAction *)) activate;
}

