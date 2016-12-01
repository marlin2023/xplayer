//
// Created by chris on 10/20/16.
//

#define TAG "YUV_GL_RENDER"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "YuvGLRender.h"

#include "util/XLog.h"

YuvGLRender::YuvGLRender(MediaFile *mediaFile)
{
    this->mediaFileHandle = mediaFile;
    texture_coord_x = 1.0;
    //this->init();
}

YuvGLRender::~YuvGLRender()
{

    // delete teture
     glDeleteTextures(1, &textureYId);
     textureYId = -1;
     glDeleteTextures(1, &textureUId);
     textureUId = -1;
     glDeleteTextures(1, &textureVId);
     textureVId = -1;

    // free opengl resource
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(simpleProgram);
}

void YuvGLRender::init()
{
    simpleProgram = buildProgram(VERTEX_SHADER, FRAG_SHADER);
    glUseProgram(simpleProgram);
    XLog::d(ANDROID_LOG_WARN ,TAG ,"==>simpleProgram:%d\n", simpleProgram);

    mPositionSlot = glGetAttribLocation(simpleProgram, "vPosition");
    mT_texCoordInSlot = glGetAttribLocation(simpleProgram, "a_texCoord");
    XLog::d(ANDROID_LOG_WARN ,TAG ,"===>mPositionSlot:%d mT_texCoordInSlot:%d", mPositionSlot,
                                               mT_texCoordInSlot);
    glEnableVertexAttribArray(mPositionSlot);
    glEnableVertexAttribArray(mT_texCoordInSlot);


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
    //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>in render_frame thread .");
    //this->mediaFileHandle->video_frame_queue->get(src_frame);
    int ret1 = this->mediaFileHandle->video_frame_queue->get(src_frame ,0); // no block mode
    if(ret1 != 1){
        XLog::d(ANDROID_LOG_WARN ,TAG ,"==>in render_frame thread .no video frame ,return ,video packet size = %d ,video packet q mem size =%d" ,mediaFileHandle->video_queue->size() ,mediaFileHandle->video_queue->q_size);
        XLog::d(ANDROID_LOG_WARN ,TAG ,"==>in render_frame thread .no video frame ,return ,audio packet size = %d ,audio packet q mem size =%d" ,mediaFileHandle->audio_queue->size() ,mediaFileHandle->audio_queue->q_size);
        //mediaFileHandle->stopRender();
        usleep(50 * 1000); //in microseconds
        av_frame_free(&src_frame);  // free frame memory
        return;
    }

    //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>video frame queue size :%d\n", this->mediaFileHandle->video_frame_queue->size());

    // For synchronization start
    int64_t video_frame_render_pts = src_frame->pts * av_q2d(mediaFileHandle->video_stream->time_base) * 1000;   // in millisecond
    int64_t sync_audio_clock_time = mediaFileHandle->sync_audio_clock_time;
    int64_t diff_time = video_frame_render_pts - sync_audio_clock_time;
    int64_t sync_sleep_time = 0;

    // TODO filter some error diff_time.
    if( (diff_time > 0) && (diff_time < 800) ){ // 100 millisecond
        sync_sleep_time = diff_time * 1000;

    }else if( (diff_time >= -500) && (diff_time <= 0) ){

        sync_sleep_time = 500;
    }else if(diff_time  < -500){
        XLog::d(ANDROID_LOG_WARN ,TAG ,"==>sync_video_clock_time=%lld ,sync_audio_clock_time =%lld ,diff_time =%lld ,"
                                                                    "video_frame_q size =%d ,audio_frame_q size =%d\n",
                                                                     video_frame_render_pts ,sync_audio_clock_time,diff_time ,
                                                                     mediaFileHandle->video_frame_queue->size() ,mediaFileHandle->audio_frame_queue->size());

        sync_sleep_time = 1000;    /// video frame need to catch the audio frame clock.
    }else{
        XLog::d(ANDROID_LOG_WARN ,TAG ,"==>sync_video_clock_time=%lld ,sync_audio_clock_time =%lld ,diff_time =%lld ,"
                                                                    "video_frame_q size =%d ,audio_frame_q size =%d\n",
                                                                     video_frame_render_pts ,sync_audio_clock_time,diff_time ,
                                                                     mediaFileHandle->video_frame_queue->size() ,mediaFileHandle->audio_frame_queue->size());

        sync_sleep_time = 100 * 1000;
    }
    usleep(sync_sleep_time); //in microseconds
    // For synchronization end

    //XLog::d(ANDROID_LOG_WARN ,TAG ,"==>src_frame->linesize[0] =%d\n",src_frame->linesize[0]);

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

    if(src_frame->linesize[0] != src_frame->width){

        texture_coord_x = src_frame->width / (float)src_frame->linesize[0] - 0.009;  // 0.009 used to calibration .
        //XLog::d(ANDROID_LOG_WARN ,TAG ,"==> reset texture coordinate  ,padding size =%d ,amend texture_coord_x =%f\n" ,src_frame->linesize[0] - src_frame->width ,texture_coord_x);
        // reset texture coordinate
        coordVertices[2] = texture_coord_x;
        coordVertices[6] = texture_coord_x;
        //coordVertices[] = {
        //    0.0f, 1.0f,
        //    texture_coord_x, 1.0f,
        //     0.0f, 0.0f,
        //    texture_coord_x, 0.0f };
    }

    glVertexAttribPointer(mPositionSlot, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(mPositionSlot);

    glVertexAttribPointer(mT_texCoordInSlot, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(mT_texCoordInSlot);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYId);
    glUniform1i(tex_y, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureUId);
    glUniform1i(tex_u, 1);

    glActiveTexture(GL_TEXTURE2);
    checkGlError("glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, textureVId);
    checkGlError("glBindTexture");
    glUniform1i(tex_v, 2);
    checkGlError("glUniform1i");

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    checkGlError("glDrawArrays");

    //av_frame_unref(src_frame);
    av_frame_free(&src_frame);  // free frame memory
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
    vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
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