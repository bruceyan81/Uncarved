# Third-Party Notices

Uncarved uses software and assets from third-party projects.

Software dependencies are managed through vcpkg unless otherwise noted and
are not vendored in this repository.

Direct software dependencies and version pins are declared in `vcpkg.json`.

When dependencies are installed, vcpkg provides their license and copyright
information under:

`<build-dir>/vcpkg_installed/<triplet>/share/<port>/copyright`

Some third-party assets are bundled directly with the project under
`Resources/`. These assets remain subject to their respective license terms,
and applicable license texts are distributed alongside them.

The notices below are provided for attribution and convenience.

## Software Dependencies

### GLM

**OpenGL Mathematics (GLM)**
Version: 1.0.3
Project: https://github.com/g-truc/glm
Copyright: Copyright (c) 2005 - G-Truc Creation
License: MIT License (selected from the upstream dual-license terms)

Upstream license:

https://github.com/g-truc/glm/blob/1.0.3/copying.txt

### RapidJSON

**RapidJSON**
Version: 1.1.0
Project: https://github.com/Tencent/rapidjson
Copyright: Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip
License: MIT License

RapidJSON includes `msinttypes`, which is distributed under the BSD-3-Clause
License.

Upstream license and third-party notices:

https://github.com/Tencent/rapidjson/blob/v1.1.0/license.txt

## Bundled Assets

### Noto Sans

**Noto Sans**
Version: 2.015
Project: notofonts/latin-greek-cyrillic
Release: `NotoSans-v2.015`
Upstream commit: `c4a321e123e4d4ff315f57f4e0adf294fe3a95be`
Upstream file: `NotoSans/googlefonts/variable-ttf/NotoSans[wdth,wght].ttf`
Bundled file: `Resources/Fonts/NotoSans.ttf`
Copyright: Copyright 2022 The Noto Project Authors
License: SIL Open Font License, Version 1.1
Bundled license: `Resources/Fonts/OFL-NotoSans.txt`

The bundled font binary is unmodified from the upstream release; only its
filename has been changed for use within the project's resource layout.

### Cinematic Piano BGM

**Cinematic Piano BGM**
Author: rosko vair
Source: https://roskovair.itch.io/cinematic-piano-bgm
License: Creative Commons Zero v1.0 Universal
