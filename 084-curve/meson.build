project('curve.c', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('curve.rc',
  include_directories : include_directories('.'),
)

exe = executable('curve',
  'curve.c',
  res,
  gui_app : true
)

test('curve', exe)
