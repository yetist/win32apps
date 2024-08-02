project('windesk', 'c')

add_project_arguments(['-DUNICODE', '-DDEBUG'], language: 'c')

win = import('windows')

res = win.compile_resources('resource.rc',
  include_directories : include_directories('.'),
)

#cc = meson.get_compiler('c')
#root = meson.get_external_property('root')
#
#lib_winmm  = cc.find_library('winmm',  dirs : [root])
    #    dependencies: [lib_winmm],

windesk = executable('windesk',
    'debug.c',
    'desktop.c',
    'utils.c',
    'windesk.c',
    res,
    gui_app : false
)

test('windesk', windesk)
