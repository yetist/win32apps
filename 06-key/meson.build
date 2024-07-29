project('key', 'c')

win = import('windows')

res = win.compile_resources('key.rc',
  depend_files: ['resources/key.ico', 'resources/small.ico'],
  include_directories : include_directories('.'),
  args : [['-DFOO'], '-DBAR']
)

exe = executable('key',
  'key.c',
  res,
  gui_app : true
)

test('key', exe)
