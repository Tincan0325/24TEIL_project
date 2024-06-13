with open('req.txt', 'r') as file:
    packages = [line.strip() for line in file.readlines()]

conda_packages = []
pip_packages = []

for package in packages:
    if package in conda_packages_list:
        conda_packages.append(package)
    else:
        pip_packages.append(package)

# 安装通过conda安装的包
if conda_packages:
    conda_command = 'conda install ' + ' '.join(conda_packages)
    subprocess.run(conda_command, shell=True)

# 安装通过pip安装的包
if pip_packages:
    pip_command = 'pip install ' + ' '.join(pip_packages)
    subprocess.run(pip_command, shell=True)
