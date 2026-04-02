# include <Siv3D.hpp>

void Main()
{
	// 2D物理演算のシミュレーション空間（重力は下向きに 980）
	P2World world{ 980.0 };

	// 描画用のリスト
	Array<P2Body> bodies;

	// 線（床）の作成用
	Optional<Vec2> startPos;

	while (System::Update())
	{
		// 物理演算の更新
		world.update();

		// 左クリック：ボール（動体）を生成
		if (MouseL.down())
		{
			bodies << world.createCircle(P2Dynamic, Cursor::Pos(), 20, P2Material{ .restitution = 0.5 });
		}

		// 右クリック：ドラッグで床（静止体）を生成
		if (MouseR.down())
		{
			startPos = Cursor::Pos();
		}
		if (MouseR.up() && startPos)
		{
			const Vec2 endPos = Cursor::Pos();
			if (startPos->distanceFrom(endPos) > 5)
			{
				bodies << world.createLine(P2Static, Vec2{ 0, 0 }, Line{ *startPos, endPos }, P2Material{ .friction = 0.5 });
			}
			startPos.reset();
		}

		// 描画処理
		for (const auto& body : bodies)
		{
			body.draw(Palette::Skyblue);
		}

		// ドラッグ中のプレビュー線
		if (startPos)
		{
			Line{ *startPos, Cursor::Pos() }.draw(4, Palette::Orange);
		}

		// 説明文
		Print << U"左クリック: ボール作成";
		Print << U"右クリックドラッグ: 床を作成";
		Print << U"Cキー: クリア";

		if (KeyC.down())
		{
			bodies.clear();
		}
	}
}
