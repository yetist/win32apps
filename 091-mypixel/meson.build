project('mypixel', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('mypixel.rc',
  include_directories : include_directories('.'),
)

exe = executable('mypixel',
  'mypixel.c',
  res,
  gui_app : true
)

test('mypixel', exe)
