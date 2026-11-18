# 静的FBXの描画 · 形状編 改訂版

Visual Studioで `gameWindow.slnx` を開き、Debug / x64 でビルドして実行します。教材の [textured_cube.fbx](TexturedCube/textured_cube.fbx) は、モデラーで三角形化・単一メッシュ化済みです。位置は8個、面は12個、面の角は36個です。FBX上では一辺1cmなので、`gameWindow.cpp` は表示時に `Transform` のスケールを100にしています。

## 工程1〜6 · まず形状を描く

単色描画までのコードはコミット `23c94f4` にあります。同じFBXを使うので、以降も位置と面の構成を比較できます。

1. `ufbx_load_file` に左手系・Y軸上向き・1単位1mを指定して、シーンを開きます。
2. メッシュを持つノードを一つ選び、`geometry_to_world` を位置に適用します。
3. `mesh->faces` から三角形を読みます。`face.index_begin` は最初の角の番号、`mesh->vertex_indices` の値は角が参照する位置の番号です。
4. シーンを解放する前に必要なデータをコピーし、頂点バッファとインデックスバッファを作って描画します。

## 工程7 · マテリアル別に描く範囲を作る

`mesh->face_material` から、各面が参照する `node->materials` の番号を取得します。面の三つのインデックスをマテリアル別に集め、連結して一つのインデックスバッファに入れます。各範囲の `firstIndex` と `indexCount` を記録し、`DrawIndexed` で範囲ごとに描きます。未割り当てのメッシュは既定の一グループで扱います。

## 工程8 · Diffuse色と頂点カラーを表示する

`SHADER_3D` と `Simple3D.hlsl` に切り替えます。FBXのDiffuse色にDiffuse係数を掛け、マテリアルごとにシェーダーへ渡します。頂点カラーは `mesh->vertex_color` を面の角の番号で読み、三角形の内側で補間します。色が省略されている場合の既定値は白です。

ここでGPU頂点を位置単位から**面の角単位**に切り替えます。同じ位置を共有する二つの角に別々の頂点カラーを設定でき、次の工程では同じ仕組みでUVの継ぎ目を扱えます。FBX描画中は深度を使うため、各範囲を描く順番が変わっても手前の面が表示されます。

工程8の実行では画像をサンプリングせず、Diffuse色 × 頂点カラーで表示します。複数マテリアルの確認には、同じメッシュの面に赤と青のDiffuse色を割り当て、角の頂点カラーに白と別の色を設定した三角形化済みFBXを使います。

[テクスチャ編へ進む](TexturedCube/README.md)

## 以前の形状確認用データ

以下はufbx公式テストデータから取得した原本です。四角形を含むため、このクラスで描くときはモデラー側で三角形化して再出力します。

| ファイル | 位置数 | 面数 | 三角形化後の面数 |
| --- | ---: | ---: | ---: |
| blender_272_cube_7400_binary.fbx | 8 | 6 | 12 |
| blender_282_suzanne_7400_binary.fbx | 507 | 500 | 968 |
| maya_cube_7400_ascii.fbx | 8 | 6 | 12 |

## 入手元とライセンス

取得元: https://github.com/ufbx/ufbx/tree/main/data

リポジトリが提示するライセンスはMITまたはPublic Domain（Unlicense）の選択式です。原文を `LICENSE.ufbx.txt` に同梱しています。

https://github.com/ufbx/ufbx/blob/main/LICENSE

### 元ファイルと内容識別用Git blob SHA-1

- https://github.com/ufbx/ufbx/blob/main/data/blender_272_cube_7400_binary.fbx
  - `4fa2695a48c14c21937593b8d1a075da937b7cce`
- https://github.com/ufbx/ufbx/blob/main/data/blender_282_suzanne_7400_binary.fbx
  - `f7fcc0eaa86a6f88ca341c1d18cf0a55db44826f`
- https://github.com/ufbx/ufbx/blob/main/data/maya_cube_7400_ascii.fbx
  - `fa0a016ac563514f7c02e1018e594dce6b6a5551`

保存後に `git hash-object` で元ファイルの識別値との一致を確認しています。
