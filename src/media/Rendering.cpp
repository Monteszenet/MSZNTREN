#include "Rendering.h"

#include <vector>

#include "libs/sdl/include/SDL_image.h"
#include "libs/sdl/include/SDL_ttf.h"

//#include "libs/glew/GL/glew.h"

#include "src/core/Window.h"
#include "src/core/Logging.h"

struct MFont
{
	TTF_Font* handle = NULL;
	std::string path;
	std::string name;
};

struct Renderer
{
	//SDL_GLContext gl_context = NULL;
	SDL_Renderer* handle = NULL;
	SDL_Texture* target = NULL;

	std::vector<MFont> fonts;
	int font_amount = -1;

} *renderer = NULL;

void InitRendering()
{
	renderer = new Renderer;

	// renderer->gl_context = SDL_GL_CreateContext(Window_GetHandle());
	// 
	// GLenum err = glewInit()
;	// if (err != GLEW_OK)
	// 	LOG_FATAL("Could not initialise GLEW, GLEW Error: " /*+ glewGetErrorString(err)*/);
	// 
	// LOG_INFO("Initialised OpenGL.");


	renderer->handle = SDL_CreateRenderer(Window_GetHandle(), -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

	renderer->target = SDL_CreateTexture(renderer->handle, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, Window_GetWidth(), Window_GetHeight());

	LOG_INFO("Initialised renderer.");
}

void FinaliseRendering()
{
	for (auto& f : renderer->fonts)
	{
		TTF_CloseFont(f.handle);
		f.handle = NULL;
	}

	SDL_DestroyRenderer(renderer->handle);
	renderer = NULL;
}

int CreateFont(std::string path, int size, std::string name)
{
	MFont new_font = { NULL, path, name };

	new_font.handle = TTF_OpenFont(path.c_str(), size);
	if (new_font.handle == NULL)
		LOG_ERROR("Unable to open font. Path given: '" + path + "'\n[SDL ERROR] " + C(SDL_GetError()));

	renderer->fonts.push_back(new_font);

	renderer->font_amount += 1;

	return renderer->font_amount;
}

void PreRenderState()
{
	// Render all pushed textures to target.
	SDL_SetRenderTarget(renderer->handle, renderer->target);
	SDL_SetRenderDrawColor(renderer->handle, 0, 0, 0, 255);
	SDL_RenderClear(renderer->handle);
}

void RenderState()
{
	SDL_SetRenderTarget(renderer->handle, NULL);

	SDL_RenderCopy(renderer->handle, renderer->target, NULL, NULL);

	SDL_RenderPresent(renderer->handle);
}

void Render_ClearColour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	SDL_SetRenderDrawColor(renderer->handle, r, g, b, a);
}



void Render_DrawQuad_Colour(QuadPoint<int> center, QuadPoint<unsigned int> size, MColour colour, bool top_left)
{
	SDL_Rect rect;
	if (top_left == false)
		rect = { center.x - ((int)size.x / 2), center.y - ((int)size.y / 2), (int)size.x, (int)size.y };
	else if (top_left == true)
		rect = { center.x, center.y, (int)size.x, (int)size.y };
	SDL_SetRenderTarget(renderer->handle, renderer->target);

	SDL_SetRenderDrawColor(renderer->handle, colour.r, colour.g, colour.b, colour.a);

	SDL_RenderFillRect(renderer->handle, &rect);
}

void Render_DrawQuad_Texture(QuadPoint<int> center, QuadPoint<unsigned int> size, MTexture* texture, bool top_left)
{
	SDL_Rect dst_rect;
	if (top_left == false)
		dst_rect = { center.x - ((int)size.x / 2), center.y - ((int)size.y / 2), (int)size.x, (int)size.y };
	else if (top_left == true)
		dst_rect = { center.x, center.y, (int)size.x, (int)size.y };

	SDL_RenderCopyEx(renderer->handle, texture->handle, NULL, &dst_rect, 0, NULL, (SDL_RendererFlip)texture->flip);
}

void Render_DrawQuad_Texture_Rotated(QuadPoint<int> center, QuadPoint<unsigned int> size, double angle, MTexture* texture, bool top_left)
{
	SDL_Rect dst_rect;
	if (top_left == false)
		dst_rect = { center.x - ((int)size.x / 2), center.y - ((int)size.y / 2), (int)size.x, (int)size.y };
	else if (top_left == true)
		dst_rect = { center.x, center.y, (int)size.x, (int)size.y };

	SDL_RenderCopyEx(renderer->handle, texture->handle, NULL, &dst_rect, angle, NULL, (SDL_RendererFlip)texture->flip);
}

MTexture* Render_CreateTexture(std::string path)
{
	MTexture* new_texture = new MTexture;
	new_texture->path = path;
	
	new_texture->type = MTexture::Type_FromPath;

	int w = 0;
	int h = 0;

	// First-time load, sets params.
	new_texture->handle = IMG_LoadTexture(renderer->handle, new_texture->path.c_str());

	if (new_texture->handle == NULL)
		LOG_ERROR("Failed to create texture. Path given: '" + path + "'\n[SDL ERROR] " + C(SDL_GetError()));

	SDL_QueryTexture(new_texture->handle, NULL, NULL, 
		&w, &h);
	SDL_DestroyTexture(new_texture->handle);
	new_texture->handle = NULL;
	
	new_texture->width = (unsigned int)w;
	new_texture->height = (unsigned int)h;

	return new_texture;
}

MTexture* Render_CreateTexture_Text(std::string text, FONT_ID font_id, MFontParameters params)
{
	MTexture* new_texture = new MTexture;

	new_texture->type = MTexture::Type_FromSurface;

	MFont* font = NULL;
	font = &renderer->fonts[font_id];
	
	if (font == NULL)
		LOG_ERROR("Attempted to index nonexistent font. Font index: " + S(font_id));

	switch (params.text_type)
	{
	case MFontParameters::Font_8Bit:
		switch (params.blend_type)
		{
		case MFontParameters::Font_Solid:
			new_texture->surface = TTF_RenderText_Solid_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				params.wrap_length);
			break;
		case MFontParameters::Font_Shaded:
			new_texture->surface = TTF_RenderText_Shaded_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				{ params.background.r, params.background.g, params.background.b, params.background.a },
				params.wrap_length);
			break;
		case MFontParameters::Font_LCD:
			new_texture->surface = TTF_RenderText_LCD_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				{ params.background.r, params.background.g, params.background.b, params.background.a },
				params.wrap_length);
			break;
		case MFontParameters::Font_Blended:
			new_texture->surface = TTF_RenderText_Blended_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				params.wrap_length);
			break;
		}
		break;
	case MFontParameters::Font_UTF:
		switch (params.blend_type)
		{
		case MFontParameters::Font_Solid:
			new_texture->surface = TTF_RenderUTF8_Solid_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				params.wrap_length);
			break;
		case MFontParameters::Font_Shaded:
			new_texture->surface = TTF_RenderUTF8_Shaded_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				{ params.background.r, params.background.g, params.background.b, params.background.a },
				params.wrap_length);
			break;
		case MFontParameters::Font_LCD:
			new_texture->surface = TTF_RenderUTF8_LCD_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				{ params.background.r, params.background.g, params.background.b, params.background.a },
				params.wrap_length);
			break;
		case MFontParameters::Font_Blended:
			new_texture->surface = TTF_RenderUTF8_Blended_Wrapped(font->handle, text.c_str(),
				{ params.foreground.r, params.foreground.g, params.foreground.b, params.foreground.a },
				params.wrap_length);
			break;
		}
		break;
	}

	if (new_texture->surface == NULL)
		LOG_FATAL("Failed to create font-texture from text. Text given: '" + text + "', Font given: '" + font->name + 
			"'\n[SDL ERROR] " + C(SDL_GetError()));

	new_texture->width = new_texture->surface->w;
	new_texture->height = new_texture->surface->h;

	return new_texture;
}

void Render_DeleteTexture(MTexture* texture)
{
	if (texture->loaded == true)
		Render_UnloadTexture(texture);
	if (texture->surface != NULL)
		SDL_FreeSurface(texture->surface);
	delete texture;
}

void Render_LoadTexture(MTexture* texture)
{
	if (texture->loaded == false)
	{
		texture->loaded = true;

		if (texture->type == MTexture::Type_FromPath)
		{
			texture->handle = IMG_LoadTexture(renderer->handle, texture->path.c_str());
		}
		else if (texture->type == MTexture::Type_FromSurface)
		{
			texture->handle = SDL_CreateTextureFromSurface(renderer->handle, texture->surface);
		}
	}
}

void Render_UnloadTexture(MTexture* texture)
{
	if (texture->loaded == true)
	{
		texture->loaded = false;

		SDL_DestroyTexture(texture->handle);
		texture->handle = NULL;
	}
}