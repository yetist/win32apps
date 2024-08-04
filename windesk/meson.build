project('windesk', 'c'
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

soversion = 2

if version_minor.is_odd()
  interface_age = 0
else
  interface_age = version_micro
endif

current = version_minor * 100 + version_micro - interface_age
revision = interface_age
libversion = '@0@.@1@.@2@'.format(soversion, current, revision)
package_string = '@0@-@1@.0'.format(package_name, soversion)

config_h = configuration_data()
config_h.set_quoted('PACKAGE', package_name)
config_h.set_quoted('PACKAGE_NAME', package_name)
config_h.set_quoted('PACKAGE_STRING', package_string)
config_h.set_quoted('PACKAGE_VERSION', package_version)
config_h.set_quoted('GETTEXT_PACKAGE', package_name)
config_h.set_quoted('VERSION', package_version)

config_h.set_quoted('MARCO_LOCALEDIR', localdir)
config_h.set_quoted('LIBDIR', libdir)
config_h.set_quoted('MARCO_DATADIR', datadir)

configure_file(
    input: 'version.h.in'
    output: 'version.h',
    configuration: config_h
)

# Compiler & Project arguments
add_project_arguments([
  '-DHAVE_CONFIG_H',
  '-DUNICODE',
  '-I' + meson.build_root(),
  ], language: 'c')

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
    gui_app : true
)

test('windesk', windesk)
