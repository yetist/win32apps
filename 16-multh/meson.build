project('multh', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('multh.rc',
  include_directories : include_directories('.'),
)

exe = executable('multh',
  'multh.c',
  res,
  gui_app : true
)

test('multh', exe)
