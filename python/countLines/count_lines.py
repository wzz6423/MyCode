import os


def count_lines(filepath):
    try:
        with open(filepath, 'rb') as f:
            return sum(1 for _ in f)
    except:
        return 0


total = 0
for root, _, files in os.walk('E:/Gitee/linux_test/StellarPost'):
    for file in files:
        full_path = os.path.join(root, file)
        total += count_lines(full_path)

print(total)