#include "Texture.h"
#include <cassert>

using namespace DirectX;

void Texture::Initialize(DirectXCommon* dxCom)
{
	HRESULT result = S_FALSE;

	this->dxCom = dxCom;

	//デスクリプタヒープを生成
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = textureMaxCount;
	result = dxCom->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	if (FAILED(result))
	{
		assert(0);
	}
}

void Texture::LoadTexture(UINT texnumber, const wchar_t* filename)
{
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	if (FAILED(result))
	{
		assert(0);
	}

	const Image* img = scratchImg.GetImage(0, 0, 0);

	//テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);
	//テクスチャ用バッファの生成
	result = dxCom->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[texnumber])
	);

	if (FAILED(result))
	{
		assert(0);
	}

	//テクスチャバッファにデータ転送
	result = texBuff[texnumber]->WriteToSubresource(
		0,
		nullptr,	//全領域へのコピー
		img->pixels,	//元のデータアドレス
		(UINT)img->rowPitch,		//1ラインサイズ
		(UINT)img->slicePitch	//全サイズ
	);

	if (FAILED(result))
	{
		assert(0);
	}

	//シェーダリソースビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = metadata.format;	//RGBA
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	dxCom->GetDevice()->CreateShaderResourceView(
		texBuff[texnumber].Get(),	//ビューと関連付けるバッファ
		&srvDesc,	//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeap->GetCPUDescriptorHandleForHeapStart(),
			texnumber,
			dxCom->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		));
}

void Texture::SetDescriptorHeaps(ID3D12GraphicsCommandList* cmdList)
{
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };

	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void Texture::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex, UINT texNumber)
{
	//SRVサイズ
	UINT sizeSRV= dxCom->GetDevice()->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);
	//デスクリプタテーブルの先頭を取得
	D3D12_GPU_DESCRIPTOR_HANDLE start = descHeap->GetGPUDescriptorHandleForHeapStart();

	//SRVのGPUハンドルを取得
	CD3DX12_GPU_DESCRIPTOR_HANDLE handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(start, texNumber, sizeSRV);

	cmdList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
}

ID3D12Resource* Texture::GetTexBuff(UINT texnumber)
{
	//オーバーフロー防止
	//assert(texnumber < textureMaxCount);
	if (texnumber >= textureMaxCount)
	{
		return nullptr;
	}
	return texBuff[texnumber].Get();
}
