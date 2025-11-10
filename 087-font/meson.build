project('font.c', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('font.rc',
  include_directories : include_directories('.'),
)

exe = executable('font',
  'font.c',
  res,
  gui_app : true
)

test('font', exe)
