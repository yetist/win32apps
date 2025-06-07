project('timer', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('timer.rc',
  include_directories : include_directories('.'),
)

exe = executable('timer',
  'timer.c',
  res,
  gui_app : true
)

test('timer', exe)
