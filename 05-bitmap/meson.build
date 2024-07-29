project('bitmap', 'c')

win = import('windows')

res = win.compile_resources('bitmap.rc',
  depend_files: ['sun.bmp', 'moon.bmp', 'rcCursor.cur', 'rcIcon.ico', 'Small.ico'],
  include_directories : include_directories('.'),
  args : [['-DFOO'], '-DBAR']
)

exe = executable('bitmap',
  'bitmap.c',
  res,
  gui_app : true
)

test('bitmap', exe)
