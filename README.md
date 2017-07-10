objpar
======

Single header Wavefront OBJ parser.
 
Currently it depends on stdlib's atoi and atof. You can provide your own implementation
by doing 

`#define objpar_atoi my_atoi`
`#define objpar_atof my_atof`

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
