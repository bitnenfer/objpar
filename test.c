#include "objpar.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ARRAYSIZE(x) (sizeof(x) / sizeof((x)[0]))

void* open_file(const char* p_file_name, size_t* p_file_size);

int main()
{
    unsigned int mesh_index;
    char* files[] = {
        "data/plane.obj",
        "data/cube.obj",
        "data/cube_nontri.obj",
        "data/monkey.obj",
        "data/teapot.obj",
    };

    for (mesh_index = 0; mesh_index < ARRAYSIZE(files); ++mesh_index)
    {
        void* p_data;
        void* p_buffer;
        unsigned int index;
        size_t file_size;
        objpar_data_t obj_data;
        objpar_mesh_t obj_mesh;

        p_data = open_file(files[mesh_index], &file_size);
        /* We need to know the size of the buffer used for 
           storing the data. For that we use objpar_get_size */
        p_buffer = malloc(objpar_get_size(p_data, file_size));
        
        /* objpar doesn't define any struct. It only uses integral type
           You must provide pointers to the different properties supported
           by objpar. */
        objpar((const char*)p_data, file_size, p_buffer, &obj_data);

        free(p_data);

        printf("Geometry Vertices Count: %u\n", obj_data.position_count);
        printf("Vertex Normals Count: %u\n", obj_data.normal_count);
        printf("Texture Vertices Count: %u\n", obj_data.texcoord_count);
        printf("Face Count: %u\n\n", obj_data.face_count);
        
        printf("Geometry Vertices:\n");
        for (index = 0; index < obj_data.position_count * obj_data.position_width; index += obj_data.position_width)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < obj_data.position_width; ++j)
            {
                float v = obj_data.p_positions[index + j];
                printf("%f\t", v);
            }
            putc('\n', stdout);
        }

        printf("\nVertex Normals:\n");
        for (index = 0; index < obj_data.normal_count * obj_data.normal_width; index += obj_data.normal_width)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < obj_data.normal_width; ++j)
            {
                float v = obj_data.p_normals[index + j];
                printf("%f\t", v); 
            }
            putc('\n', stdout);
        }

        printf("\nTexture Vertices:\n");
        for (index = 0; index < obj_data.texcoord_count * obj_data.texcoord_width; index += obj_data.texcoord_width)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < obj_data.texcoord_width; ++j)
            {
                float v = obj_data.p_texcoords[index + j];
                printf("%f\t", v);
            }
            putc('\n', stdout);
        }

        printf("\nFaces:\n");
        for (index = 0; index < obj_data.face_count * obj_data.face_width * 3; index += obj_data.face_width * 3)
        {
            unsigned int j;
            putc('\t', stdout);
            for (j = 0; j < obj_data.face_width * 3; j += 3)
            {
                unsigned int v = obj_data.p_faces[index + j + OBJPAR_V_IDX];
                unsigned int vn = obj_data.p_faces[index + j + OBJPAR_VN_IDX];
                unsigned int vt = obj_data.p_faces[index + j + OBJPAR_VT_IDX];

                printf("%u/%u/%u\t", v, vt, vn);
            }
            putc('\n', stdout);

        }

        /* Build Mesh */
        if (obj_data.face_width == 3)
        {
            unsigned int i;
            unsigned int count;
            void* p_mesh_buffer = malloc(objpar_get_mesh_size(&obj_data));
            assert(objpar_build_mesh(&obj_data, p_mesh_buffer, &obj_mesh));
            
            count = obj_mesh.vertex_count;
            float* p_vertices = (float*)obj_mesh.p_vertices;
            printf("\n\nMesh\n-----\nVertex Count: %u", obj_mesh.vertex_count);
            printf("\nVertex Stride: %u", obj_mesh.vertex_stride);
            printf("\nPosition Byte Offset: %d", obj_mesh.position_offset);
            printf("\nTexCoord Byte Offset: %d", obj_mesh.texcoord_offset);
            printf("\nNormal Byte Offset: %d", obj_mesh.normal_offset);
            printf("\nVertices:\n");
            if (obj_mesh.texcoord_offset == -1 && obj_mesh.normal_offset == -1)
            {
                for (i = 0; i < count; ++i)
                {
                    float* p_vertex = &p_vertices[i * 3];
                    printf("\t{ pos_x: %f pos_y: %f pos_z: %f }\n", p_vertex[0], p_vertex[1], p_vertex[2]);
                }
            }
            else if (obj_mesh.texcoord_offset == -1 && obj_mesh.normal_offset > -1)
            {
                for (i = 0; i < count; ++i)
                {
                    float* p_vertex = &p_vertices[i * 6];
                    printf("\t{ pos_x: %f pos_y: %f pos_z: %f", p_vertex[0], p_vertex[1], p_vertex[2]);
                    printf(" nor_x: %f nor_y: %f nor_z: %f }\n", p_vertex[3], p_vertex[4], p_vertex[5]);
                }
            }
            else if (obj_mesh.texcoord_offset > -1 && obj_mesh.normal_offset == -1)
            {
                for (i = 0; i < count; ++i)
                {
                    float* p_vertex = &p_vertices[i * 6];
                    printf("\t{ pos_x: %f pos_y: %f pos_z: %f", p_vertex[0], p_vertex[1], p_vertex[2]);
                    printf(" tc_x: %f tc_y: %f tc_z: %f }\n", p_vertex[3], p_vertex[4], p_vertex[5]);
                }
            }
            else if (obj_mesh.texcoord_offset > -1 && obj_mesh.normal_offset > -1)
            {
                for (i = 0; i < count; ++i)
                {
                    float* p_vertex = &p_vertices[i * 9];
                    printf("\t{ pos_x: %f pos_y: %f pos_z: %f", p_vertex[0], p_vertex[1], p_vertex[2]);
                    printf(" tc_x: %f tc_y: %f tc_z: %f", p_vertex[3], p_vertex[4], p_vertex[5]);
                    printf(" nor_x: %f nor_y: %f nor_z: %f }\n", p_vertex[6], p_vertex[7], p_vertex[8]);
                }
            }
            free(p_mesh_buffer);
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
