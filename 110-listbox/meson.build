project('listbox', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('listbox.rc',
  include_directories : include_directories('.'),
)

exe = executable('listbox',
  'listbox.c',
  res,
  gui_app : true
)

test('listbox', exe)
