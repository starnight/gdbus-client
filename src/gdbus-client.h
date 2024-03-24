#include <gio/gio.h>

GDBusProxy* get_dbus_proxy (void);
GVariant* get_title (GDBusProxy*);
void set_title (GDBusProxy*, const gchar*);
void send_msg (GDBusProxy*, const gchar*);
void free_dbus_proxy (GDBusProxy*);
