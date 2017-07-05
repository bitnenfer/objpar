objpar
======

Simple Wavefront OBJ parser.
For use it only requires defining op_malloc and op_free. If not
it will fallback to stdlib malloc/free.

For now it only supports:
- Geometric Vertices.
- Vertex Normals.
- Texture Vertices
- Faces

Repo: [https://github.com/bitnenfer/objpar/](https://github.com/bitnenfer/objpar/)

Wavefront OBJ Format Specification: [http://www.martinreddy.net/gfx/3d/OBJ.spec](http://www.martinreddy.net/gfx/3d/OBJ.spec)

```
  Copyright © 2000 Felipe Alfonso <felipe@voidptr.io>
  This work is free. You can redistribute it and/or modify it under the
  terms of the Do What The Fuck You Want To Public License, Version 2,
  as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
```
