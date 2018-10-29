#!/usr/bin/env python3

import os
from setuptools import setup

try:
    os.symlink("../../c/tdigest.so", "python/tdigestc/tdigest.so")
except:
    pass

import sys
print(sys.version_info)

setup(
    name="tdigestc",
    version="0.1.0",
    author="Andrew Werner",
    author_email="awerner32@gmail.com",
    description="Compact, approximate quantile estimation for data streams",
    packages=["tdigestc"],
    package_dir={"tdigestc": "python/tdigestc"},
    package_data={"tdigestc": ["tdigest.so"]},
    include_package_data=True,
)

