// SPDX-License-Identifier: GPL-2.0-or-later
// fork: GOW3_MSAA=0 renders every guest MSAA surface single-sample. Color, depth and AA-config
// registers report 1 sample and MSAA texture descriptors become plain 2D (array) textures, so
// render targets, pipelines and the shaders that read them all agree on one sample.
#pragma once

#include <cstdlib>

namespace AmdGpu {

inline bool ForceSingleSample() {
    static const bool force = [] {
        const char* env = std::getenv("GOW3_MSAA");
        return env && env[0] == '0';
    }();
    return force;
}

} // namespace AmdGpu
