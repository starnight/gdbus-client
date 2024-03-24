gio_dep = dependency('gio-2.0')
gtk_dep = dependency('gtk4')
deps = [
  gio_dep,
  gtk_dep
]

sources = [
  'main.c',
  'gdbus-client.c'
]

executable('gdbus-client', sources, dependencies: deps, install: true)

