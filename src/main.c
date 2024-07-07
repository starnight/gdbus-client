#include <gtk/gtk.h>

#include "gdbus-client.h"

GDBusProxy *proxy = NULL;

gchar *
get_dbus_title (void)
{
  GVariant *value;
  gchar *title; 
  
  value = get_title(proxy);
  if (value == NULL)
    return NULL;

  g_variant_get (value, "s", &title);
  g_variant_unref (value);

  return title;
}

static void
update_window_title (GtkWidget *window,
                     gchar *title)
{
  gtk_window_set_title (GTK_WINDOW (window), title);
}

static void
button_click_handler (GtkWidget *widget,
                      gpointer   data)
{
  GtkEntry *entry = data;
  GtkEntryBuffer *buffer = gtk_entry_get_buffer (entry);
  const gchar *text = gtk_entry_buffer_get_text (buffer);
  const gchar *magic_title_cmd = "/title ";
  glong len = g_utf8_strlen(magic_title_cmd, -1);

  if (strncmp (text, magic_title_cmd, len) == 0)
  {
    if (text[len] != '\0')
      set_title (proxy, text + len);
  }
  else
  {
    send_msg (proxy, text);
  }

  gtk_entry_buffer_delete_text (buffer, 0, -1);
}

static void
update_textview (GtkTextView *textview,
                 const gchar *text)
{
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (textview);
  GtkTextIter iter;

  gtk_text_buffer_get_end_iter (buffer, &iter);
  gtk_text_buffer_insert (buffer, &iter, text, -1);
}

static void
msg_notify_handler (GDBusProxy  *proxy,
                    const gchar *sender_name,
                    const gchar *signal_name,
                    GVariant    *parameters,
                    gpointer     user_data)
{
  GtkTextView *textview = user_data;
  gchar *sender;
  gchar *msg;
  gchar *text;
  
  /* The arguments map to "MsgNotification" signal
   * https://github.com/starnight/gdbus-server/?tab=readme-ov-file#introspect-the-bus-object
   */
  g_variant_get (parameters, "(&s&s)", &sender, &msg);
  text = g_strdup_printf ("%s: %s\n", sender, msg);
  update_textview (textview, text);

  g_free (text);
}

static void
dbus_signals_handler (GDBusProxy  *proxy,
                      const gchar *sender_name,
                      const gchar *signal_name,
                      GVariant    *parameters,
                      gpointer     user_data)
{
  if (g_strcmp0 (signal_name, "MsgNotification") == 0)
    msg_notify_handler (proxy, sender_name, signal_name, parameters, user_data);
}

static void
title_changed_handler (GVariant *value,
                       gpointer  user_data)
{
  GtkWidget *window = user_data;
  gchar *title;
  
  g_variant_get (value, "&s", &title);
  update_window_title (window, title);
}

static void
dbus_properties_changed_handler (GDBusProxy   *proxy,
                                 GVariant     *changed_properties,
                                 const gchar **invalidated_properties,
                                 gpointer      user_data)
{
  GVariantIter *iter;
  const gchar *key;
  GVariant *value;

  /* The arguments map to "PropertiesChanged" signal
   * https://github.com/starnight/gdbus-server/?tab=readme-ov-file#introspect-the-bus-object
   */
  g_variant_get (changed_properties, "a{sv}", &iter);
  while (g_variant_iter_loop (iter, "{&sv}", &key, &value))
  {
    if (g_strcmp0 (key, "Title") == 0)
      title_changed_handler (value, user_data);
  }
  g_variant_iter_free (iter);
}

static void
layout (GtkWidget *window)
{
  GtkWidget *box1;
  GtkWidget *textview_box;
  GtkWidget *send_msg_box;
  GtkWidget *scrolledwindow;
  GtkWidget *textview;
  GtkWidget *entry;
  GtkWidget *button;

  box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child (GTK_WINDOW (window), box1);

  textview_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  scrolledwindow = gtk_scrolled_window_new ();
  gtk_widget_set_hexpand (scrolledwindow, TRUE);
  gtk_widget_set_vexpand (scrolledwindow, TRUE);
  textview = gtk_text_view_new ();
  gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scrolledwindow), textview);
  gtk_box_append (GTK_BOX (textview_box), scrolledwindow);
  gtk_box_append (GTK_BOX (box1), textview_box);

  send_msg_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  entry = gtk_entry_new ();
  gtk_widget_set_hexpand (entry, TRUE);
  g_signal_connect (entry, "activate", G_CALLBACK (button_click_handler), entry);
  button = gtk_button_new_with_label ("Send");
  g_signal_connect (button, "clicked", G_CALLBACK (button_click_handler), entry);
  gtk_box_append (GTK_BOX (send_msg_box), entry);
  gtk_box_append (GTK_BOX (send_msg_box), button);
  gtk_box_append (GTK_BOX (box1), send_msg_box);

  g_signal_connect (proxy,
                    "g-signal",
                    G_CALLBACK (dbus_signals_handler),
                    textview);
  g_signal_connect (proxy,
                    "g-properties-changed",
                    G_CALLBACK (dbus_properties_changed_handler),
                    window);
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  gchar *title;

  window = gtk_application_window_new (app);
  proxy = get_dbus_proxy ();

  title = get_dbus_title ();
  update_window_title (window, title);
  g_free (title);

  gtk_window_set_default_size (GTK_WINDOW (window), -1, 350);
  layout(window);

  gtk_window_present (GTK_WINDOW (window));
}

static void
on_close (GtkApplication *app,
          GtkWindow      *window,
          gpointer        user_data)
{
  free_dbus_proxy (proxy);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("io.starnight.gdbus-client", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect (app, "window-removed", G_CALLBACK (on_close), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
