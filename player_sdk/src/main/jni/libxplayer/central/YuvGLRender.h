//
// Created by chris on 10/20/16.
//

#ifndef XPLAYER_YUVGLRENDER_H
#define XPLAYER_YUVGLRENDER_H

extern "C" {

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

} // end of extern C

#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "MediaFile.h"

static const char* FRAG_SHADER =
    "varying lowp vec2 tc;\n"
    "uniform sampler2D SamplerY;\n"
    "uniform sampler2D SamplerU;\n"
    "uniform sampler2D SamplerV;\n"
    "void main(void)\n"
    "{\n"
        "mediump vec3 yuv;\n"
        "lowp vec3 rgb;\n"
        "yuv.x = texture2D(SamplerY, tc).r;\n"
        "yuv.y = texture2D(SamplerU, tc).r - 0.5;\n"
        "yuv.z = texture2D(SamplerV, tc).r - 0.5;\n"
        "rgb = mat3( 1,   1,   1,\n"
                    "0,       -0.39465,  2.03211,\n"
                    "1.13983,   -0.58060,  0) * yuv;\n"
        "gl_FragColor = vec4(rgb, 1);\n"
    "}\n";

static const char* VERTEX_SHADER =
      "attribute vec4 vPosition;    \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 tc;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = vPosition;  \n"
      "   tc = a_texCoord;  \n"
      "}                            \n";


enum {
    ATTRIB_VERTEX,
    ATTRIB_TEXTURE,
};


static GLfloat squareVertices[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f,  1.0f,
    1.0f,  1.0f,
};

static GLfloat coordVertices[] = {
    -1.0f, 1.0f,
    1.0f, 1.0f,
    -1.0f,  -1.0f,
    1.0f,  -1.0f,
};

class YuvGLRender{

public:
    YuvGLRender(MediaFile *mediaFile);
    ~YuvGLRender();

    /**
     * render the source frame.
     */
    //void render_frame(AVFrame *src_frame);
    void render_frame();


    /**
     * initialize .
     */
    void init();

    /**
     * build opengl program
     *
     * @param vertexShaderSource
     *              vertex shader source code
     * @param fragmentShaderSource
     *              fragment shader source code
     */
    GLuint buildProgram(const char* vertexShaderSource,
            const char* fragmentShaderSource);

    /**
     * media file handle
     */
    MediaFile *mediaFileHandle;


private:

    /**
     * compile shader source code
     *
     * @param texture_data
     *              texture data array
     */
    GLuint buildShader(const char* source, GLenum shaderType);

    /**
     * check opengl es error
     *
     * @param texture_data
     *              texture data array
     */
    GLuint bindTexture(GLuint texture, uint8_t *texture_data, GLuint w , GLuint h);

    /**
     * check opengl es error
     */
    void checkGlError(const char* op);


    /**
     *  y data texture object
     */
    GLuint textureYId;

    /**
     *  u data texture object
     */
    GLuint textureUId;

    /**
     *  v data texture object
     */
    GLuint textureVId;

    /**
     * gl es program handle
     */
    GLuint simpleProgram;


};

#endif //XPLAYER_YUVGLRENDER_H
