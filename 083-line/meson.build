project('line.c', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

#res = win.compile_resources('line.rc',
#  include_directories : include_directories('.'),
#)

exe = executable('line',
  'line.c',
  #  res,
  gui_app : true
)

test('line', exe)
