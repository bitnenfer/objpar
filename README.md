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

The current implementation defines two structures. The first one is `struct objpar_data` and the second one is `struct objpar_mesh`.

The only function call needed to parse the OBJ string is `objpar`.

 This is the signature of the function:

```C
unsigned int objpar(
    const char* p_string,               /* Raw string */
    unsigned int string_size,           /* String size */
    void* p_buffer,                     /* User allocated buffer with the size provided by objpar_get_size() */
    struct objpar_data* p_data          /* Structure used to store the OBJ parsed data */
);

/* Usage */

struct objpar_data obj_data;

p_data = open_file("my_file.obj", &file_size);

p_buffer = malloc(objpar_get_size(p_data, file_size));

objpar(p_data, file_size, p_buffer, &obj_data);

```

The function `objpar_build_mesh` will generate a flat array containing the vertex data
for the specified `struct objpar_data` structure.

The vertex structure will depend on which parameters are stored on the parsed obj file.
Here is how it would look for a file with position, normals and texture coordinates.

```C
struct vertex
{
    float position[POSITION_WIDTH];
    float texcoord[TEXCOORD_WIDTH];
    float normals[NORMALS_WIDTH];
};

```

The `struct objpar_mesh` structure will provide the data, vertex stride, vertex count, position,
texcoord and normal offsets. With this information you can define your vertex input 
layout for different graphics API. If an offset has a value of -1 it means that it's 
not part of the vertex.

The signature for the function `objpar_build_mesh` is:

```C

unsigned int objpar_build_mesh(
    const struct objpar_data* p_data,       /* Data obtained by objpar function */
    void* p_buffer,                         /* User allocated buffer with the size provided by objpar_get_mesh_size() */
    struct objpar_mesh* p_mesh              /* Structure used to store the mesh data */ 
);

/* Usage */

void* p_mesh_buffer = malloc(objpar_get_mesh_size(&obj_data));

objpar_build_mesh(&obj_data, p_mesh_buffer, &obj_mesh);

```

Repo: [https://github.com/bitnenfer/objpar/](https://github.com/bitnenfer/objpar/)

Wavefront OBJ Format Specification: [http://www.martinreddy.net/gfx/3d/OBJ.spec](http://www.martinreddy.net/gfx/3d/OBJ.spec)

```
  Copyright © 2000 Felipe Alfonso <felipe@voidptr.io>
  This work is free. You can redistribute it and/or modify it under the
  terms of the Do What The Fuck You Want To Public License, Version 2,
  as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
```
