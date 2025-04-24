#pragma once

#include <iostream>
#include <cstdint>

#ifndef __EMSCRIPTEN__
#include <cstdalign>
#endif

#include <memory>
#include <vector>
#include <iomanip>
#include <limits>
#include <numbers>

#include "SDL3/SDL.h"
#include "Logger/Logger.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"