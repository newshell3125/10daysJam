#include "DebugText.h"

void DebugText::Initialize(UINT texnumber)
{
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		spriteDatas[i] = Sprite::Create(texnumber, {0,0});
	}

}

void DebugText::Print(const std::string& text, float x, float y, float scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontindexY = fontIndex / fontLineCount;
		int fontindexX = fontIndex % fontLineCount;

		spriteDatas[spriteIndex]->SetPosition({ x + fontWidth * scale * i,y });
		spriteDatas[spriteIndex]->SetTextureRect(
			fontindexX * fontWidth, 
			fontindexY * fontHeight, 
			fontWidth, fontHeight);

		spriteDatas[spriteIndex]->SetSize(fontWidth * scale, fontHeight * scale);

		spriteIndex++;
	}

}

void DebugText::DrawAll()
{
	for (int i = 0; i < spriteIndex; i++)
	{
		spriteDatas[i]->Draw();
	}
	spriteIndex = 0;
}

void DebugText::Finalize()
{
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		delete spriteDatas[i];
	}
}
