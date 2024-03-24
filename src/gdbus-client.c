#include <gio/gio.h>

#include "gdbus-client.h"

#define DBUS_TEST_BUS_NAME              "io.starnight.dbus_test.TestServer"
#define DBUS_TEST_OBJECT_PATH           "/io/starnight/dbus_test/TestObject"
#define DBUS_TEST_INTERFACE_NAME        "io.starnight.dbus_test.TestInterface"
#define DBUS_PROPERTY_INTERFACE_NAME    "org.freedesktop.DBus.Properties"

GDBusProxy *
get_dbus_proxy (void)
{
  GDBusProxy *proxy;
  GError *error;

  error = NULL;
  proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                         G_DBUS_PROXY_FLAGS_NONE,
                                         NULL,
                                         DBUS_TEST_BUS_NAME,
                                         DBUS_TEST_OBJECT_PATH,
                                         DBUS_TEST_INTERFACE_NAME,
                                         NULL,
                                         &error);
  g_assert_no_error (error);

  return proxy;
}

GVariant *
get_title (GDBusProxy *proxy)
{
  return g_dbus_proxy_get_cached_property(proxy, "Title");
}

void
set_title (GDBusProxy  *proxy,
           const gchar *title)
{
  GDBusConnection *conn = g_dbus_proxy_get_connection (proxy);
  GVariant *value;
  GVariant *ret;
  GError *error;

  value = g_variant_new ("s", title);

  error = NULL;
  ret = g_dbus_connection_call_sync (conn,
                                     DBUS_TEST_BUS_NAME,
                                     DBUS_TEST_OBJECT_PATH,
                                     DBUS_PROPERTY_INTERFACE_NAME,
                                     "Set",
                                     g_variant_new ("(ssv)",
                                                    DBUS_TEST_INTERFACE_NAME,
                                                    "Title",
                                                    value),
                                     NULL,
                                     G_DBUS_CALL_FLAGS_NONE,
                                     -1,
                                     NULL,
                                     &error);
  g_assert_no_error (error);
  g_variant_unref (ret);
}

void
send_msg (GDBusProxy  *proxy,
          const gchar *msg)
{
  GVariant *ret;
  GError *error;

  error = NULL;
  ret = g_dbus_proxy_call_sync (proxy,
                                "SendMsg",
                                g_variant_new ("(s)", msg),
                                G_DBUS_CALL_FLAGS_NONE,
                                -1,
                                NULL,
                                &error);
  g_assert_no_error (error);
  g_variant_unref (ret);
}

void
free_dbus_proxy (GDBusProxy *proxy)
{
  if (proxy != NULL)
    g_object_unref (proxy);
}
