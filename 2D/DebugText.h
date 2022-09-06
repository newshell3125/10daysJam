#pragma once

#include<wrl.h>
#include <Windows.h>
#include<string>

#include"Sprite.h"

class DebugText
{
public:
	static const int maxCharCount = 256;//�ő啶����
	static const int fontWidth = 9;		//�t�H���g�摜��1�������̉���
	static const int fontHeight = 18;	//�t�H���g�摜��1�������̏c��
	static const int fontLineCount = 14;//�t�H���g�摜��1�s���̕�����

	void Initialize(UINT texnumber);

	void Print(const std::string& text, float x, float y, float scale = 1.0f);

	void DrawAll();

	void Finalize();
private:
	Sprite* spriteDatas[maxCharCount];

	int spriteIndex = 0;
};

