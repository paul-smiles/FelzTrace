# Third-Party Licenses and Notices

This project publishes a Docker image based on Ubuntu. The image includes
packages distributed under various open-source licenses.

All original Ubuntu/Debian package licenses are preserved in the container at:

- /usr/share/common-licenses/
- /usr/share/doc/\<package\>/

## GPL/LGPL Source Access Notice

This image contains GPL and LGPL licensed development tools provided by Ubuntu.
In accordance with GPL §3, corresponding source code for these components is
available at:

https://packages.ubuntu.com/
or by running:
    apt source \<package\>

These GPL components are used exclusively for the build process. They do not
form part of the distributed application and do not impose copyleft obligations
on it.

## Application Dependency

### spdlog
- **License:** MIT
- **Upstream:** https://github.com/gabime/spdlog

spdlog includes portions of or depends on the fmt library, licensed under the MIT License.

### fmt
- **License:** MIT
- **Upstream:** https://github.com/fmtlib/fmt

### MIT License Texts

The MIT licenses for spdlog and fmt are included verbatim below.

--- spdlog MIT License ---

The MIT License (MIT)

Copyright (c) 2016 - present, Gabi Melman and spdlog contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

-- NOTE: Third party dependency used by this software --
This software depends on the fmt lib (MIT License),
and users must comply to its license: https://raw.githubusercontent.com/fmtlib/fmt/master/LICENSE

--- fmt MIT License ---

Copyright (c) 2012 - present, Victor Zverovich and {fmt} contributors

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

--- Optional exception to the license ---

As an exception, if, as a result of your compiling your source code, portions
of this Software are embedded into a machine-executable object form of such
source code, you may redistribute such embedded portions in such object form
without including the above copyright and permission notices.