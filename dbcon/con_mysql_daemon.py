#! /usr/bin/env python3

import daemon

from con_mysql import main

with daemon.DaemonContext():
    main()
