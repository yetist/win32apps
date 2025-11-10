project('shape.c', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('shape.rc',
  include_directories : include_directories('.'),
)

exe = executable('shape',
  'shape.c',
  res,
  gui_app : true
)

test('shape', exe)
