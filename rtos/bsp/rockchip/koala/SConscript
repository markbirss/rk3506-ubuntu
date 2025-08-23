# for module compiling
import os
Import('RTT_ROOT')

cwd = str(Dir('#'))
objs = []
list = os.listdir(cwd)

objs = SConscript(os.path.join(cwd, 'HalSConscript'), variant_dir = '.', duplicate=0)

for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        objs = objs + SConscript(os.path.join(d, 'SConscript'))

objs = objs + SConscript(os.path.join(RTT_ROOT, 'bsp/rockchip/common/drivers/SConscript'), variant_dir = 'common/drivers', duplicate=0)
objs = objs + SConscript(os.path.join(RTT_ROOT, 'applications/SConscript'), variant_dir = 'applications', duplicate=0)
objs = objs + SConscript(os.path.join(RTT_ROOT, 'bsp/rockchip/common/tests/SConscript'), variant_dir = 'common/tests', duplicate=0)
objs = objs + SConscript(os.path.join(RTT_ROOT, 'examples/kernel/SConscript'), variant_dir = 'examples/kernel', duplicate=0)

Return('objs')
