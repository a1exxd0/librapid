# coding: utf8

import os
import platform
import distutils.sysconfig
import shutil
from pathlib import Path

# If using windows, tell the os where the DLL for blas is
# If blas was not installed, this doesn't really do anything
if platform.system() == "Windows":
	import win32api
	# this_directory = Path(__file__).parent
	this_directory = os.path.join(__file__, os.pardir)

	print("Loading DLL from", os.path.join(this_directory, "blas"))
	win32api.SetDllDirectory(os.path.join(this_directory, "blas"))

from librapid_ import *
