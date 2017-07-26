#include <stdio.h>
#include <unistd.h>
#include <gtk/gtk.h>

static void destroy(GtkWidget *widget, gpointer data);
static void unlock_gtk(GtkWidget *widget, GtkWidget *passwd);
static void initialize_window(GtkWidget* window, char* name);

int main (int argc, char *argv[]) {

  #ifdef DEBUG
    fprintf(stderr, "GTK: executing as user %d\n", getuid());
  #endif

  GtkWidget *window, *hbox, *table, *icon, *passwd, *button, *label;
  gtk_init (&argc, &argv);

  PangoFontDescription *df;
  df = pango_font_description_from_string("Arial");

  //Create the main window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  initialize_window(window, argv[1]);

  // create layout
  hbox = gtk_hbox_new(0,0);
  gtk_container_set_border_width (GTK_CONTAINER(hbox), 20);
  gtk_container_add(GTK_CONTAINER(window),hbox);

  icon = gtk_image_new_from_file ("padlock.png");
  gtk_container_add(GTK_CONTAINER(hbox), icon);

  /* Create a 1x2 table */
  table = gtk_table_new (2, 1, FALSE);
  gtk_container_add(GTK_CONTAINER(hbox), table);

  passwd = gtk_entry_new();
  pango_font_description_set_size(df, 13*PANGO_SCALE);
  gtk_widget_modify_font(passwd, df);
  gtk_entry_set_visibility(GTK_ENTRY(passwd), FALSE);
  gtk_entry_set_activates_default(GTK_ENTRY(passwd), TRUE);
  gtk_entry_set_width_chars(GTK_ENTRY(passwd), 20);
  gtk_table_attach_defaults(GTK_TABLE(table), passwd, 0, 1, 0, 1);
  g_signal_connect(passwd, "activate", G_CALLBACK(unlock_gtk), passwd);

  button = gtk_button_new();
  gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 1, 1, 2);
  g_signal_connect(button, "clicked", G_CALLBACK(unlock_gtk), passwd);

  label = gtk_label_new("Unlock");
  pango_font_description_set_size(df, 16*PANGO_SCALE);
  gtk_widget_modify_font(label, df);
  gtk_container_add(GTK_CONTAINER(button), label);

  gtk_widget_show_all(window);
  gtk_widget_grab_focus(passwd);
  gtk_main ();

  return 0;

}


static void initialize_window(GtkWidget* window, char* name) {
  gchar buf[64];
  g_snprintf(buf, sizeof(buf), "Please enter LUKS key for %s", name);
  gtk_window_set_title(GTK_WINDOW(window),buf);
  gtk_window_set_default_size(GTK_WINDOW (window), 350, 140);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  g_signal_connect(window, "destroy", G_CALLBACK (destroy), NULL);
}

static void unlock_gtk(GtkWidget *widget, GtkWidget *passwd) {
  printf("%s", (char *)gtk_entry_get_text(GTK_ENTRY(passwd)));
  gtk_main_quit();
}

static void destroy(GtkWidget *widget, gpointer data) {
  gtk_main_quit();
}
