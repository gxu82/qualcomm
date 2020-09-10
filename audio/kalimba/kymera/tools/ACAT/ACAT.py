############################################################################
# CONFIDENTIAL
#
# Copyright (c) 2012 - 2018 Qualcomm Technologies, Inc. and/or its
# subsidiaries. All rights reserved.
#
############################################################################
"""
ACAT's main script will import ACAT as a package and run it.
"""
from __future__ import print_function
import os
import sys

from ACAT import parse_args, load_session, do_analysis
from ACAT.Core.CoreTypes import Usage
from ACAT.Core.logger import config_logger
from ACAT.Core.CoreUtils import global_options


def main():
    """
    Parse the argument and execute the ACAT
    """
    _set_graphviz_path()
    parse_args()

    return_value = 0
    try:
        config_logger(global_options.verbose)
        session = load_session()
        do_analysis(session)

    except Usage as err:
        print(str(err), file=sys.stderr)
        return_value = 2  # signal the error.

    return return_value


def _set_graphviz_path():
    """
    Add Graphviz path, if available, to the right location

    Search PATH environment and add graphviz path to the `sys.path`
    """
    part_of_package_name = 'graphviz'
    for path in sys.path:
        if part_of_package_name in path.lower():
            return

    for path in os.environ['PATH'].split(';'):
        if part_of_package_name in path.lower():
            sys.path.append(path)


if __name__ == '__main__':
    sys.exit(main())
