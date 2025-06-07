project('dialog', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('mouse.rc',
  include_directories : include_directories('.'),
)

exe = executable('mouse',
  'mouse.c',
  res,
  gui_app : true
)

test('mouse', exe)
