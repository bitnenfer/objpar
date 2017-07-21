/**
*
*      Copyright © 2000 Felipe Alfonso <felipe@voidptr.io>
*      This work is free. You can redistribute it and/or modify it under the
*      terms of the Do What The Fuck You Want To Public License, Version 2,
*      as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*
*
* objpar
* ======
*
* Simple Wavefront OBJ parser.
* 
* Currently it depends on stdlib's atoi and atof. You can provide your own implementation
* by doing 
*
* `#define objpar_atoi my_atoi`
* `#define objpar_atof my_atof`
*
* For now it only supports:
* - Geometric Vertices.
* - Vertex Normals.
* - Texture Vertices
* - Faces
*
* The function objpar_build_mesh will generate a flat array containing the vertex data
* for the specified objpar_data structure.
*
* The vertex structure will depend on which parameters are stored on the parsed obj file.
* Here is how it would look for a file with position, normals and texture coordinates.
* 
* struct vertex
* {
*     float position[POSITION_WIDTH];
*     float texcoord[TEXCOORD_WIDTH];
*     float normals[NORMALS_WIDTH];
* };
* 
* The objpar_mesh structure will provide the data, vertex stride, vertex count, position,
* texcoord and normal offsets. With this information you can define your vertex input 
* layout for different graphics API. If an offset has a value of -1 it means that it's 
* not part of the vertex.
* 
*
* Repo: https://github.com/bitnenfer/objpar/
*
* -------------------------------------------------------------------------------
* Wavefront OBJ Format Specification: http://www.martinreddy.net/gfx/3d/OBJ.spec
*/

#ifndef _OBJPAR_H_
#define _OBJPAR_H_

#if __cplusplus
extern "C" 
{
#endif

#if !defined(objpar_atoi) || !defined(objpar_atof)
#include <stdlib.h>
#define objpar_atoi atoi
#define objpar_atof (float)atof
#endif

#define OBJPAR_NULL(type) ((type*)0)

#define OBJPAR_V_IDX 0
#define OBJPAR_VT_IDX 1
#define OBJPAR_VN_IDX 2
#define objpar_get_size(string, string_size) objpar((const char*)string, string_size, NULL, NULL)
#define objpar_get_mesh_size(obj_data) objpar_build_mesh(obj_data, NULL, NULL)

typedef struct objpar_data
{
    /* Data */
    float* p_positions;
    float* p_texcoords;
    float* p_normals;
    unsigned int* p_faces;
    
    /* Sizes */
    unsigned int position_count;
    unsigned int normal_count;
    unsigned int texcoord_count;
    unsigned int face_count;
    unsigned int position_width;
    unsigned int normal_width;
    unsigned int texcoord_width;
    unsigned int face_width;

} objpar_data_t;

typedef struct objpar_mesh
{
    void* p_vertices;
    unsigned int vertex_count;
    unsigned int vertex_stride;
    int position_offset;
    int texcoord_offset;
    int normal_offset;
} objpar_mesh_t;

/* Declaration */
static unsigned int objpar(const char* p_string, unsigned int string_size, void* p_buffer, struct objpar_data* p_data);
static unsigned int objpar_build_mesh(const struct objpar_data* p_data, void* p_buffer, struct objpar_mesh* p_mesh);
static unsigned int objpar_internal_v(const char* p_string, unsigned int* p_index, unsigned int string_size, float** pp_vbuff, unsigned int vertex_width);
static unsigned int objpar_internal_vn(const char* p_string, unsigned int* p_index, unsigned int string_size, float** pp_nbuff, unsigned int normal_width);
static unsigned int objpar_internal_vt(const char* p_string, unsigned int* p_index, unsigned int string_size, float** pp_tbuff, unsigned int texcoord_width);
static unsigned int objpar_internal_f(const char* p_string, unsigned int* p_index, unsigned int string_size, unsigned int** pp_fbuff, unsigned int face_width);
static unsigned int objpar_internal_comment(const char* p_string, unsigned int* p_index, unsigned int string_size);
static unsigned int objpar_internal_newline(const char* p_string, unsigned int* p_index, unsigned int string_size, unsigned int* p_space_count);

/* Definition */
unsigned int objpar(const char* p_string, unsigned int string_size, void* p_buffer, struct objpar_data* p_data)
{
    unsigned int index;
    unsigned int vertex_count;
    unsigned int normal_count;
    unsigned int texcoord_count;
    unsigned int face_count;
    unsigned int vertex_width;
    unsigned int normal_width;
    unsigned int texcoord_width;
    unsigned int face_width;
    unsigned int vertex_buffer_size;
    unsigned int normal_buffer_size;
    unsigned int texcoord_buffer_size;
    unsigned int face_buffer_size;
    unsigned int total_buffer_size;
    unsigned int face_comp_count;
    float* p_vertices;
    float* p_normals;
    float* p_texcoords;
    unsigned int* p_faces;
    void* p_curr_buffer;

    index = 0;
    vertex_count = 0;
    normal_count = 0;
    texcoord_count = 0;
    face_count = 0;
    vertex_width = 0;
    normal_width= 0;
    texcoord_width = 0;
    face_width= 0;
    total_buffer_size = 0;
    face_comp_count = 3;
    p_vertices = OBJPAR_NULL(float);
    p_normals = OBJPAR_NULL(float);
    p_texcoords = OBJPAR_NULL(float);
    p_faces = OBJPAR_NULL(unsigned int);
    p_curr_buffer = OBJPAR_NULL(void);

    /* First count elements to avoid reallocation */
    while (index < string_size)
    {
        unsigned int count;

        if ((count = objpar_internal_v(p_string, &index, string_size, OBJPAR_NULL(float*), 0)))
        {
            vertex_count += 1;
            vertex_width = count;
        }
        else if ((count = objpar_internal_vn(p_string, &index, string_size, OBJPAR_NULL(float*), 0)))
        {
            normal_count += 1;
            normal_width = count;
        }
        else if ((count = objpar_internal_vt(p_string, &index, string_size, OBJPAR_NULL(float*), 0)))
        {
            texcoord_count += 1;
            texcoord_width = count;
        }
        else if ((count = objpar_internal_f(p_string, &index, string_size, OBJPAR_NULL(unsigned int*), 0)))
        {
            face_count += 1;
            face_width = count;
        }
        else if (objpar_internal_comment(p_string, &index, string_size));
        else objpar_internal_newline(p_string, &index, string_size, 0);
    }

    vertex_buffer_size = (sizeof(float) * vertex_width) * vertex_count;
    normal_buffer_size = (sizeof(float) * normal_width) * normal_count;
    texcoord_buffer_size = (sizeof(float) * texcoord_width) * texcoord_count;
    face_buffer_size = ((sizeof(unsigned int) * face_comp_count) * face_width) * face_count;

    total_buffer_size = (vertex_buffer_size + normal_buffer_size + texcoord_buffer_size + face_buffer_size);

    if (p_buffer == OBJPAR_NULL(void) ||
        p_data == OBJPAR_NULL(void))
    {
        return total_buffer_size;
    }

    if (total_buffer_size == 0)
    {
        return 0;
    }

    p_curr_buffer = p_buffer;

    if (vertex_count > 0)
    {
        p_vertices = (float*)p_curr_buffer;
        p_curr_buffer = (void*)((char*)p_curr_buffer + vertex_buffer_size);
    }
    if (normal_count > 0)
    {
        p_normals = (float*)p_curr_buffer;
        p_curr_buffer = (void*)((char*)p_curr_buffer + normal_buffer_size);
    }
    if (texcoord_count > 0)
    {
        p_texcoords = (float*)p_curr_buffer;
        p_curr_buffer = (void*)((char*)p_curr_buffer + texcoord_buffer_size);
    }
    if (face_count > 0)
    {
        p_faces = (unsigned int*)p_curr_buffer;
        p_curr_buffer = (void*)((char*)p_curr_buffer + face_buffer_size);
    }

    p_data->p_positions = p_vertices;
    p_data->p_normals = p_normals;
    p_data->p_texcoords = p_texcoords;
    p_data->p_faces = p_faces;
    p_data->position_count = vertex_count;
    p_data->normal_count = normal_count;
    p_data->texcoord_count = texcoord_count;
    p_data->face_count = face_count;
    p_data->position_width = vertex_width;
    p_data->normal_width = normal_width;
    p_data->texcoord_width = texcoord_width;
    p_data->face_width = face_width;

    index = 0;
    vertex_count = 0;
    normal_count = 0;
    texcoord_count = 0;
    face_count = 0;

    while (index < string_size)
    {
        if (objpar_internal_v(p_string, &index, string_size, &p_vertices, vertex_width));
        else if (objpar_internal_vn(p_string, &index, string_size, &p_normals, normal_width));
        else if (objpar_internal_vt(p_string, &index, string_size, &p_texcoords, texcoord_width));
        else if (objpar_internal_f(p_string, &index, string_size, &p_faces, face_width));
        else if (objpar_internal_comment(p_string, &index, string_size));
        else objpar_internal_newline(p_string, &index, string_size, OBJPAR_NULL(unsigned int));
    }
    return 1;
}

unsigned int objpar_build_mesh(const struct objpar_data* p_data, void* p_buffer, struct objpar_mesh* p_mesh)
{
    unsigned int* p_faces;
    unsigned int offset_size;
    unsigned int stride;
    unsigned int position_count;
    unsigned int texcoord_count;
    unsigned int normal_count;
    unsigned int position_width;
    unsigned int texcoord_width;
    unsigned int normal_width;
    unsigned int face_width;
    unsigned int component_offset;
    unsigned int vertex_component_count;
    unsigned int vertex_count;
    unsigned int index;
    void* p_current;
    float* p_positions;
    float* p_texcoords;
    float* p_normals;

    if (p_data->face_width != 3)
    {
        /* To build a mesh this function requires the obj file to have
        triangulated faces in advance. */
        return 0;
    }

    position_count = p_data->position_count;
    texcoord_count = p_data->texcoord_count;
    normal_count = p_data->normal_count;
    position_width = p_data->position_width;
    texcoord_width = p_data->texcoord_width;
    normal_width = p_data->normal_width;
    face_width = p_data->face_width;

    vertex_component_count = (position_count > 0 ? 1 : 0) + (texcoord_count > 0 ? 1 : 0) + (normal_count > 0 ? 1 : 0);
    offset_size = (position_count > 0 ? position_width : 0) + (texcoord_count > 0 ? texcoord_width : 0) + (normal_count > 0 ? normal_width : 0);
    stride = offset_size * sizeof(float);
    vertex_count = p_data->face_count;

    if (p_buffer == OBJPAR_NULL(void) ||
        p_mesh == OBJPAR_NULL(void))
    {
        return stride * face_width * vertex_count;
    }

    component_offset = 0;
    p_faces = p_data->p_faces;

    p_mesh->vertex_stride = stride;
    p_mesh->position_offset = -1;
    p_mesh->texcoord_offset = -1;
    p_mesh->normal_offset = -1;
    p_mesh->p_vertices = p_buffer;
    p_mesh->vertex_count = vertex_count * face_width;
    
    if (position_count > 0)
    {
        p_mesh->position_offset = component_offset++ * p_data->position_width * sizeof(float);
    }
    if (texcoord_count > 0)
    {
        p_mesh->texcoord_offset = component_offset++ * p_data->texcoord_width * sizeof(float);
    }
    if (normal_count > 0)
    {
        p_mesh->normal_offset = component_offset++ * p_data->normal_width * sizeof(float);
    }

    p_current = p_buffer;
    p_positions = p_data->p_positions;
    p_texcoords = p_data->p_texcoords;
    p_normals = p_data->p_normals;

    {
        unsigned int count = vertex_count * face_width * 3;
        for (index = 0; index < count; index += 3)
        {
            if (position_count > 0)
            {
                unsigned int idx;
                unsigned int j;
                float* p_position = (float*)p_current;

                idx = (p_faces[index + OBJPAR_V_IDX] - 1);
                idx *= position_width;

                for (j = 0; j < position_width; ++j)
                {
                    p_position[j] = p_positions[idx + j];
                }

                p_current = (void*)(p_position + position_width);
            }
            if (texcoord_count > 0)
            {
                unsigned int idx;
                unsigned int j;
                float* p_texcoord = (float*)p_current;

                idx = (p_faces[index + OBJPAR_VT_IDX] - 1) * texcoord_width;

                for (j = 0; j < texcoord_width; ++j)
                {
                    p_texcoord[j] = p_texcoords[idx + j];
                }

                p_current = (void*)(p_texcoord + texcoord_width);
            }
            if (normal_count > 0)
            {
                unsigned int idx;
                unsigned int j;
                float* p_normal = (float*)p_current;

                idx = (p_faces[index + OBJPAR_VN_IDX] - 1) * normal_width;

                for (j = 0; j < normal_width; ++j)
                {
                    p_normal[j] = p_normals[idx + j];
                }

                p_current = (void*)(p_normal + normal_width);
            }
        }

    }

    return 1;
}

unsigned int objpar_internal_v(const char* p_string, unsigned int* p_index, unsigned int string_size, float** pp_vbuff, unsigned int vertex_width)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int str_size;
    unsigned int i;
    char c0;
    char c1;
    float* p_vertex;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];

    if (c0 == 'v' && c1 == ' ')
    {
        if (pp_vbuff == OBJPAR_NULL(float*))
        {
            unsigned int space_count = 0;
            objpar_internal_newline(p_string, p_index, string_size, &space_count);
            return space_count;
        }

        p_vertex = *pp_vbuff;
        comp_count = 0;
        str_size = 0;

        index += 2;
        for (i = 0; i < vertex_width; ++i)
        {
            p_vertex[i] = 0.0f;
        }
        c0 = p_string[index];

        while (c0 != '\n' && c0 != '\r')
        {
            while (c0 > 0x2C && c0 < 0x3A)
            {
                str[str_size++] = c0;
                c0 = p_string[++index];
            }
            if (str_size > 0)
            {
                float comp;
                str[str_size] = 0;
                comp = objpar_atof(str); /* TODO: implement custom objpar_atof */
                p_vertex[comp_count] = comp;
            }
            comp_count += 1;
            if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *pp_vbuff = p_vertex + vertex_width;
        *p_index = index;
        return 1;
    }
    return 0;
}

unsigned int objpar_internal_vn(const char* p_string, unsigned int* p_index, unsigned int string_size, float** pp_nbuff, unsigned int normal_width)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int str_size;
    unsigned int i;
    char c0;
    char c1;
    float* p_normal;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];

    if (c0 == 'v' && c1 == 'n')
    {
        if (pp_nbuff == OBJPAR_NULL(float*))
        {
            unsigned int space_count = 0;
            objpar_internal_newline(p_string, p_index, string_size, &space_count);
            return space_count;
        }

        p_normal = *pp_nbuff;
        comp_count = 0;
        str_size = 0;

        index += 3;
        for (i = 0; i < normal_width; ++i)
        {
            p_normal[i] = 0.0f;
        }
        c0 = p_string[index];

        while (c0 != '\n' && c0 != '\r')
        {
            while (c0 > 0x2C && c0 < 0x3A)
            {
                str[str_size++] = c0;
                c0 = p_string[++index];
            }
            if (str_size > 0)
            {
                float comp;
                str[str_size] = 0;
                comp =  objpar_atof(str); /* TODO: implement custom objpar_atof */
                p_normal[comp_count] = comp;
            }
            comp_count += 1;
            if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *pp_nbuff = p_normal + normal_width;
        *p_index = index;
        return 1;
    }
    return 0;
}

unsigned int objpar_internal_vt(const char* p_string, unsigned int* p_index, unsigned int string_size, float** pp_tbuff, unsigned int texcoord_width)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int str_size;
    unsigned int i;
    char c0;
    char c1;
    float* p_texcoord;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];

    if (c0 == 'v' && c1 == 't')
    {
        if (pp_tbuff == OBJPAR_NULL(float*))
        {
            unsigned int space_count = 0;
            objpar_internal_newline(p_string, p_index, string_size, &space_count);
            return space_count;
        }

        p_texcoord = *pp_tbuff;
        comp_count = 0;
        str_size = 0;

        index += 3;
        for (i = 0; i < texcoord_width; ++i)
        {
            p_texcoord[i] = 0.0f;
        }
        c0 = p_string[index];

        while (c0 != '\n' && c0 != '\r')
        {
            while (c0 > 0x2C && c0 < 0x3A)
            {
                str[str_size++] = c0;
                c0 = p_string[++index];
            }
            if (str_size > 0)
            {
                float comp;
                str[str_size] = 0;
                comp = objpar_atof(str); /* TODO: implement custom objpar_atof */
                p_texcoord[comp_count] = comp;
            }
            comp_count += 1;
            if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *pp_tbuff = p_texcoord + texcoord_width;
        *p_index = index;
        return 1;
    }
    return 0;
}

unsigned int objpar_internal_f(const char* p_string, unsigned int* p_index, unsigned int string_size, unsigned int** pp_fbuff, unsigned int face_width)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int comp_offset;
    unsigned int str_size;
    unsigned int face_comp_count;
    unsigned int i;
    char c0;
    char c1;
    unsigned int* p_face;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];
    face_comp_count = 3;
    if (c0 == 'f' && c1 == ' ')
    {
        if (pp_fbuff == OBJPAR_NULL(unsigned int*))
        {
            unsigned int space_count = 0;
            objpar_internal_newline(p_string, p_index, string_size, &space_count);
            if (space_count < 3)
                space_count = 3;
            return space_count;
        }

        p_face = *pp_fbuff;
        comp_count = 0;
        comp_offset = 0;
        str_size = 0;

        index += 2;

        for (i = 0; i < face_width * face_comp_count; i += 3)
        {
            p_face[i + 0] = 0;
            p_face[i + 1] = 0;
            p_face[i + 2] = 0;
        }

        c0 = p_string[index];
        while (c0 != '\n' && c0 != '\r')
        {
            while (c0 > 0x2F && c0 < 0x3A)
            {
                str[str_size++] = c0;
                c0 = p_string[++index];
            }
            if (str_size > 0)
            {
                str[str_size] = 0;
                p_face[comp_count] = objpar_atoi(str);
                str_size = str_size;
            }
            comp_count += 1;
            if (c0 != '/' && comp_count % 3 != 0 && comp_count < face_width * face_comp_count)
            {
                comp_count += 2;
                c0 = p_string[++index];
            }
            else if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *p_index = index;
        *pp_fbuff = p_face + (face_width * face_comp_count);
        return 1;
    }
    return 0;
}

unsigned int objpar_internal_comment(const char* p_string, unsigned int* p_index, unsigned int string_size)
{
    unsigned int index;
    char c;

    index = *p_index;
    c = p_string[index];

    if (c == '#')
    {
        while (c != '\n' && c != '\r')
        {
            c = p_string[++index];
        }
        *p_index = ++index;
        return 1;
    }
    return 0;
}

unsigned int objpar_internal_newline(const char* p_string, unsigned int* p_index, unsigned int string_size, unsigned int* p_space_count)
{
    unsigned int index;
    unsigned int space_count;
    char c;

    space_count = 0;
    index = *p_index;
    c = p_string[index];

    while (c != '\n' && c != '\r')
    {
        if (c == ' ' || c == '\t')
            space_count += 1;
        c = p_string[++index];
    }
    *p_index = ++index;
    if (p_space_count != OBJPAR_NULL(unsigned int))
        *p_space_count = space_count;
    
    return 1;
}

#if __cplusplus
}
#endif

#endif /* _OBJPAR_H_ */
