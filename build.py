import os
import sys
import glob
from concurrent.futures import ThreadPoolExecutor
from collections import defaultdict

class Builder:
  def __init__(self):
    self.output = "chang"
    self.output_time = os.path.getmtime(self.output) if os.path.exists(self.output) else 0

    self.compiler = "clang++"
    self.extension = "cpp"

    self.include = "include"
    self.srcdir = "src"
    self.objdir = "build"

    self.flags = f"-g -O2 -std=c++20 -I{self.include} -Wno-switch"
    self.linker_flags = f"-Wl,--gc-sections"
    
    self.src_files = []
    self.objects = []
    
    self.actual_src_file_cache = defaultdict(str)
    self.updated = False
  
  def run(self, with_multi_thread: bool):
    if not os.path.isdir(self.objdir):
      os.system(f"mkdir {self.objdir}")

    self.src_files = glob.glob(f"{self.srcdir}/**/*.{self.extension}",recursive=True)
    self.objects = [change_ext(self.to_dest_name(i), "o") for i in self.src_files]

    if with_multi_thread:
      with ThreadPoolExecutor() as exe:
        exe.map(compile_wrap, self.src_files)
    else:
      for i in self.src_files:
        if self.compile(i) != 0:
          return 1
  
    if self.updated or not os.path.exists(self.output):
      print("linking...")
      os.system(f"{self.compiler} -fuse-ld=/usr/bin/mold {self.linker_flags} {' '.join(self.objects)} -o {self.output}")
    else:
      print("Already up to date.")

  def extract_depend_files(self, text):
    incl = self.include
    ret = [ ]

    pos = text.find(incl)
    
    while pos != -1:
      text = text[pos:]
      x = text[:text.find(".h") + 2]

      ret.append(x)
      text = text[len(x):]

      pos = text.find(incl)
    
    return ret

  def to_dest_name(self, file):
    return self.objdir + "/" + file[len(self.srcdir) + 1:].replace("/", "_")

  def compile(self, file):
    if self.check_build(file):
      self.updated = True
      print(file)
      return os.system(f"{self.compiler} {self.flags} -MMD -MP -MF {change_ext(self.to_dest_name(file), 'd')} -c -o {change_ext(self.to_dest_name(file), 'o')} {file}")
    
    return 0

  def check_build(self, file):
    ft = os.path.getmtime(file)
    obj_path = change_ext(self.to_dest_name(file), "o")
    dep_path = change_ext(obj_path, "d")
    obj = os.path.getmtime(obj_path) if os.path.exists(obj_path) else 0

    if not os.path.exists(obj_path) or ft > obj:
      return True
    
    if not os.path.exists(change_ext(obj_path, "d")):
      return True

    with open(dep_path) as fst:
      for dep in self.extract_depend_files("".join(fst.readlines())):
        if os.path.getmtime(dep) > obj:
          return True

    return False

def change_ext(file, ext):
  return file[:file.rfind(".")] + "." + ext

def compile_wrap(file):
  builder.compile(file)

builder = Builder()
argv = sys.argv

if "clean" in argv:
  os.system(f"rm -drf {builder.objdir}")
  exit(1)

builder.run("-j" in argv)

