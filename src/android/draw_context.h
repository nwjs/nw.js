// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_ANDROID_DRAW_CONTEXT_H_
#define CONTENT_SHELL_ANDROID_DRAW_CONTEXT_H_

#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "ui/gfx/native_widget_types.h"

#include <GLES2/gl2.h>

namespace content {
class GraphicsContext;

// Helper class for drawing content textures to the UI window surface.
class DrawContext {
 public:
  DrawContext(ANativeWindow* window);
  ~DrawContext();

  // Draws the given texture to the UI and returns a
  // SyncPoint identifier.
  uint32 Draw(int texture);

  void Reshape(int width, int height);

 private:
  // The shader program.
  int program_;
  int vertex_shader_;
  int fragment_shader_;

  // Shader uniform locations.
  int texture_uniform_;

  // The vertex attribute array used to draw a simple quad.
  unsigned int vertex_buffer_;

  // The graphics context used for drawing.
  scoped_ptr<content::GraphicsContext> context_;

  DISALLOW_COPY_AND_ASSIGN(DrawContext);
};

}  // namespace content

#endif // CONTENT_SHELL_ANDROID_DRAW_CONTEXT_H_
