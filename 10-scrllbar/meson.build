project('scrllbar', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('scrllbar.rc',
  include_directories : include_directories('.'),
)

exe = executable('scrllbar',
  'scrllbar.c',
  res,
  gui_app : true
)

test('scrllbar', exe)
