#pragma once

#include "imgui.h"

__declspec(selectany) bool bUseCustomDraw = false;

typedef float(*FUNC_MEASURE_CHAR)(unsigned int);
__declspec(selectany) FUNC_MEASURE_CHAR Func_Measure_Char = nullptr;

typedef float(*FUNC_MEASURE_TEXT)(const char* utf8_c_, size_t length);
__declspec(selectany) FUNC_MEASURE_TEXT Func_Measure_Text = nullptr;

typedef void(*FUNC_DRAW_CHAR)(unsigned int c_, float x, float y, unsigned int col);
__declspec(selectany) FUNC_DRAW_CHAR Func_Draw_Char = nullptr;

typedef void(*FUNC_DRAW_TEXT)(const char* utf8_c_, size_t length, float x, float y, unsigned int col);
__declspec(selectany) FUNC_DRAW_TEXT Func_Draw_Text = nullptr;

typedef void(*FUNC_CLIP_RC)(const ImVec4& clip_rect);
__declspec(selectany) FUNC_CLIP_RC Func_Clip_Rect = nullptr;


typedef void(*FUNC_UNCLIP_RC)(void);
__declspec(selectany) FUNC_UNCLIP_RC Func_Clip_Reset = nullptr;

typedef void(*FUNC_DRAW_RC)(const ImVec2& p_min, const ImVec2& p_max, ImU32 col);
__declspec(selectany) FUNC_DRAW_RC Func_Draw_Rect = nullptr;


__declspec(selectany) float _font_spacing;

//static float Measure_Char(unsigned int c_){ return 0; }
