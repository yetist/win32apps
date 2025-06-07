project('menu', 'c')

win = import('windows')

res = win.compile_resources('menu.rc',
  depend_files: ['Menu.ico', 'Small.ico'],
  include_directories : include_directories('.'),
  args : [['-DFOO'], '-DBAR']
)

exe = executable('menu',
  'menu.c',
  res,
  gui_app : true
)

test('menu', exe)
