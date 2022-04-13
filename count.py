import glob

cxx = glob.glob("src/**/*.cc", recursive=True) + \
  glob.glob("src/**/*.cpp", recursive=True)

header = glob.glob("include/**/*.h", recursive=True)

files = cxx + header

total = 0

for i in files:
  with open(i) as f:
    c = len(f.readlines())
    total += c
    print(f"{i} = {c}")

print(f"\n{len(cxx)} C++ files\n{len(header)} headers\ntotal: {len(files)} files = {total}")

