project('editbox', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('editbox.rc',
  include_directories : include_directories('.'),
)

exe = executable('editbox',
  'editbox.c',
  res,
  gui_app : true
)

test('editbox', exe)
