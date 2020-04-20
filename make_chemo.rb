#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from subprocess import Popen, PIPE
import re
import shutil
from os import environ, rename, system
from pathlib import Path
import sys
import hashlib
from datetime import datetime

def get_name():
    name = ''
    with open(Path.cwd() / "Source/version.c", 'r') as f:
        name = (re.findall('\".+?\"', str(f.readlines())))[1].strip('"')
    return name

def get_version():
    v = []
    with open(Path.cwd() / "Source/version.c", 'r') as f:
        for line in f:
            line = line.strip('\n')
            if 'MAJOR_VERSION' in line:
                v.append(re.findall("\d+", line))
            elif 'MINOR_VERSION' in line:
                v.append(re.findall("\d+", line))
            elif 'REVISION_VERSION' in line:
                v.append(re.findall("\d+", line))
            elif 'INTERNAL_VERSION' in line:
                v.append(re.findall("\d+", line))
                break
    return [j[0] for j in v]

def make_release(version):
    file_name = get_name()
    my_env = environ.copy()
    my_env["PATH"] = "C:\Program Files\IAR Systems\Embedded Workbench 7.3\common\\bin;" \
                     + my_env["PATH"]
    cmd = "iarbuild" + " build\\IAR\\" + file_name + ".ewp" + " -make Release"

    print(cmd)
    p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, env = my_env )#bufsize=1
    
    ret = ''
    while True:
        out = p.stdout.read()
        if not out and p.poll() != None: #out == '' 
            break
        if out:
            ret += out.decode('utf-8')
            print(out.decode('utf-8'), end='', flush=True)
 
    if 'up-to-date' in ret:
        print('\nfirmware already up to date')
        return

    errors = re.findall("\d+", ret.split('\r\n')[-3])
    # print(errors)
    if errors[0] == '0':
        print('Build success!')
        s = file_name + 'V' + version
        p = Path.cwd() / s
        if p.exists():
            for file in p.glob('*'):
                file.unlink()
        else:
            p.mkdir()
        in_file = Path.cwd() / ('build/iar/Release/exe/' +file_name + '.bin')
        # in_file.rename(p / (file_name + '.bin'))
        shutil.copy(in_file, p)
        rename(p / (file_name + '.bin'), p / (s+'.bin'))
        file = p / (s+'.bin')
        with open(p/'description.txt', 'w') as f:
            f.write('sha1: {}\n'.format(hashlib.sha1(file.read_bytes()).hexdigest()))      
            f.write('release date: {}\n'.format(datetime.now().strftime('%c')))
  
    else:
        print('Build fail!')

if __name__ == "__main__":
    # print(get_version())
    print(datetime.now())
    v = '.'.join(c for c in get_version())
    print('building {}, version {}...'.format(get_name(),v))
    make_release(v)
    