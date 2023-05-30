#include "Program.h"

void Program::render_layers()
{
	PreRenderState();

	for (auto& l : RenderLayers)
	{
		for (auto& i : l.second)
		{
			if (i.texture->loaded == false)
				Render_LoadTexture(i.texture);

			Render_DrawQuad_Texture_Rotated({ i.x, i.y },
				{ (unsigned int)i.w, (unsigned int)i.h },
					0, i.texture, true);
		}
	}

	RenderState();
}