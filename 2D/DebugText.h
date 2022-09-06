#pragma once

#include<wrl.h>
#include <Windows.h>
#include<string>

#include"Sprite.h"

class DebugText
{
public:
	static const int maxCharCount = 256;//最大文字数
	static const int fontWidth = 9;		//フォント画像内1文字分の横幅
	static const int fontHeight = 18;	//フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;//フォント画像内1行分の文字数

	void Initialize(UINT texnumber);

	void Print(const std::string& text, float x, float y, float scale = 1.0f);

	void DrawAll();

	void Finalize();
private:
	Sprite* spriteDatas[maxCharCount];

	int spriteIndex = 0;
};

