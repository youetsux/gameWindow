#include "Camera.h"

namespace
{
	XMVECTOR DEF_POS{ 0, 3, -10, 0 };	//カメラの位置
	XMVECTOR DEF_TGT{ 0, 0, 0, 0 };	//カメラの焦点
	float DEF_FOV{ XM_PIDIV4 };	//視野角 4/π＝45度
	float DEF_WIN_WIDTH{ 800.0f };	//ウィンドウの幅
	float DEF_WIN_HEIGHT{ 600.0f };	//ウィンドウの高さ
	float DEF_ASPECT{ DEF_WIN_WIDTH / DEF_WIN_HEIGHT };	//アスペクト比
	float DEF_NEAR{ 0.1f };	//近くのクリップ面
	float DEF_FAR{ 100.0f };	//遠くのクリップ面
	XMVECTOR UP_DIR{ 0, 1, 0, 0 };	//上方向ベクトル
 }

//変数
XMVECTOR position_;	//カメラの位置（視点）
XMVECTOR target_;	//見る位置（焦点）
XMMATRIX viewMatrix_;	//ビュー行列　4x4の行列
XMMATRIX projMatrix_;	//プロジェクション行列 4x4の行列



//初期化
void Camera::Initialize()
{
	position_ = DEF_POS;	//カメラの位置
	target_ = DEF_TGT;	//カメラの焦点
	projMatrix_ = XMMatrixPerspectiveFovLH(DEF_FOV, DEF_ASPECT, DEF_NEAR, DEF_FAR);	//射影行列
}

//更新
void Camera::Update()
{
	//ビュー行列の作成
	viewMatrix_ = XMMatrixLookAtLH(position_, target_, UP_DIR);
}

//位置を設定
void Camera::SetPosition(XMVECTOR position)
{
	position_ = position;
}

//焦点を設定
void Camera::SetTarget(XMVECTOR target)
{
	target_ = target;
}

//ビュー行列を取得
XMMATRIX Camera::GetViewMatrix()
{
	return viewMatrix_;
}

//プロジェクション行列を取得
XMMATRIX Camera::GetProjectionMatrix()
{
	return projMatrix_;
}