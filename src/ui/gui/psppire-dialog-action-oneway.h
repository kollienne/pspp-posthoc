/* PSPPIRE - a graphical user interface for PSPP.
   Copyright (C) 2013  Free Software Foundation

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


#include <glib-object.h>
#include <glib.h>

#include "psppire-dialog-action.h"

#ifndef __PSPPIRE_DIALOG_ACTION_ONEWAY_H__
#define __PSPPIRE_DIALOG_ACTION_ONEWAY_H__

G_BEGIN_DECLS


#define PSPPIRE_TYPE_DIALOG_ACTION_ONEWAY (psppire_dialog_action_oneway_get_type ())

#define PSPPIRE_DIALOG_ACTION_ONEWAY(obj)	\
                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
						  PSPPIRE_TYPE_DIALOG_ACTION_ONEWAY, PsppireDialogActionOneway))

#define PSPPIRE_DIALOG_ACTION_ONEWAY_CLASS(klass) \
                     (G_TYPE_CHECK_CLASS_CAST ((klass), \
				 PSPPIRE_TYPE_DIALOG_ACTION_ONEWAY, \
                                 PsppireDialogActionOnewayClass))


#define PSPPIRE_IS_DIALOG_ACTION_ONEWAY(obj) \
	             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PSPPIRE_TYPE_DIALOG_ACTION_ONEWAY))

#define PSPPIRE_IS_DIALOG_ACTION_ONEWAY_CLASS(klass) \
                     (G_TYPE_CHECK_CLASS_TYPE ((klass), PSPPIRE_TYPE_DIALOG_ACTION_ONEWAY))


#define PSPPIRE_DIALOG_ACTION_ONEWAY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), \
				   PSPPIRE_TYPE_DIALOG_ACTION_ONEWAY, \
				   PsppireDialogActionOnewayClass))

typedef struct _PsppireDialogActionOneway       PsppireDialogActionOneway;
typedef struct _PsppireDialogActionOnewayClass  PsppireDialogActionOnewayClass;


struct _PsppireDialogActionOneway
{
  PsppireDialogAction parent;

  /*< private >*/
  gboolean dispose_has_run ;

  GtkWidget *descriptives;
  GtkWidget *homogeneity;

  GtkWidget *posthoc_bonferroni;
  GtkWidget *posthoc_gh;
  GtkWidget *posthoc_lsd;
  GtkWidget *posthoc_scheffe;
  GtkWidget *posthoc_sidak;
  GtkWidget *posthoc_tukey;


  GtkWidget *contrasts_dialog;
  GtkWidget *posthoc_dialog;

  GtkWidget *next;
  GtkWidget *prev;
  GtkWidget *acr;
  GtkWidget *vars_treeview;
  GtkWidget *factor_entry;


  /* Gets copied into contrasts when "Continue"
     is clicked */
  GArray *temp_contrasts;

  /* Index into the temp_contrasts */
  guint c;


  GArray *contrasts_array;

  GtkWidget *ctotal;
  GtkWidget *stack_label;
};


struct _PsppireDialogActionOnewayClass
{
  PsppireDialogActionClass parent_class;
};


GType psppire_dialog_action_oneway_get_type (void) ;

G_END_DECLS

#endif /* __PSPPIRE_DIALOG_ACTION_ONEWAY_H__ */
