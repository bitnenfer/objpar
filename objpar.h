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
 * For use it only requires defining op_malloc and op_free. If not
 * it will fallback to stdlib malloc/free.
 *
 * For now it only supports:
 * - Geometric Vertices.
 * - Vertex Normals.
 * - Texture Vertices
 * - Faces
 *
 * Repo: https://github.com/bitnenfer/objpar/
 *
 * -------------------------------------------------------------------------------
 * Wavefront OBJ Format Specification: http://www.martinreddy.net/gfx/3d/OBJ.spec
 */
#ifndef _OBJPAR_H_
#define _OBJPAR_H_

#ifndef op_malloc
#include <stdlib.h>
#define op_malloc malloc
#endif
#ifndef op_free
#include <stdlib.h>
#define op_free free
#endif

typedef struct op_vec2f
{
    float x, y;
} op_vec2f_t;

typedef struct op_vec3f
{
    float x, y, z;
} op_vec3f_t;

typedef struct op_vec4f
{
    float x, y, z, w;
} op_vec4f_t;

typedef struct op_face
{
    unsigned int v[3];
    unsigned int vt[3];
    unsigned int vn[3];
} op_face_t;

typedef struct op_objmesh
{
    op_vec4f_t* p_geo_vert;         /* geometric vertices */
    op_vec3f_t* p_nor_vert;         /* vertex normals */
    op_vec3f_t* p_tex_vert;         /* texture vertices */
    op_face_t* p_faces;             /* faces */
    unsigned int geo_vert_count;
    unsigned int nor_vert_count;
    unsigned int tex_vert_count;
    unsigned int face_count;
} op_objmesh_t;

/* Declaration */
int objpar(const char* p_string, unsigned int string_size, op_objmesh_t* p_out);
void objpar_free(op_objmesh_t* p_out);
int objpar_geo_vert(const char* p_string, unsigned int* p_index, unsigned int string_size, op_vec4f_t* p_vertex);
int objpar_nor_vert(const char* p_string, unsigned int* p_index, unsigned int string_size, op_vec3f_t* p_vertex);
int objpar_tex_vert(const char* p_string, unsigned int* p_index, unsigned int string_size, op_vec3f_t* p_vertex);
int objpar_face(const char* p_string, unsigned int* p_index, unsigned int string_size, op_face_t* p_face);
int objpar_comment(const char* p_string, unsigned int* p_index, unsigned int string_size);
int objpar_newline(const char* p_string, unsigned int* p_index, unsigned int string_size);

/* Definition */
int objpar(const char* p_string, unsigned int string_size, op_objmesh_t* p_out)
{
    unsigned int index;
    unsigned int geo_vert_count;
    unsigned int nor_vert_count;
    unsigned int tex_vert_count;
    unsigned int face_count;
    op_vec4f_t* p_geo_vert;
    op_vec3f_t* p_nor_vert;
    op_vec3f_t* p_tex_vert;
    op_face_t* p_faces;

    if (p_out == NULL)
    {
        return 0;
    }

    index = 0;
    geo_vert_count = 0;
    nor_vert_count = 0;
    tex_vert_count = 0;
    face_count = 0;
    p_geo_vert = NULL;
    p_nor_vert = NULL;
    p_tex_vert = NULL;
    p_faces = NULL;

    /* First count elements to avoid reallocation */
    while (index < string_size)
    {
        if (objpar_geo_vert(p_string, &index, string_size, NULL))
        {
            geo_vert_count += 1;
        }
        else if (objpar_nor_vert(p_string, &index, string_size, NULL))
        {
            nor_vert_count += 1;
        }
        else if (objpar_face(p_string, &index, string_size, NULL))
        {
            face_count += 1;
        }
        else if (objpar_tex_vert(p_string, &index, string_size, NULL))
        {
            tex_vert_count += 1;
        }
        else if (objpar_comment(p_string, &index, string_size));
        else objpar_newline(p_string, &index, string_size);
    }

    /* Allocate our resources */
    if (geo_vert_count > 0)
    {
        p_geo_vert = (op_vec4f_t*)op_malloc(sizeof(struct op_vec4f) * geo_vert_count);
    }
    if (nor_vert_count > 0)
    {
        p_nor_vert = (op_vec3f_t*)op_malloc(sizeof(struct op_vec3f) * nor_vert_count);
    }
    if (tex_vert_count > 0)
    {
        p_tex_vert = (op_vec3f_t*)op_malloc(sizeof(struct op_vec3f) * tex_vert_count);
    }
    if (face_count > 0)
    {
        p_faces = (op_face_t*)op_malloc(sizeof(struct op_face) * face_count);
    }
    
    p_out->p_geo_vert = p_geo_vert;
    p_out->p_nor_vert = p_nor_vert;
    p_out->p_tex_vert = p_tex_vert;
    p_out->p_faces = p_faces;
    p_out->geo_vert_count = geo_vert_count;
    p_out->nor_vert_count = nor_vert_count;
    p_out->tex_vert_count = tex_vert_count;
    p_out->face_count = face_count;

    index = 0;
    geo_vert_count = 0;
    nor_vert_count = 0;
    tex_vert_count = 0;
    face_count = 0;

    while (index < string_size)
    {
        op_face_t f;
        op_vec4f_t v;
        op_vec3f_t vn;
        op_vec3f_t vt;
        
        if (objpar_geo_vert(p_string, &index, string_size, &v))
        {
            p_geo_vert[geo_vert_count++] = v;
        }
        else if (objpar_nor_vert(p_string, &index, string_size, &vn))
        {
            p_nor_vert[nor_vert_count++] = vn;
        }
        else if (objpar_face(p_string, &index, string_size, &f))
        {
            p_faces[face_count++] = f;
        }
        else if (objpar_tex_vert(p_string, &index, string_size, &vt))
        {
            p_tex_vert[tex_vert_count++] = vt;
        }
        else if (objpar_comment(p_string, &index, string_size));
        else objpar_newline(p_string, &index, string_size);
    }
    return 1;
}

void objpar_free(op_objmesh_t* p_out)
{
    if (p_out->p_faces != NULL)
        op_free(p_out->p_faces);
    if (p_out->p_geo_vert != NULL)
        op_free(p_out->p_geo_vert);
    if (p_out->p_nor_vert != NULL)
        op_free(p_out->p_nor_vert);
    if (p_out->p_tex_vert != NULL)
        op_free(p_out->p_tex_vert);

    p_out->p_faces = NULL;
    p_out->p_geo_vert = NULL;
    p_out->p_nor_vert = NULL;
    p_out->p_tex_vert = NULL;

}

int objpar_geo_vert(const char* p_string, unsigned int* p_index, unsigned int string_size, op_vec4f_t* p_vertex)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int str_size;
    char c0;
    char c1;
    float* p_v;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];

    if (c0 == 'v' && c1 == ' ')
    {
        if (p_vertex == NULL)
        {
            objpar_newline(p_string, p_index, string_size);
            return 1;
        }

        comp_count = 0;
        str_size = 0;
        p_v = (float*)p_vertex;

        index += 2;
        p_vertex->x = 0.0f;
        p_vertex->y = 0.0f;
        p_vertex->z = 0.0f;
        p_vertex->w = 1.0f;
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
                comp = strtof(str, NULL); /* TODO: implement custom strtof */
                p_v[comp_count] = comp;
            }
            comp_count += 1;
            if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *p_index = index;
        return 1;
    }
    return 0;
}

int objpar_nor_vert(const char* p_string, unsigned int* p_index, unsigned int string_size, op_vec3f_t* p_vertex)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int str_size;
    char c0;
    char c1;
    float* p_v;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];

    if (c0 == 'v' && c1 == 'n')
    {
        if (p_vertex == NULL)
        {
            objpar_newline(p_string, p_index, string_size);
            return 1;
        }

        comp_count = 0;
        str_size = 0;
        p_v = (float*)p_vertex;

        index += 3;
        p_vertex->x = 0.0f;
        p_vertex->y = 0.0f;
        p_vertex->z = 0.0f;
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
                comp = strtof(str, NULL); /* TODO: implement custom strtof */
                p_v[comp_count] = comp;
            }
            comp_count += 1;
            if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *p_index = index;
        return 1;
    }
    return 0;
}

int objpar_tex_vert(const char* p_string, unsigned int* p_index, unsigned int string_size, op_vec3f_t* p_vertex)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int str_size;
    char c0;
    char c1;
    float* p_v;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];

    if (c0 == 'v' && c1 == 't')
    {
        if (p_vertex == NULL)
        {
            objpar_newline(p_string, p_index, string_size);
            return 1;
        }

        comp_count = 0;
        str_size = 0;
        p_v = (float*)p_vertex;

        index += 3;
        p_vertex->x = 0.0f;
        p_vertex->y = 0.0f;
        p_vertex->z = 0.0f;
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
                comp = strtof(str, NULL); /* TODO: implement custom strtof */
                p_v[comp_count] = comp;
            }
            comp_count += 1;
            if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *p_index = index;
        return 1;
    }
    return 0;
}

int objpar_face(const char* p_string, unsigned int* p_index, unsigned int string_size, op_face_t* p_face)
{
    char str[32];
    unsigned int index;
    unsigned int comp_count;
    unsigned int comp_offset;
    unsigned int str_size;
    char c0;
    char c1;
    unsigned int* p_f;

    index = *p_index;
    c0 = p_string[index];
    c1 = p_string[index + 1];

    if (c0 == 'f' && c1 == ' ')
    {
        if (p_face == NULL)
        {
            objpar_newline(p_string, p_index, string_size);
            return 1;
        }

        comp_count = 0;
        comp_offset = 0;
        str_size = 0;
        p_f = (unsigned int*)p_face;

        index += 2;

        p_face->v[0] = 0;
        p_face->vn[0] = 0;
        p_face->vt[0] = 0;
        p_face->v[1] = 0;
        p_face->vn[1] = 0;
        p_face->vt[1] = 0;
        p_face->v[2] = 0;
        p_face->vn[2] = 0;
        p_face->vt[2] = 0;

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
				if (comp_offset >= 3)
				{
					/* Meshes need to be triangulated */
					return 0;
				}
                str[str_size] = 0;
                p_f[(comp_count * 3) + comp_offset] = atoi(str); /* TODO: implement custom atoi */
            }
            comp_count += 1;
            if (comp_count == 3)
            {
                comp_offset += 1;
                comp_count = 0;
                if (c0 != '\n')
					c0 = p_string[++index];
            }
            else if (c0 == ' ' && comp_count < 2)
            {
                comp_offset += 1;
                comp_count = 0;
                if (c0 != '\n')
					c0 = p_string[++index];
            }
            else if (c0 != '\n')
                c0 = p_string[++index];
            str_size = 0;
        }
        *p_index = index;
        return 1;
    }
    return 0;
}

int objpar_comment(const char* p_string, unsigned int* p_index, unsigned int string_size)
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

int objpar_newline(const char* p_string, unsigned int* p_index, unsigned int string_size)
{
    unsigned int index;
    char c;

    index = *p_index;
    c = p_string[index];

    while (c != '\n' && c != '\r')
    {
        c = p_string[++index];
    }
    *p_index = ++index;
    return 1;
}

#endif /* _OBJPAR_H_ */
