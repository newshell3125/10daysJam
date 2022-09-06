#pragma once
#include<d3d12.h>
#include<d3dx12.h>
#include<wrl.h>
#include<DirectXTex.h>
#include"DirectXCommon.h"


class Texture
{
public:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public://�萔
	static const int textureMaxCount = 512;//�e�N�X�`���̍ő喇��

public://�����o�֐�
	//������
	void Initialize(DirectXCommon* dxCom);

	void LoadTexture(UINT texnumber,const wchar_t* filename);

	void SetDescriptorHeaps(ID3D12GraphicsCommandList* cmdList);
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex, UINT texNumber);

	ID3D12Resource* GetTexBuff(UINT texnumber);

	ID3D12DescriptorHeap* GetDescHeap() { return descHeap.Get(); }

private://�����o�ϐ�
	DirectXCommon* dxCom = nullptr;

	ComPtr<ID3D12DescriptorHeap> descHeap;
	ComPtr<ID3D12Resource> texBuff[textureMaxCount];
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};


};

