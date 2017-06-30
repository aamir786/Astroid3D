//
//  shader.hpp
//  3d_Astroid
//

#ifndef SHADER_HPP
#define SHADER_HPP

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

GLuint LoadShadersWG(const char *vertPath, const char *geomPath, const char *fragPath, GLchar **varyings, GLsizei varNum);

#endif
