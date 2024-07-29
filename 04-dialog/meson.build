project('dialog', 'c')

win = import('windows')

res = win.compile_resources('dialog.rc',
  depend_files: ['sun.bmp', 'moon.bmp', 'rcCursor.cur', 'rcIcon.ico', 'Small.ico'],
  include_directories : include_directories('.'),
  args : [['-DFOO'], '-DBAR']
)

exe = executable('dialog',
  'dialog.c',
  res,
  gui_app : true
)

test('dialog', exe)
