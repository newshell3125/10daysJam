#pragma once

#include<d3d12.h>
#include<d3dx12.h>
#include<wrl.h>
#include<DirectXMath.h>

#include<DirectXTex.h>

#include<d3dcompiler.h>

#include"DirectXCommon.h"
#include"WindowApp.h"
#include"Texture.h"

class Sprite
{
//エイリアス
public:
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	
//サブクラス
public:
	class Common
	{
		friend class Sprite;

	public:
		void InitializeGraphicsPipeline();

		ComPtr<ID3D12PipelineState>pipelineState;
		ComPtr<ID3D12RootSignature> rootSignature;
		DirectX::XMMATRIX matProjection{};

		ComPtr<ID3DBlob> rootSigBlob;

		DirectXCommon* dxCom = nullptr;
		Texture* texture = nullptr;
		WindowApp* winApp = nullptr;


	};

	//頂点データ構造体
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos;//xyz座標
		DirectX::XMFLOAT2 uv;//uv座標
	};

	//定数データ構造体
	struct ConstBufferDate
	{
		DirectX::XMFLOAT4 color;	//色(RGBA)
		DirectX::XMMATRIX mat;	//3D変換行列
	};

//静的メンバ関数
public:
	//静的メンバの初期化
	static void StaticInitialize(WindowApp* winApp,DirectXCommon* dxCom, Texture* texture);

	//静的メンバの解放
	static void StaticFinalize();

	//パイプラインステートの設定
	static void sPipelineState();

	static Sprite* Create(UINT texNumber, XMFLOAT2 position);
//メンバ関数
public:
	Sprite() {};
	~Sprite() {};

	//初期化+生成
	void Initialize(UINT texNumber,XMFLOAT2 position);

	//頂点バッファデータ転送
	void TransferVertices();

	//Set関数
	//位置変更 
	void SetPosition(XMFLOAT2 position);
	//大きさ変更
	void SetSize(float width, float height);
	//左右反転
	void SetisFlipX(bool isFlipX);
	//上下反転
	void SetisFlipY(bool isFlipY);
	//アンカーポイントの変更
	void SetAnchorPoint(XMFLOAT2 anchorPoint);
	//テクスチャ範囲設定
	void SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height);
	//テクスチャ変更
	void SetTextureNum(int num);

	//描画
	void Draw();
	
//静的メンバ変数
private:
	static Common* common;

	static const int texMax = 100;

//メンバ変数
private:
	ComPtr<ID3D12Resource>vertBuff;
	ComPtr<ID3D12Resource>constBuff;
	D3D12_VERTEX_BUFFER_VIEW vBView{};

	float rotation = 0.0f;//Z軸回りの回転角
	XMFLOAT2 position{0,0};//座標
	XMMATRIX matWorld{};//ワールド行列
	XMFLOAT4 color = { 1,1,1,1 };//色
	UINT texNumber;//テクスチャ番号

	XMFLOAT2 anchorpoint{};
	//頂点数
	static const int vertNum = 4;
	//横
	float width = 100.0f;
	//縦
	float height = 100.0f;
	//左右反転
	bool isFlipX = false;
	//上下反転
	bool isFlipY = false;
	//テクスチャ切り取り範囲
	float tex_x = 0.0f;
	float tex_y = 0.0f;
	float tex_width = 0.0f;
	float tex_height = 0.0f;


	//スプライト用頂点データ
	VertexPosUv vertices[4] = {
		{{0.0f,100.0f,0.0f},{0.0f,1.0f}},
		{{0.0f,0.0f,0.0f},{0.0f,0.0f}},
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},
		{{100.0f,0.0f,0.0f},{1.0f,0.0f}},
	};
	enum { LB, LT, RB, RT };

};