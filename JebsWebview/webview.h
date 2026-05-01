/*
 * Jebs Webview - A lightweight webview library for Linux.
 *
 * Copyright (c) 2026 gm123744
 *
 * Features:
 *  - Webview creation and management
 *  - Content loading
 *  - Configuration and settings support
 *  - Event handling and callbacks
 *
 * License (MIT):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction...
 *
 * (full MIT license text continues here)
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND...
 */

#ifndef WEBVIEW_H
#define WEBVIEW_H

#if defined(_WIN32) || defined(_WIN64)
    #error "Jebs Webview is only supported on Linux."
#endif

#if defined(JVERSION_2_0)
    #include "versions/v2_0Jwebview.h"

#elif defined(JVERSION_2_5)
    #include "versions/v2_5Jwebview.h"

#elif defined(JVERSION_3_0)
    #include "versions/v3_0Jwebview.h"

#else
    #include "versions/v3_0Jwebview.h"
#endif


#endif