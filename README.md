# gdbus-client

This is a D-Bus client example written with GTK GLib C &amp; Gio.
The corresponding D-Bus server example is [gdbus-server](https://github.com/starnight/gdbus-server/tree/main).

## Build & Execute

### Normal Build

```sh
$ meson setup builddir
$ cd builddir
$ meson compile
$ ./gdbus-client
```

### Build & Execute with Flatpak

It is based on GNOME 45 runtime now.

```sh
$ flatpak-builder build-dir build-aux/flatpak/io.starnight.gdbus-client.yaml --force-clean --install --user
$ flatpak run io.starnight.gdbus-client
```

## Reference

* [GTK](https://docs.gtk.org/gtk4/index.html)
* [GIO](https://docs.gtk.org/gio/index.html)
* [GIO's test gdbus-example-watch-proxy.c](https://gitlab.gnome.org/GNOME/glib/-/blob/HEAD/gio/tests/gdbus-example-watch-proxy.c)
* [bluetooth: Set Adapter Powered property and watch signal using GDBUS](https://www.linumiz.com/bluetooth-set-adapter-powered-property-and-watch-signal-using-gdbus/)
* D-Bus debug tools: [dbus-monitor](https://dbus.freedesktop.org/doc/dbus-monitor.1.html) and [Bustle](https://gitlab.gnome.org/World/bustle)
