#pragma once

namespace gou::constants {
    // 0 .. max value of 16bit signed int (uint16/2) scaled into range 0 .. 1.0f
    const float GamePadAxisScaleFactor = (1.0f / float(2>>15));

    const char PathSeparator = '/';
};
