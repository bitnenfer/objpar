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

 The only function call needed to parse the OBJ string is `objpar`.

 This is the signature of the function:

```C
unsigned int objpar(
    const char* p_string,               /* Raw string */
    unsigned int string_size,           /* String size */
    void* p_buffer,                     /* User allocated buffer with the size provided by objpar_get_size() */
    float** pp_vertices,                /* Pointer to a pointer for geometric vertices */
    float** pp_normals,                 /* Pointer to a pointer for vertex normals */
    float** pp_texcoords,               /* Pointer to a pointer for texture coordinates */
    unsigned int** pp_faces,            /* Pointer to a pointer for faces */
    unsigned int* p_vertex_count,       /* Pointer to ouput the vertex count */
    unsigned int* p_normal_count,       /* Pointer to output the normal count */
    unsigned int* p_texcoord_count,     /* Pointer to output the texture coordinate count */
    unsigned int* p_face_count,         /* Pointer to output the face count */
    unsigned int* p_vertex_width,       /* Pointer to output the vertex width (if it's vec2, vec3 or vec4) */
    unsigned int* p_normal_width,       /* Pointer to output the normal width (if it's vec2, vec3 or vec4) */
    unsigned int* p_texcoord_width,     /* Pointer to output the texcoord width (if it's vec2, vec3 or vec4) */
    unsigned int* p_face_width          /* Pointer to output the face width (if it's a triangle or a quad) */
);

/* Usage */

p_data = open_file("my_file.obj", &file_size);

p_buffer = malloc(objpar_get_size(p_data, file_size));

objpar(
    p_data, 
    file_size,
    p_buffer,
    &mesh.p_vertices,
    &mesh.p_normals,
    &mesh.p_texcoords,
    &mesh.p_faces,
    &mesh.vertex_count,
    &mesh.normal_count,
    &mesh.texcoord_count,
    &mesh.face_count,
    &mesh.vertex_width,
    &mesh.normal_width,
    &mesh.texcoord_width,
    &mesh.face_width);

```

Repo: [https://github.com/bitnenfer/objpar/](https://github.com/bitnenfer/objpar/)

Wavefront OBJ Format Specification: [http://www.martinreddy.net/gfx/3d/OBJ.spec](http://www.martinreddy.net/gfx/3d/OBJ.spec)

```
  Copyright © 2000 Felipe Alfonso <felipe@voidptr.io>
  This work is free. You can redistribute it and/or modify it under the
  terms of the Do What The Fuck You Want To Public License, Version 2,
  as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
```
