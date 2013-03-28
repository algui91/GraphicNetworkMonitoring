#!/usr/bin/env python

from distutils.core import setup, Extension

SRC = 'src/'

module1 = Extension('gnm',
                    include_dirs = ['./include'],
#                    extra_link_args = ['-O0'],
                    sources = [SRC + 'gnmModule.c'])

setup (name = 'GraphicNetworkMonitoring',
       version = '0.1',
       description = 'GNM - Graphic Network Monitoring tool',
       author = 'Alejandro Alcalde',
       author_email = 'algui91@gmail.com',
       url = 'https://github.com/algui91/GraphicNetworkMonitoring',
       license = 'GPLv3',
       long_description = '''
       TODO
       ''',
       ext_modules = [module1])
