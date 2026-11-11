# FBX形状読み込み用テストデータ

ufbx公式リポジトリの `data` から取得した、静的形状の読み込み確認用データです。FBXファイルは取得したバイト列のまま保存しています。

## 授業で使う順番

| 順番 | ファイル | 用途 | 頂点数 | 面数 | 三角形分割後 |
| --- | --- | --- | ---: | ---: | ---: |
| 1 | blender_272_cube_7400_binary.fbx | 最初の読み込み・四角形から三角形への分割 | 8 | 6 | 12 |
| 2 | blender_282_suzanne_7400_binary.fbx | サルの頭。複雑な形状と上下・前後の確認 | 507 | 500 | 968 |
| 3 | maya_cube_7400_ascii.fbx | ASCII形式と単位変換の確認 | 8 | 6 | 12 |

頂点数は `mesh->num_vertices`（位置の頂点数）です。法線・UVを含めて描画用頂点を分割した場合の頂点数とは異なります。いずれもメッシュは1個です。形状確認では、FBX内のマテリアルを使わず単色で描画できます。

## 読み込み確認結果

プロジェクト内の `ufbx.c` / `ufbx.h` を別のコンソール検証プログラムに組み込み、次の条件で実際に読み込み・全ての面の三角形分割・頂点インデックスの範囲を確認しました。ゲーム画面での描画確認はまだ行っていません。

```cpp
ufbx_load_opts opts = {};
opts.target_axes = ufbx_axes_left_handed_y_up;
opts.target_unit_meters = 1.0;
```

`node->geometry_to_world` を各位置に適用した後の範囲（単位m）は次のとおりです。

| ファイル | 最小座標 (x, y, z) | 最大座標 (x, y, z) |
| --- | --- | --- |
| blender_272_cube_7400_binary.fbx | (-1, -1, -1) | (1, 1, 1) |
| blender_282_suzanne_7400_binary.fbx | (-1.36719, -0.984375, -0.851562) | (1.36719, 0.984375, 0.851563) |
| maya_cube_7400_ascii.fbx | (-0.005, -0.005, -0.005) | (0.005, 0.005, 0.005) |

ASCII版の立方体は1辺1cmです。メートルに統一すると非常に小さくなるので、表示時にTransformのscale_を (100, 100, 100) にすると1辺1mになります。最初の授業ではBlender版の立方体を使ってください。

プロジェクトのフォルダーを作業ディレクトリとする場合の読み込みパス:

```cpp
"Assets/FBXTest/blender_272_cube_7400_binary.fbx"
```

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
