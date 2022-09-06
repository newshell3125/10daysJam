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
//�G�C���A�X
public:
	//Microsoft::WRL::���ȗ�
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
	
//�T�u�N���X
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

	//���_�f�[�^�\����
	struct VertexPosUv
	{
		DirectX::XMFLOAT3 pos;//xyz���W
		DirectX::XMFLOAT2 uv;//uv���W
	};

	//�萔�f�[�^�\����
	struct ConstBufferDate
	{
		DirectX::XMFLOAT4 color;	//�F(RGBA)
		DirectX::XMMATRIX mat;	//3D�ϊ��s��
	};

//�ÓI�����o�֐�
public:
	//�ÓI�����o�̏�����
	static void StaticInitialize(WindowApp* winApp,DirectXCommon* dxCom, Texture* texture);

	//�ÓI�����o�̉��
	static void StaticFinalize();

	//�p�C�v���C���X�e�[�g�̐ݒ�
	static void sPipelineState();

	static Sprite* Create(UINT texNumber, XMFLOAT2 position);
//�����o�֐�
public:
	Sprite() {};
	~Sprite() {};

	//������+����
	void Initialize(UINT texNumber,XMFLOAT2 position);

	//���_�o�b�t�@�f�[�^�]��
	void TransferVertices();

	//Set�֐�
	//�ʒu�ύX 
	void SetPosition(XMFLOAT2 position);
	//�傫���ύX
	void SetSize(float width, float height);
	//���E���]
	void SetisFlipX(bool isFlipX);
	//�㉺���]
	void SetisFlipY(bool isFlipY);
	//�A���J�[�|�C���g�̕ύX
	void SetAnchorPoint(XMFLOAT2 anchorPoint);
	//�e�N�X�`���͈͐ݒ�
	void SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height);
	//�e�N�X�`���ύX
	void SetTextureNum(int num);

	//�`��
	void Draw();
	
//�ÓI�����o�ϐ�
private:
	static Common* common;

	static const int texMax = 100;

//�����o�ϐ�
private:
	ComPtr<ID3D12Resource>vertBuff;
	ComPtr<ID3D12Resource>constBuff;
	D3D12_VERTEX_BUFFER_VIEW vBView{};

	float rotation = 0.0f;//Z�����̉�]�p
	XMFLOAT2 position{0,0};//���W
	XMMATRIX matWorld{};//���[���h�s��
	XMFLOAT4 color = { 1,1,1,1 };//�F
	UINT texNumber;//�e�N�X�`���ԍ�

	XMFLOAT2 anchorpoint{};
	//���_��
	static const int vertNum = 4;
	//��
	float width = 100.0f;
	//�c
	float height = 100.0f;
	//���E���]
	bool isFlipX = false;
	//�㉺���]
	bool isFlipY = false;
	//�e�N�X�`���؂���͈�
	float tex_x = 0.0f;
	float tex_y = 0.0f;
	float tex_width = 0.0f;
	float tex_height = 0.0f;


	//�X�v���C�g�p���_�f�[�^
	VertexPosUv vertices[4] = {
		{{0.0f,100.0f,0.0f},{0.0f,1.0f}},
		{{0.0f,0.0f,0.0f},{0.0f,0.0f}},
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},
		{{100.0f,0.0f,0.0f},{1.0f,0.0f}},
	};
	enum { LB, LT, RB, RT };

};