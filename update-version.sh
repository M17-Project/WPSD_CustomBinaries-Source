#!/bin/bash

# update version date str.
find . -name Version.h -exec sed -i -e "/const char\* VERSION =/ s/\"[^\"]*\"/\"$(date +'%Y%m%d')_WPSD\"/" -e "/const wxString VERSION =/ s/wxT(\"[^\"]*\");/wxT(\"$(date +'%Y%m%d')_WPSD\");/" {} \;
