# Vendored Calf DSP headers

These header-only DSP primitives are copied verbatim from the Calf Studio Gear
project (`src/calf/`):

- https://github.com/calf-studio-gear/calf

Files: `primitives.h`, `fixed_point.h`, `delay.h`, `buffer.h`, `onepole.h`

Copyright (C) 2001-2007 Krzysztof Foltman and the Calf Studio Gear authors.

Licensed under the **GNU Lesser General Public License, version 2.1 or later**
(see the per-file notices at the top of each header). LGPL-2.1-or-later is
compatible with this project's GPLv3 license: the "or any later version" clause
permits the combined work to be distributed under GPLv3.

Used by `CalfCompressorPatch.hpp` and `CalfReverbPatch.hpp`. The original Calf
source was referenced via an external path that was never checked in; these
headers were vendored so the patches build standalone.
