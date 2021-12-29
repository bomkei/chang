import os
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

    for i in glob.glob(f"{self.srcdir}/**/*.{self.extension}",recursive=True):
      self.clone_source(i)

    self.get_sources()

    for i in self.src_files:
      if self.compile(i) != 0:
        os.system(f"rm {self.objdir}/*.cpp")
        return 1
  
    if self.updated or not os.path.exists(self.output):
      print("linking...")
      os.system(f"{self.compiler} -fuse-ld=/usr/bin/mold {self.linker_flags} {' '.join(self.objects)} -o {self.output}")
    else:
      print("Already up to date.")

    os.system(f"rm {self.objdir}/*.cpp")

  def get_sources(self):
    self.src_files = glob.glob(f"{self.objdir}/*.{self.extension}")
    self.objects = [change_ext(i, "o") for i in self.src_files]

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

  def compile(self, file):
    if self.check_build(file):
      self.updated = True
      print(self.actual_src_file_cache[file[file.find("/") + 1:]])
      return os.system(f"{self.compiler} {self.flags} -c -o {change_ext(file, 'o')} {file}")
    
    return 0

  def check_build(self, file):
    ft = os.path.getmtime(self.actual_src_file_cache[file[file.find("/")+1:]])
    obj = os.path.getmtime(change_ext(file, "o"))

    if not os.path.exists(change_ext(file, "o")) or ft > obj:
      return True

    with open(change_ext(file, "d")) as fst:
      for dep in self.extract_depend_files("".join(fst.readlines())):
        if os.path.getmtime(dep) > obj:
          return True

    return False

  def clone_source(self, file):
    dest = file[len(self.srcdir) + 1:].replace("/","_")
    self.actual_src_file_cache[dest] = file

    os.system(f"cp -f {file} {self.objdir}/{dest}")
    os.system(f"{self.compiler} {self.flags} -MM -MP -MF {self.objdir}/{change_ext(dest, 'd')} {file}")

def change_ext(file, ext):
  return file[:file.rfind(".")] + "." + ext

builder = Builder()

builder.run(False)

