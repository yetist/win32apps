project('statbar', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

cc = meson.get_compiler('c')
root = meson.get_external_property('root')

win = import('windows')
lib_comctl32 = cc.find_library('comctl32',  dirs : [root])

res = win.compile_resources('statbar.rc',
  include_directories : include_directories('.'),
)

exe = executable('statbar',
  'statbar.c',
  res,
  gui_app : true,
  dependencies: [lib_comctl32],
)

test('statbar', exe)
