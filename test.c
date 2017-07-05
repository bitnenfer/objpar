#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "objpar.h"

void* open_file(const char* p_file_name, size_t* p_file_size);

int main()
{
    unsigned int mesh_index;
    char* files[3];

    files[0] = "data/cube.obj";
    files[1] = "data/monkey.obj";
    files[2] = "data/teapot.obj";

    for (mesh_index = 0; mesh_index < 3; ++mesh_index)
    {
        void* p_data;
        op_objmesh_t mesh;
        unsigned int index;
        size_t file_size;
        p_data = open_file(files[mesh_index], &file_size);
        objpar(p_data, file_size, &mesh);
        free(p_data);

        printf("Geometry Vertices Count: %u\n", mesh.geo_vert_count);
        printf("Vertex Normals Count: %u\n", mesh.nor_vert_count);
        printf("Texture Vertices Count: %u\n", mesh.tex_vert_count);
        printf("Faces Count: %u\n\n", mesh.face_count);

        printf("Geometry Vertices:\n");
        for (index = 0; index < mesh.geo_vert_count; ++index)
        {
            op_vec4f_t v = mesh.p_geo_vert[index];
            printf("\t%f\t%f\t%f\t%f\n", v.x, v.y, v.z, v.w);
        }

        printf("\nVertex Normals:\n");
        for (index = 0; index < mesh.nor_vert_count; ++index)
        {
            op_vec3f_t v = mesh.p_nor_vert[index];
            printf("\t%f\t%f\t%f\n", v.x, v.y, v.z);
        }

        printf("\nTexture Vertices:\n");
        for (index = 0; index < mesh.tex_vert_count; ++index)
        {
            op_vec3f_t v = mesh.p_tex_vert[index];
            printf("\t%f\t%f\t%f\n", v.x, v.y, v.z);
        }

        printf("\nFaces:\n");
        for (index = 0; index < mesh.face_count; ++index)
        {
            op_face_t f = mesh.p_faces[index];
            printf("\t%u|%u|%u %u|%u|%u %u|%u|%u\n",
                   f.v[0], f.vt[0], f.vn[0],
                   f.v[1], f.vt[1], f.vn[1],
                   f.v[2], f.vt[2], f.vn[2]
            );
        }

        objpar_free(&mesh);
    }

    return 0;
}

void* open_file(const char* p_file_name, size_t* p_file_size)
{
    FILE* p_file;
    void* p_file_data;
    size_t size;
    int err;

#if defined(_MSC_VER)
    fopen_s(&p_file, p_file_name, "rb");
#else
    p_file = fopen(p_file_name, "rb+");
#endif
    assert(p_file != NULL);
    fseek(p_file, 0L, SEEK_END);
    size = ftell(p_file);
    rewind(p_file);
    p_file_data = malloc(size);
#if defined(_MSC_VER)
    fread_s(p_file_data, size, size, 1, p_file);
#else
    fread(p_file_data, size, 1, p_file);
#endif
    err = ferror(p_file);
    assert(err == 0);
    fclose(p_file);
    *p_file_size = size;
    return p_file_data;
}
