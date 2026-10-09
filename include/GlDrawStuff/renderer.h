#ifndef GL_DRAW_STUFF_RENDERER_H
#define GL_DRAW_STUFF_RENDERER_H

#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef struct Renderer Renderer;

typedef struct {
    GLsizei index_count;
    GLint base_vertex;
    GLintptr index_offset;
} Mesh;

Renderer* renderer_create(GLsizeiptr vertex_buffer_size, GLsizeiptr index_buffer_size, void* shader);
void renderer_destroy(Renderer* renderer);

Mesh renderer_upload(Renderer* renderer, void* shape);
void renderer_begin(Renderer* renderer);
void renderer_draw(Renderer* renderer, void* shape, Mesh mesh);
void renderer_end(Renderer* renderer);



#ifdef __cplusplus
}
#endif



#endif // !GL_DRAW_STUFF_RENDERER_H

