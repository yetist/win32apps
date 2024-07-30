project('winmaincpp', 'c')

exe = executable('prog', 'main.c', gui_app : true)
hello = executable('hello', 'hello.c', gui_app : true)

cc = meson.get_compiler('c')
root = meson.get_external_property('root')

lib_winmm  = cc.find_library('winmm',  dirs : [root])

window = executable('window',
    'window.c',
    dependencies: [lib_winmm],
    gui_app : true
)

test('winmaincpp', exe)
test('hello', hello)
test('window', window)
