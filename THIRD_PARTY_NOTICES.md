# Third-Party Notices

Uncarved depends on software from third-party projects. These components are
managed through [vcpkg](https://github.com/microsoft/vcpkg) and are not vendored
in this repository.

Direct dependencies and version pins are declared in `vcpkg.json`.

When dependencies are installed, vcpkg provides their license and copyright
information under:

`<build-dir>/vcpkg_installed/<triplet>/share/<port>/copyright`

The notices below are provided for attribution and convenience. Each
third-party component remains subject to its own license terms.

## GLM

**OpenGL Mathematics (GLM)**
Version: 1.0.3
Project: https://github.com/g-truc/glm
Copyright: Copyright (c) 2005 - G-Truc Creation
License: MIT License (selected from the upstream dual-license terms)

Upstream license:

https://github.com/g-truc/glm/blob/1.0.3/copying.txt

## RapidJSON

**RapidJSON**
Version: 1.1.0
Project: https://github.com/Tencent/rapidjson
Copyright: Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip
License: MIT License

RapidJSON includes `msinttypes`, which is distributed under the BSD-3-Clause
License.

Upstream license and third-party notices:

https://github.com/Tencent/rapidjson/blob/v1.1.0/license.txt
