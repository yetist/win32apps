project('combobox', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('combobox.rc',
  include_directories : include_directories('.'),
)

exe = executable('combobox',
  'combobox.c',
  res,
  gui_app : true
)

test('combobox', exe)
