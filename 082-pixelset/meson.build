project('pixelset.c', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

win = import('windows')

res = win.compile_resources('pixelset.rc',
  include_directories : include_directories('.'),
)

exe = executable('pixelset',
  'pixelset.c',
  res,
  gui_app : true
)

test('pixelset', exe)
