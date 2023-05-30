#pragma once

#include <string>

#include "libs/sdl/include/SDL_image.h"
#include "libs/sdl/include/SDL_ttf.h"

#define uint8_t unsigned char

#define FONT_ID uint8_t
#define FONT_NULL -1

class MTexture
{
public:
	enum MTextureType
	{
		Type_None,
		Type_FromSurface,
		Type_FromPath
	};
	enum MTextureFlip
	{
		Flip_None,
		Flip_Horizontal,
		Flip_Vertical
	};

public:
	SDL_Texture* handle = NULL;
	std::string path;
	SDL_Surface* surface = NULL;
	
	MTextureType type = Type_None;
	MTextureFlip flip = Flip_None;

	unsigned int width = 0;
	unsigned int height = 0;

	bool loaded = false;
};

struct MColour
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

template<typename T>
struct QuadPoint
{
	T x;
	T y;
};

struct MFontParameters
{
	enum TextType
	{
		Font_8Bit = 0,
		Font_UTF,
		Font_Glyph,
		Font_Unicode
	};

	enum BlendType
	{
		Font_Solid = 4,
		Font_Shaded,
		Font_LCD,
		Font_Blended
	};

	TextType text_type;
	BlendType blend_type;
	MColour foreground;
	MColour background;
	int wrap_length = 0;
};

void InitRendering();
void FinaliseRendering();

int CreateFont(std::string path, int size, std::string name);

void PreRenderState();
void RenderState();

void Render_ClearColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void Render_DrawQuad_Colour(QuadPoint<int> center, QuadPoint<unsigned int> size, MColour colour, bool top_left = false);
void Render_DrawQuad_Texture(QuadPoint<int> center, QuadPoint<unsigned int> size, MTexture* texture, bool top_left = false);
void Render_DrawQuad_Texture_Rotated(QuadPoint<int> center, QuadPoint<unsigned int> size, double angle, MTexture* texture, bool top_left = false);

MTexture* Render_CreateTexture(std::string path);
MTexture* Render_CreateTexture_Text(std::string text, FONT_ID font_id, MFontParameters params);
void Render_DeleteTexture(MTexture* texture);
void Render_LoadTexture(MTexture* texture);
void Render_UnloadTexture(MTexture* texture);