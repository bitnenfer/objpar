#include "objpar.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void* open_file(const char* p_file_name, size_t* p_file_size);

typedef struct simple_mesh
{
    float* p_vertices;
    float* p_normals;
    float* p_texcoords;
    unsigned int* p_faces;
    unsigned int vertex_count;
    unsigned int normal_count;
    unsigned int texcoord_count;
    unsigned int face_count;
    unsigned int vertex_width;
    unsigned int normal_width;
    unsigned int texcoord_width;
    unsigned int face_width;
} simple_mesh_t;

int main()
{
    unsigned int mesh_index;
    char* files[4];

    files[0] = "data/cube.obj";
    files[1] = "data/cube_nontri.obj";
    files[2] = "data/monkey.obj";
    files[3] = "data/teapot.obj";

    for (mesh_index = 0; mesh_index < 4; ++mesh_index)
    {
        void* p_data;
        void* p_buffer;
        unsigned int index;
        size_t file_size;
        simple_mesh_t mesh;

        p_data = open_file(files[mesh_index], &file_size);
        /* We need to know the size of the buffer used for 
           storing the data. For that we use objpar_get_size */
        p_buffer = malloc(objpar_get_size(p_data, file_size));
        
        /* objpar doesn't define any struct. It only uses integral type
           You must provide pointers to the different properties supported
           by objpar. */
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

        free(p_data);

        printf("Geometry Vertices Count: %u\n", mesh.vertex_count);
        printf("Vertex Normals Count: %u\n", mesh.normal_count);
        printf("Texture Vertices Count: %u\n", mesh.texcoord_count);
        printf("Face Count: %u\n\n", mesh.face_count);
        
        printf("Geometry Vertices:\n");
        for (index = 0; index < mesh.vertex_count * mesh.vertex_width; index += mesh.vertex_width)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < mesh.vertex_width; ++j)
            {
                float v = mesh.p_vertices[index + j];
                printf("%f\t", v);
            }
            putc('\n', stdout);
        }

        printf("\nVertex Normals:\n");
        for (index = 0; index < mesh.normal_count * mesh.normal_width; index += mesh.normal_width)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < mesh.normal_width; ++j)
            {
                float v = mesh.p_normals[index + j];
                printf("%f\t", v);
            }
            putc('\n', stdout);
        }

        printf("\nTexture Vertices:\n");
        for (index = 0; index < mesh.texcoord_count * mesh.texcoord_width; index += mesh.texcoord_width)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < mesh.texcoord_width; ++j)
            {
                float v = mesh.p_texcoords[index + j];
                printf("%f\t", v);
            }
            putc('\n', stdout);
        }

        printf("\nFaces:\n");
        for (index = 0; index < mesh.face_count * mesh.face_width * 3; index += mesh.face_width * 3)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < mesh.face_width * 3; j += 3)
            {
                unsigned int v = mesh.p_faces[index + j + OBJPAR_V_IDX];
                unsigned int vn = mesh.p_faces[index + j + OBJPAR_VN_IDX];
                unsigned int vt = mesh.p_faces[index + j + OBJPAR_VT_IDX];

                printf("%u/%u/%u\t", v, vt, vn);
            }
            putc('\n', stdout);

        }
        free(p_buffer);
    }
    getchar();
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
