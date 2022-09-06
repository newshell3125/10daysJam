#include "Sprite.h"
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")

Sprite::Common* Sprite::common = nullptr;

void Sprite::StaticInitialize(WindowApp* winApp,DirectXCommon* dxCom,Texture* texture)
{
	common = new Common();
	common->texture = texture;
	common->dxCom = dxCom;

	common->InitializeGraphicsPipeline();

	common->matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, winApp->window_width, 
		winApp->window_height,
		0.0f, 0.0f, 1.0f);


}

void Sprite::Common::InitializeGraphicsPipeline()
{
	HRESULT result;

	ComPtr<ID3DBlob> vsBlob;  //頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;  //ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob;  //エラーオブジェクト

	//スプライト用
		//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"SpriteVertexShader.hlsl",  //シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  //インクルード可能にする
		"VSmain", "vs_5_0",  //エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  //デバッグ用設定
		0,
		&vsBlob, &errorBlob
	);

	if (FAILED(result))
	{
		assert(0);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"SpritePixelShader.hlsl",  //シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  //インクルード可能にする
		"PSmain", "ps_5_0",  //エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,  //デバッグ用設定
		0,
		&psBlob, &errorBlob
	);

	if (FAILED(result))
	{
		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin()
		);

		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//スプライト用
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{//xy座標
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},

		{	//uv座標(1行で書いたほうが見やすい)
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;  //標準設定

	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;


	//レンダーターゲットのブレンド設定(8個あるがいまは一つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};

	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;  //RBGA全てのチャンネルを描画

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//標準設定

	//ブレンドステートに設定する

	blenddesc.BlendEnable = true;					//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を	0%使う

	//加算合成
	/*blenddesc.BlendOp = D3D12_BLEND_OP_ADD;	//加算
	blenddesc.SrcBlend = D3D12_BLEND_ONE;		//ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_ONE;		//デストの値を100%使う*/

	//減算合成
	/*blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	//デストからソースを減算
	blenddesc.SrcBlend = D3D12_BLEND_ONE;				//ソースの値を100%使う
	blenddesc.DestBlend = D3D12_BLEND_ONE;				//デストの値を100%使う*/

	//色反転
	/*blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	//1.0f-デストカラーの値
	blenddesc.DestBlend = D3D12_BLEND_ZERO;				//使わない*/

	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-ソースのアルファ値

	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;  //描画対象は１つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;  //0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1;  //1ピクセルにつき1回サンプリング

	//デプスステンシルステートの設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;	//深度値フォーマット

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE sDescRangeSRV;
	sDescRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER sRootparams[2];
	//定数用
	sRootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	//テクスチャ用
	sRootparams[1].InitAsDescriptorTable(1, &sDescRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
	
	//ルートシグネチャ
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(sRootparams), sRootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);

	result = dxCom->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));

	if (FAILED(result))
	{
		assert(0);
	}

	gpipeline.pRootSignature = rootSignature.Get();

	result = dxCom->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));

	if (FAILED(result))
	{
		assert(0);
	}
}



void Sprite::StaticFinalize()
{
	if (common != nullptr)
	{
		delete common;
		common = nullptr;
	}
}


void Sprite::Initialize(UINT texNumber,XMFLOAT2 position)
{
	HRESULT result = S_FALSE;

	this->texNumber = texNumber;
	this->position = position;

	//テクスチャ情報取得
	if (common->texture->GetTexBuff(texNumber))
	{
		D3D12_RESOURCE_DESC resDesc = common->texture->GetTexBuff(texNumber)->GetDesc();

		width = (float)resDesc.Width;
		height = (float)resDesc.Height;

		tex_width = (float)resDesc.Width;
		tex_height = (float)resDesc.Height;
	}

	//頂点バッファ生成
	result = common->dxCom->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	if (FAILED(result))
	{
		assert(0);
	}

	TransferVertices();

	//頂点バッファビュー
	vBView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vBView.SizeInBytes = sizeof(vertices);
	vBView.StrideInBytes = sizeof(vertices[0]);

	//定数バッファ生成
	result = common->dxCom->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDate) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

	if (FAILED(result))
	{
		assert(0);
	}

	//定数バッファデータ転送
	ConstBufferDate* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = color;
	constMap->mat = common->matProjection;
	constBuff->Unmap(0, nullptr);
}

void Sprite::TransferVertices()
{
	HRESULT result = S_FALSE;

	//vertices[LB].pos = { 0.0f,height,0.0f };//左下
	//vertices[LT].pos = { 0.0f,0.0f,0.0f };//左上
	//vertices[RB].pos = { width,height,0.0f };//右下
	//vertices[RL].pos = { width,0.0f,0.0f };//右上



	float left = (0.0f - anchorpoint.x) * width;
	float right = (1.0f - anchorpoint.x) * width;
	float top = (0.0f - anchorpoint.y) * height;
	float bottom = (1.0f - anchorpoint.y) * height;

	if (isFlipX)
	{
		left = -left;
		right = -right;
	}

	if (isFlipY)
	{
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,bottom,0.0f };//左下
	vertices[LT].pos = { left,top,0.0f };//左上
	vertices[RB].pos = { right,bottom,0.0f };//右下
	vertices[RT].pos = { right,top,0.0f };//右上

		// テクスチャ情報取得
	if (common->texture->GetTexBuff(texNumber))
	{
		D3D12_RESOURCE_DESC resDesc = common->texture->GetTexBuff(texNumber)->GetDesc();

		float tex_left = tex_x / resDesc.Width;
		float tex_right = (tex_x + tex_width) / resDesc.Width;
		float tex_top = tex_y / resDesc.Height;
		float tex_bottom = (tex_y + tex_height) / resDesc.Height;

		vertices[LB].uv = { tex_left,	tex_bottom }; // 左下
		vertices[LT].uv = { tex_left,	tex_top }; // 左上
		vertices[RB].uv = { tex_right,	tex_bottom }; // 右下
		vertices[RT].uv = { tex_right,	tex_top }; // 右上
	}


	//頂点バッファデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);

	if (FAILED(result))
	{
		assert(0);
	}

}

void Sprite::sPipelineState()
{
	//パイプラインステートの設定
	common->dxCom->GetCmdList()->SetGraphicsRootSignature(common->rootSignature.Get());
	common->dxCom->GetCmdList()->SetPipelineState(common->pipelineState.Get());

	common->dxCom->GetCmdList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

Sprite* Sprite::Create(UINT texNumber, XMFLOAT2 position)
{
	Sprite* sprite=new Sprite();

	sprite->Initialize(texNumber, position);

	return sprite;
}

void Sprite::Draw()
{
	//頂点バッファの設定
	common->dxCom->GetCmdList()->IASetVertexBuffers(0, 1, &vBView);
	//定数バッファビューをセット
	common->dxCom->GetCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//デスクリプタヒープの配列
	common->texture->SetDescriptorHeaps(common->dxCom->GetCmdList());
	//シェーダーリソースビューをセット
	common->texture->SetGraphicsRootDescriptorTable(common->dxCom->GetCmdList(), 1, texNumber);

	//ワールド行列の更新
	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation));
	matWorld *= DirectX::XMMatrixTranslation(position.x,position.y,0.0f);

	//行列の転送
	ConstBufferDate* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matWorld * common->matProjection;
	constBuff->Unmap(0, nullptr);


	//描画コマンドの設定
	common->dxCom->GetCmdList()->DrawInstanced(4, 1, 0, 0);

}

void Sprite::SetPosition(XMFLOAT2 position)
{
	this->position = position;

	TransferVertices();

}

void Sprite::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;

	TransferVertices();	
}

void Sprite::SetisFlipX(bool isFlipX)
{
	this->isFlipX = isFlipX;

	TransferVertices();
}

void Sprite::SetisFlipY(bool isFlipY)
{
	this->isFlipY = isFlipY;

	TransferVertices();

}

void Sprite::SetAnchorPoint(XMFLOAT2 anchorPoint)
{
	this->anchorpoint = anchorPoint;

	TransferVertices();
}

void Sprite::SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height)
{
	this->tex_x = tex_x;
	this->tex_y = tex_y;
	this->tex_width = tex_width;
	this->tex_height = tex_height;

	width = this->tex_width;
	height = this->tex_height;

	TransferVertices();
}

void Sprite::SetTextureNum(int num)
{
	Initialize(num, position);
}
