project('menu.c', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('menu.rc',
  include_directories : include_directories('.'),
)

exe = executable('menu',
  'menu.c',
  res,
  gui_app : true
)

test('menu', exe)
