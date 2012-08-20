// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/android/draw_context.h"

#include "base/logging.h"
#include "content/public/browser/android/graphics_context.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebGraphicsContext3D.h"

namespace {

static const char g_vertex_shader[] =
  "attribute vec4 a_Position;"
  "attribute vec2 a_texCoord;"
  "varying vec2 v_texCoord;"
  "void main() {"
  "  gl_Position = a_Position;"
  "  v_texCoord = a_texCoord;"
  "}";

// Minimal texture mapping pixel shader.
static const char g_fragment_shader[] =
  "precision mediump float;"
  "varying vec2 v_texCoord;"
  "uniform sampler2D s_texture;"
  "void main() {"
  "  gl_FragColor = texture2D(s_texture, v_texCoord);"
  "}";

}  // anonymous namespace

namespace content {

DrawContext::DrawContext(ANativeWindow* window)
    : program_(GL_ZERO),
      vertex_shader_(0),
      fragment_shader_(0),
      texture_uniform_(GL_ZERO),
      vertex_buffer_(GL_ZERO),
      context_(content::GraphicsContext::CreateForUI(window)) {
  const GLfloat attribs[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f };
  WebKit::WebGraphicsContext3D* context3D = context_->GetContext3D();
  vertex_buffer_ = context3D->createBuffer();
  context3D->bindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  context3D->bufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), attribs,
                        GL_STATIC_DRAW);

  vertex_shader_ = context3D->createShader(GL_VERTEX_SHADER);
  context3D->shaderSource(vertex_shader_, g_vertex_shader);
  context3D->compileShader(vertex_shader_);

  fragment_shader_ = context3D->createShader(GL_FRAGMENT_SHADER);
  context3D->shaderSource(fragment_shader_, g_fragment_shader);
  context3D->compileShader(fragment_shader_);

  program_ = context3D->createProgram();
  context3D->attachShader(program_, vertex_shader_);
  context3D->attachShader(program_, fragment_shader_);
  context3D->linkProgram(program_);

  texture_uniform_ = context3D->getUniformLocation(program_, "s_texture");
}

DrawContext::~DrawContext() {
  if (vertex_buffer_ != GL_ZERO)
    context_->GetContext3D()->deleteBuffer(vertex_buffer_);
  if (program_ != GL_ZERO)
    context_->GetContext3D()->deleteProgram(program_);
}

uint32 DrawContext::Draw(int texture) {
  DCHECK(program_ != GL_ZERO);
  WebKit::WebGraphicsContext3D* context3D = context_->GetContext3D();

  context3D->useProgram(program_);

  context3D->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  context3D->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  context3D->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  context3D->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  context3D->uniform1i(texture_uniform_, 0);
  context3D->activeTexture(GL_TEXTURE0);
  context3D->bindTexture(GL_TEXTURE_2D, texture);

  context3D->bindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  context3D->enableVertexAttribArray(0);
  context3D->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  context3D->bindAttribLocation(program_, 0, "a_Position");
  context3D->enableVertexAttribArray(1);
  context3D->vertexAttribPointer(
      1, 2, GL_FLOAT, GL_FALSE, 0, 8 * sizeof(GLfloat));
  context3D->bindAttribLocation(program_, 1, "a_texCoord");

  context3D->drawArrays(GL_TRIANGLE_STRIP, 0, 4);

  context3D->prepareTexture();

  return context_->InsertSyncPoint();
}

void DrawContext::Reshape(int width, int height) {
  WebKit::WebGraphicsContext3D* context3D = context_->GetContext3D();
  context3D->viewport(0, 0, width, height);
  context3D->reshape(width, height);
}

}  // namespace content
