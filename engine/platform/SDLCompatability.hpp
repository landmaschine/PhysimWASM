#pragma once

#include "Core/common.hpp"

#ifdef __EMSCRIPTEN__
namespace SDLCompat {
    inline void DestroyGLContext(SDL_GLContext context) {
        (void)context;
    }

    inline uint32_t GetTicks() {
      return (uint32_t)SDL_GetTicks();
    }
    
}
#else
namespace SDLCompat {
    inline void DestroyGLContext(SDL_GLContext context) {
        SDL_GL_DestroyContext(context);
    }
    
    inline uint32_t GetTicks() {
      return (uint32_t)SDL_GetTicks();
    }
}
#endif