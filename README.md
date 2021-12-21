Please read [Coding guide](plrqguide.md) when make your pull-request.

# chang: A Typed Script Language
yeah.

# 基本的な言語仕様
関数型。スコープでは、その中の最後に評価されたものがそのスコープの値となる。

# 構文
## 関数定義
```
fn add(a: int, b: int) -> int {
  a + b
}

fn nothing() -> none {
  add(1, 2);
  // 最後にセミコロンつけたら関数の戻り値にはならない。
}

fn my_func() { // 型が省略されると none になります
  1;
}
```