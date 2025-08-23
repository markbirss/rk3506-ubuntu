project(
  'alsa-config',
  version : '1.1.0',
  meson_version : '>=0.38.0',
)

install_subdir('alsa', install_dir : 'share', install_mode : ['rw-r--r--', 'root'])
if get_option('init_script').enabled()
  install_data(sources: 'alsa-utils', rename: 'S11alsa-utils', install_dir : '/etc/init.d', install_mode : ['rwxr-xr-x', 'root'])
endif
