#ifndef SHADER_TESTBED_GL_H_
#define SHADER_TESTBED_GL_H_

#include <stddef.h>

#include <gl/GL.h>

typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;

#define GL_TEXTURE0 0x84C0
#define GL_STATIC_DRAW 0x88E4
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

typedef void (APIENTRY * PFNGLGENBUFFERSPROC)(GLsizei, GLuint *);
typedef void (APIENTRY * PFNGLBINDBUFFERPROC)(GLenum, GLuint);
typedef void (APIENTRY * PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
typedef void (APIENTRY * PFNGLGENVERTEXARRAYSPROC)(GLsizei, GLuint *);
typedef void (APIENTRY * PFNGLBINDVERTEXARRAYPROC)(GLuint);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef GLuint (APIENTRY * PFNGLCREATESHADERPROC)(GLenum);
typedef void (APIENTRY * PFNGLDELETESHADERPROC)(GLuint);
typedef void (APIENTRY * PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const GLchar **, const GLint *);
typedef void (APIENTRY * PFNGLCOMPILESHADERPROC)(GLuint);
typedef void (APIENTRY * PFNGLGETSHADERIVPROC)(GLuint, GLenum, GLint *);
typedef GLuint (APIENTRY * PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRY * PFNGLDELETEPROGRAMPROC)(GLuint);
typedef void (APIENTRY * PFNGLATTACHSHADERPROC)(GLuint, GLuint);
typedef void (APIENTRY * PFNGLLINKPROGRAMPROC)(GLuint);
typedef void (APIENTRY * PFNGLGETPROGRAMIVPROC)(GLuint, GLenum, GLint *);
typedef void (APIENTRY * PFNGLUSEPROGRAMPROC)(GLuint);
typedef GLint (APIENTRY * PFNGLGETUNIFORMLOCATIONPROC)(GLuint, const GLchar *);
typedef void (APIENTRY * PFNGLUNIFORM1FPROC)(GLint, GLfloat);
typedef void (APIENTRY * PFNGLUNIFORM1IPROC)(GLint, GLint);
typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC)(GLenum);

extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;

#endif // SHADER_TESTBED_GL_H_
