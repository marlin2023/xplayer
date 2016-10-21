//
// Created by chris on 10/20/16.
//

#define TAG "YUV_GL_RENDER"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "YuvGLRender.h"

#include "util/XLog.h"

YuvGLRender::YuvGLRender(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;

    this->init();
}

YuvGLRender::~YuvGLRender()
{


}

void YuvGLRender::init()
{
    simpleProgram = buildProgram(VERTEX_SHADER, FRAG_SHADER);
    glUseProgram(simpleProgram);
    XLog::d(ANDROID_LOG_WARN ,TAG ,"==>simpleProgram:%d\n", simpleProgram);

    glGenTextures(1, &textureYId);
    XLog::d(ANDROID_LOG_WARN ,TAG ,"==>textureYId:%d\n", textureYId);
    glGenTextures(1, &textureUId);
    XLog::d(ANDROID_LOG_WARN ,TAG ,"==>textureUId:%d\n", textureUId);
    glGenTextures(1, &textureVId);
    XLog::d(ANDROID_LOG_WARN ,TAG ,"==>textureVId:%d\n", textureVId);

}


//void YuvGLRender::render_frame(AVFrame *src_frame)
void YuvGLRender::render_frame()
{
    AVFrame *src_frame;
    src_frame = av_frame_alloc();
    // TODO
    this->mediaFileHandle->video_frame_queue->get(src_frame);
    XLog::d(ANDROID_LOG_WARN ,TAG ,"==>video frame queue size :%d\n", this->mediaFileHandle->video_frame_queue->size());

    // src_frame->data[0]
    bindTexture(textureYId, src_frame->data[0], src_frame->linesize[0], src_frame->height); // first parameter use linesize.
    // src_frame->data[1]
    bindTexture(textureUId, src_frame->data[1], src_frame->linesize[1], src_frame->height/2);
    // src_frame->data[2]
    bindTexture(textureVId, src_frame->data[2], src_frame->linesize[2], src_frame->height/2);

    //
    glClearColor(1.0f, 0.0f, 0.0f, 1);
    checkGlError("glClearColor");
    glClear(GL_COLOR_BUFFER_BIT);

    GLint tex_y = glGetUniformLocation(simpleProgram, "SamplerY");
    GLint tex_u = glGetUniformLocation(simpleProgram, "SamplerU");
    GLint tex_v = glGetUniformLocation(simpleProgram, "SamplerV");
    checkGlError("glGetUniformLocation");

    glBindAttribLocation(simpleProgram, ATTRIB_VERTEX, "vPosition");
    glBindAttribLocation(simpleProgram, ATTRIB_TEXTURE, "a_texCoord");
    checkGlError("glBindAttribLocation");

    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, textureYId);
    checkGlError("glBindTexture");
    glUniform1i(tex_y, 0);
    checkGlError("glUniform1i");

    glActiveTexture(GL_TEXTURE1);
    checkGlError("glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, textureUId);
    checkGlError("glBindTexture");
    glUniform1i(tex_u, 1);
    checkGlError("glUniform1i");

    glActiveTexture(GL_TEXTURE2);
    checkGlError("glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, textureVId);
    checkGlError("glBindTexture");
    glUniform1i(tex_v, 2);
    checkGlError("glUniform1i");

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    checkGlError("glDrawArrays");

}

GLuint YuvGLRender::buildShader(const char* source, GLenum shaderType)
{
    GLuint shaderHandle = glCreateShader(shaderType);

    if (shaderHandle)
    {
        glShaderSource(shaderHandle, 1, &source, 0);
        glCompileShader(shaderHandle);

        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shaderHandle, infoLen, NULL, buf);
                    XLog::e(TAG ,"error::Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shaderHandle);
                shaderHandle = 0;
            }
        }

    }

    return shaderHandle;
}

GLuint YuvGLRender::buildProgram(const char* vertexShaderSource,
        const char* fragmentShaderSource)
{
    GLuint vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint programHandle = glCreateProgram();

    if (programHandle)
    {
        glAttachShader(programHandle, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(programHandle, fragmentShader);
        checkGlError("glAttachShader");
        glLinkProgram(programHandle);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
                    XLog::e(TAG ,"error::Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(programHandle);
            programHandle = 0;
        }

    }

    XLog::d(ANDROID_LOG_WARN ,TAG ,"===>program ID:%d\n", programHandle);
    return programHandle;
}


GLuint YuvGLRender::bindTexture(GLuint texture, uint8_t *texture_data, GLuint w , GLuint h)
{
    glBindTexture ( GL_TEXTURE_2D, texture );
    checkGlError("glBindTexture");

    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texture_data);
    checkGlError("glTexImage2D");

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    checkGlError("glTexParameteri");

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    checkGlError("glTexParameteri");

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    checkGlError("glTexParameteri");

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    checkGlError("glTexParameteri");

    return texture;
}


void YuvGLRender::checkGlError(const char* op)
{
    GLint error;
    for (error = glGetError(); error; error = glGetError())
    {
        XLog::e(TAG ,"error::after %s() glError (0x%x)\n", op, error);
    }
}