#pragma once
bool init_drawing_tool();
GLuint register_vertexs(const int num_vertexs, const vertex * vertexs);
GLuint register_tvertexs(const int num_vertexs, const tvertex *vertexs);
void draw_coord(const Matrix4f &mvp, const float a);
void draw_line(const vertex &sv, const vertex &ev, const Matrix4f &mvp);
void draw_triangle(const vertex * vertexs, const int num_vertexs, const Matrix4f &mvp);
void draw_cube(const float r, const float g, const float b, const Matrix4f &mvp);
void draw_texture(const int num_vertexs, const tvertex *vertexs,
	Mat &texture, const Matrix4f &mvp);
