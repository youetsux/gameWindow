# 同じフォルダーのPNGを使うテクスチャ付きFBX

`textured_cube.fbx` と `checkerboard_*.png` は、全て同じフォルダーに置いて使います。

## 最初の授業で使うファイル

- モデル: `textured_cube.fbx`
- 色（Diffuse）: `checkerboard_diffuse.png`

チェック柄と「DIFFUSE」の文字が入った画像を使う立方体です。1メッシュ、1マテリアル、位置8頂点、12面（全て三角形）、インデックス36個。出力済みのFBXが三角形だけになるよう準備してあり、読み込み側は分割しません。UV・法線も含まれています。

残りの5枚はFBXが参照する他のチャンネル用画像です。発光・環境色・鏡面反射・透明度・反射の学習用として同梱していますが、最初のテクスチャ描画ではDiffuseだけを使います。

## パスの扱い

読み込みパス（プロジェクトフォルダーからの相対パス）:

```cpp
"Assets/FBXTest/TexturedCube/textured_cube.fbx"
```

FBX内の画像参照は、絶対パスやサブフォルダーを含まない画像ファイル名に統一しています。モデルのあるフォルダーと画像名を結合して読み込みます。カレントディレクトリ直下から画像を探す方式にはしません。

```cpp
// material は対象メッシュに割り当てられた ufbx_material*
ufbx_texture* texture = material->fbx.diffuse_color.texture;
if (texture) {
    std::filesystem::path imagePath =
        std::filesystem::path(fbxFileName).parent_path()
        / std::filesystem::path(texture->relative_filename.data).filename();
    // imagePath を画像読み込み処理に渡す
}
```

この例には `<filesystem>` と C++17以降が必要です。ufbxは画像の参照を取り出しますが、PNGのデコードやDirect3Dのテクスチャ生成は別途必要です。

## 大きさ

元モデルは1辺1cmです。`target_unit_meters = 1.0` で単位をメートルに揃え、`geometry_to_world` を適用すると座標範囲は各軸 -0.005〜0.005 になります。Transformの `scale_` を (100, 100, 100) にすれば1辺1mとして表示できます。

## 確認済みの内容

プロジェクト内のufbxを使うコンソール検証プログラムで、次を確認しました。

- FBXの読み込み、12面全てが3頂点の三角形であること。
- UVと法線の存在、全ポリゴン頂点のUVインデックスと有限値。
- メッシュへのマテリアル割り当てとDiffuse画像への接続。
- 全6枚の画像がFBXと同じフォルダーに存在し、PNGヘッダーを持つこと。
- 各テクスチャの埋め込みデータが0バイトであること。

Diffuse画像は画像として開けることも確認済みです。ゲーム側でのテクスチャ付き描画はまだ未実装・未確認です。

## 入手元と変更点

ufbx公式テストデータ:

https://github.com/ufbx/ufbx/blob/main/data/maya_textured_cube_7500_ascii.fbx

https://github.com/ufbx/ufbx/tree/main/data/textures

元FBXのGit blob SHA-1: `cb997c1890934eecc271a10782e44368056ac469`

元のASCII形式FBXに対し、画像パス36か所をファイル名だけに変更し、6枚の埋め込み画像データを削除しています。配布素材の準備段階で、四角形6面を三角形12面へ変更し、面の角に対応する法線・UVインデックスも並べ直しています。位置座標、UV値、マテリアルの接続は維持しています。この処理はゲーム側には含めていません。PNGは元ファイルのままです。

ライセンス原文は、このフォルダーと親フォルダーの `LICENSE.ufbx.txt` に同梱しています（MITまたはUnlicenseの選択式）。
