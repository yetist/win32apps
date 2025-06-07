project('mdi', 'c')

add_project_arguments(['-DUNICODE'], language: 'c')

cc = meson.get_compiler('c')
root = meson.get_external_property('root')

win = import('windows')
#lib_richedit = cc.find_library('comctl32',  dirs : [root])
#lib_richedit2 = cc.find_library('kernel32',  dirs : [root])

res = win.compile_resources('mdi.rc',
  include_directories : include_directories('.'),
)

exe = executable('mdi',
  'main.c',
  res,
  gui_app : true,
  #dependencies: [lib_richedit], #, lib_richedit2],
)

test('mdi', exe)



