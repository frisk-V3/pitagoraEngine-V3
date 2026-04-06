using Siv3D;

using var window = new Window("ピタゴラエンジンv3");
var world = new P2World(980.0);
var bodies = new List<P2Body>();
var conveyors = new List<P2Body>();
Vec2? startPos = null;

while (System.Update())
{
    world.Update();

    // 左：光る弾を乱射
    if (MouseL.Pressed())
    {
        var ball = world.CreateCircle(P2BodyType.Dynamic, Cursor.Pos(), Random.Range(10, 25), new P2Material { Restitution = 0.8, Friction = 0.1 });
        bodies.Add(ball);
    }

    // 右：加速コンベア
    if (MouseR.Down()) startPos = Cursor.Pos();
    if (MouseR.Up() && startPos.HasValue)
    {
        if (startPos.Value.DistanceFrom(Cursor.Pos()) > 5)
        {
            var line = world.CreateLine(P2BodyType.Static, Vec2.Zero, new Line(startPos.Value, Cursor.Pos()), new P2Material { Friction = 0.0 });
            conveyors.Add(line);
        }
        startPos = null;
    }

    // スペース：マウス位置で大爆発
    if (KeySpace.Down())
    {
        foreach (var b in bodies) 
            b.ApplyImpulse((b.Pos - Cursor.Pos()).SetLength(5000));
    }

    // コンベアの加速処理
    foreach (var conv in conveyors)
        foreach (var b in bodies)
            if (conv.Intersects(b)) b.ApplyForce(new Vec2(2000, 0).Rotate(conv.Angle));

    // 画面外の掃除
    bodies.RemoveAll(b => b.Pos.Y > 800);

    // 描画（ネオンエフェクト）
    foreach (var c in conveyors) c.Draw(LineStyle.Dash(Scene.Time() * -5), 8, Palette.Cyan);
    foreach (var b in bodies) b.Draw(Color.FromHSV(b.Pos.Y, 0.8, 1.0)).DrawFrame(2, Palette.White);
    if (startPos.HasValue) new Line(startPos.Value, Cursor.Pos()).Draw(6, Palette.Orange);

    SimplePrint.At(10, 10).Text = $"物体数: {bodies.Count} | Spaceで爆破 | Cでクリア";
    if (KeyC.Down()) { bodies.Clear(); conveyors.Clear(); }
}
