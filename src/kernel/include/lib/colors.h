#pragma once

#define COLOR(r,g,b) ((b) | (g << 8) | (r << 16))

#define COLOR_BLACK     COLOR(0  ,0  ,0  )
#define COLOR_WHITE     COLOR(255,255,255)
#define COLOR_RED       COLOR(255,0  ,0  )
#define COLOR_GREEN     COLOR(0  ,255,0  )
#define COLOR_BLUE      COLOR(0  ,0  ,255)
#define COLOR_YELLOW    COLOR(255,255,0  )
#define COLOR_CYAN      COLOR(0  ,255,255)
#define COLOR_MAGENTA   COLOR(255,0  ,255)
