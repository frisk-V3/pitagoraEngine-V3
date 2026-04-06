# include <Siv3D.hpp>

void Main()
{
	Window::Resize(1280, 720);
	P2World world{ 980.0 };
	Array<P2Body> bodies;
	
	// 加速床（コンベア）を区別するためのリスト
	Array<P2Body> conveyors;
	Optional<Vec2> startPos;

	while (System::Update())
	{
		world.update();

		// --- 入力セクション ---

		// 左クリック：高反発な「光る弾」を連射
		if (MouseL.pressed()) 
		{
			auto ball = world.createCircle(P2Dynamic, Cursor::Pos(), Random(10, 25), P2Material{ .restitution = 0.8, .friction = 0.1 });
			ball.setAngularVelocity(Random(-10.0, 10.0));
			bodies << ball;
		}

		// 右クリック：加速コンベアを設置
		if (MouseR.down()) startPos = Cursor::Pos();
		if (MouseR.up() && startPos)
		{
			if (startPos->distanceFrom(Cursor::Pos()) > 5)
			{
				auto line = world.createLine(P2Static, Vec2{ 0, 0 }, Line{ *startPos, Cursor::Pos() }, P2Material{ .friction = 0.0 });
				conveyors << line;
			}
			startPos.reset();
		}

		// スペースキー：全オブジェクトを爆破（中心から外へ力を加える）
		if (KeySpace.down())
		{
			for (auto& body : bodies)
			{
				Vec2 dir = (body.getPos() - Cursor::Pos()).setLength(5000);
				body.applyImpulse(dir);
			}
		}

		// Cキー：全消去
		if (KeyC.down()) { bodies.clear(); conveyors.clear(); }

		// --- 物理・特殊処理 ---

		// コンベアに触れている物体を強制加速
		for (auto& conv : conveyors)
		{
			for (auto& body : bodies)
			{
				if (conv.intersects(body))
				{
					// 線の方向に沿って加速
					body.applyForce(Vec2{ 2000, 0 }.rotate(conv.getAngle()));
				}
			}
		}

		// 画面外（下）に落ちた体を削除してメモリ節約
		bodies.remove_if([](const P2Body& b) { return b.getPos().y > 800; });

		// --- 描画セクション ---

		// コンベア（流れるアニメーション）
		for (const auto& conv : conveyors)
		{
			conv.draw(LineStyle::Dash(Scene::Time() * -5), 8, Palette::Cyan);
		}

		// 動体（ネオンカラー）
		for (const auto& body : bodies)
		{
			body.draw(HSV{ body.getPos().y, 0.8, 1.0 }).drawFrame(2, Palette::White);
		}

		// プレビュー
		if (startPos) Line{ *startPos, Cursor::Pos() }.draw(6, Palette::Orange);

		// UI
		Print << U"左ドラッグ: 弾丸乱射";
		Print << U"右ドラッグ: 加速床(コンベア)作成";
		Print << U"Space: マウス位置で爆破！";
		Print << U"物体数: " << bodies.size();
	}
}
