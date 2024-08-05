project('windesk', 'c',
    version: '1.25.0',
    meson_version: '>=0.47.0',
    license: 'GPL2',
    default_options : [ 'warning_level=1', 'buildtype=debugoptimized', 'c_std=gnu11'],
)

package_name = meson.project_name()
package_version = meson.project_version()

# Versionning
version_arr = package_version.split('.')
version_major = version_arr[0].to_int()
version_minor = version_arr[1].to_int()
version_micro = version_arr[2].to_int()

config_h = configuration_data()
config_h.set('VERSION_MAJOR', version_major)
config_h.set('VERSION_MINOR', version_minor)
config_h.set('VERSION_MICRO', version_micro)
config_h.set('PACKAGE_NAME', package_name)
config_h.set('PACKAGE_VERSION', package_version)

configure_file(
    input: 'version.h.in',
    output: 'version.h',
    configuration: config_h
)

# Compiler & Project arguments
add_project_arguments([
  '-DHAVE_CONFIG_H',
  '-DUNICODE',
  '-I' + meson.build_root(),
  ], language: 'c')

enable_debug = false
if get_option('buildtype') == 'debug'
    enable_debug = true
    add_project_arguments(['-D_DEBUG'], language: 'c')
endif

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
    gui_app : enable_debug
)

test('windesk', windesk)
