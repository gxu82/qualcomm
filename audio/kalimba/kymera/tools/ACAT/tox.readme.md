# What is TOX?

Sometimes Python developers need to develop applications for different
versions of Pythons. Testing the code on all of those Python versions can
be repetitive and tedious if it is handled manually. Hopefully Tox is
there to help. It creates various virtual environments and execute
specified commands for them.

Please refer to their [Official Documentation][1] for more info. Also,
within the documentation, there is a helpful page for [tips and tricks][2]
which has shortcuts to the most useful commands.

[1]: https://tox.readthedocs.io/en/latest/
[2]: https://tox.readthedocs.io/en/latest/example/general.html


# How to use TOX in ACAT

This section should guide you on how to use tox from the tool installation
to create your own test environments.

## Install the tox package

Tox package can be installed with a simple `pip` command.

```
python -m pip install tox  # The latest version should be fine
```

Please note, if you have already created your virtual environment for ACAT
development you don't need to install tox anymore since the package is
already installed when you have issued `pip install -r requirements.txt`
command.

## Environments

There are already various environments configured for ACAT in `tox.ini`.
This configuration file lives in the same directory where `setup.py`
lives. However, please be cautious, these environments are formed based on
**assumption** that their correspondent python executables are installed
in a very specific location. For example:

```
[testenv:windows_py27_32]
platform = win32
basepython = C:\Python27\python.exe
description = "Windows python 2.7 32 bits"
```

Assumes that the Python 2.7 is installed in `C:\Python27\python.exe` for
windows_py27_32 environment, which may or may not be true.

To make this work for you, please add your own environments in the
`tox.ini` file with correct path to your python versions. For doing so,
you can copy and pase above section and change the name and `basepython`
variable to your specific one.


After that, it should be easy to call `tox` to run only for those
environments.  Assuming your new environments are called
`windows_rf19_py27_32` and `windows_rf19_py35_32` the tox command should
be something similar to:

```
tox -e windows_rf19_py27_32, windows_rf19_py35_32
```

Tox will use python executables in those environments which are defined in
the `tox.ini` configuration file, and automatically creates Python Virtual
Environments for each of them. After the creation of each environment, all
the unittests and coredump tests will be executed on it. At the end you
should be able to see test results for all of them.
