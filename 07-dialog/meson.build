project('dialog', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('resource.rc',
  include_directories : include_directories('.'),
)

exe = executable('dialog1',
  'dialog1.c',
  res,
  gui_app : true
)

exe = executable('dialog2',
  'dialog2.c',
  res,
  gui_app : true
)

exe = executable('dialog3',
  'dialog3.c',
  res,
  gui_app : true
)

test('dialog1', exe)
test('dialog2', exe)
test('dialog3', exe)
