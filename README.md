# Uncarved

Uncarved は、**C++20 と SDL3** を使用して個人で開発している 2D ゲームエンジンです。現在も開発を続けています。

---

## CUI ゲームから Engine Runtime へ

Uncarved は、当初のコンソール向けテキストゲームから発展してきました。SDL3、リソースの読み込み、描画などの機能を追加するにつれ、モジュール間の依存関係やデータの所有権、外部 API への依存が課題になりました。

そこで、SDL3 の機能を Platform 層にまとめ、World の状態管理を見直し、ゲーム固有の処理をより汎用的な Scene Runtime へと変更しました。

さらに、**Engine と Sandbox の役割を分離**しました。Engine は初期化、初期シーンの読み込みと World の管理、メインループなどを担当し、Sandbox は Application インターフェースと RuntimeContext を通してゲーム固有の処理を実装します。

これにより、**エンジンの実行処理とアプリケーション固有のロジックを分離**し、新しい機能を独立した Sandbox で検証できるようになりました。

**主な変更履歴：**

- [Hide SDL behind Uncarved APIs](https://github.com/bruceyan81/Uncarved/commit/8f731fb1712b66ca7e4764fce14fc514735d4807)
- [Replace game flow with generic scene loop](https://github.com/bruceyan81/Uncarved/commit/97dd5b52c934c40308c4d2632caca4e56ad2ba7e)
- [Establish Engine and Sandbox application boundary](https://github.com/bruceyan81/Uncarved/commit/e0c43d27210e0811af532f305cbb6d8e3a1d3e3d)

---

## Data Pipeline / 2D Rendering

### Data Pipeline

JSON の設定データから実行時の Actor を生成するまでには、複数の処理が必要です。開発中、リソース形式や内部データ構造を変更すると、長い処理経路の各所を修正する必要があり、変更漏れが起きやすいという課題がありました。

そこで、型付きのデータ定義やエラー内容を区別できる結果型、データの検証処理を導入し、外部データの読み込みと実行時の World 構築を分離しました。

現在は、設定、Sprite、Scene のデータから実行時のオブジェクトを生成する基本的な仕組みができています。

**関連コード：** [GameContentLoader.cpp](src/Content/GameContentLoader.cpp)

### 2D Rendering

Camera や Sprite の描画を実装する中で、座標空間や単位、座標変換の関係を明確にする必要がありました。

そこで、World、Camera、Viewport などの概念と内部の取り決めを整理し、Camera2D と SceneRenderer で座標変換、描画情報の構成、描画順の制御を行うようにしました。

これにより、**シーンデータから描画情報を構成する処理と、低レベルの描画 API の呼び出しを分離**しています。

**関連コード：** [SceneRenderer.cpp](src/View/SceneRenderer.cpp)

---

## Collision System：空間インデックスと設計上の判断

### 衝突判定と Spatial Hash Grid

現在の Collision System は AABB をベースとし、Collision Filter と、Solid / Sensor の 2 種類の衝突モードに対応しています。

基本的な重なり判定を実装した後、衝突候補を絞り込むために **Spatial Hash Grid** を追加しました。

実装中には、World Space 上では有効でも、通常の Grid では Cell を列挙できないケースへの対応が必要になりました。

そこで、Global Shape や全件走査へのフォールバックを用意し、空間インデックスの制約によって有効な衝突範囲が扱えなくならないようにしました。

なお、現時点では Benchmark による具体的な性能向上の検証は行っていません。

### 状態の整合性と設計の複雑さ

Spatial Hash Grid を実行時の処理に組み込む際、複数の衝突データの整合性を保つための設計が複雑になり、実装が進めにくくなりました。

そこで、一度作業を止め、AI の協力も得ながら設計を見直しました。正常に更新が完了した場合の整合性は確保しつつ、回復不能な故障に対して複雑な強いロールバック保証を設けない方針に変更しました。

その結果、空間インデックスのライフサイクル管理と、更新失敗を Runtime に伝える処理を実装できました。

**主な変更履歴：**

- [Add spatial hash grid indexing](https://github.com/bruceyan81/Uncarved/commit/896fb1bde2ecc9a9e3fffa70e0a201b2441b92f0)
- [Synchronize spatial index lifecycle](https://github.com/bruceyan81/Uncarved/commit/1af32c4d534e4f5eeda84b38660d61b3e7c1ea0a)
- [Terminate on application update failure](https://github.com/bruceyan81/Uncarved/commit/6ce1f2a09e7b3e091a8a8cbc7e9a9a93545304dd)

---

## Sandbox / Current Status

Sandbox は、Uncarved のエンジン API を使って制作した動作検証用のアプリケーションです。

現在は、キーボードによるキャラクター移動、Sprite の切り替え、Solid による移動の制限、Sensor による重なり判定などを実装しています。

これらを通して、Input、Runtime、World、Rendering、Collision の各システムが連携して動作することを確認しています。

Uncarved はまだ開発途中であり、物理シミュレーション、UI、より充実した入力機能などは未完成です。音声やテキストについても、一部の基盤機能は実装していますが、現在の Sandbox には十分に統合されていません。

**関連コード：** [SandboxApplication.cpp](Sandbox/src/SandboxApplication.cpp)

---

## AI について

Uncarved のソースコードの大部分は自分で書いています。

開発では、技術的な相談、設計の検討、コードレビュー、一部の実装に生成 AI を活用しています。

Engine と Sandbox の境界設計や Collision のエラー処理についても、AI の提案を参考にしながら方針を検討し、実装を進めました。

---

## License / Third-Party Notices

本プロジェクトは Apache License 2.0 で公開しています。

ライセンスの詳細、および使用している第三者のライブラリやリソースについては、[LICENSE](LICENSE) と [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) を参照してください。

---

## Acknowledgements

### 日本語

本プロジェクトは、ミシガン大学の公開講義 [EECS 404: Game Engine Architecture](https://eecs404.com/) の教材を学習の出発点としています。

講義および教材を公開してくださった [Austin Yarger](https://ayarger.engin.umich.edu/eecs-498-game-engine-architecture/) 氏に感謝いたします。

### English

Uncarved began with the publicly available materials for [EECS 404: Game Engine Architecture](https://eecs404.com/) at the University of Michigan, which provided the initial foundation for the project.

Special thanks to [Austin Yarger](https://ayarger.engin.umich.edu/eecs-498-game-engine-architecture/) for making the course and its materials publicly available.
