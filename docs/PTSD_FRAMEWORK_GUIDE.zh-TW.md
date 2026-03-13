# PTSD 框架詳細說明

這份文件是針對目前專案內建的 `PTSD` 框架整理的實作導向說明。目標不是列出所有 API，而是讓你知道：

- `PTSD` 幫你處理了什麼
- 你可以拿哪些工具來做《元氣騎士》這類 2D 遊戲
- 哪些功能已經有了，哪些還要你自己補
- 在這個專案裡，推薦怎麼組織程式

---

## 1. PTSD 是什麼

`PTSD` 全名是 `Practical Tools for Simple Design`。從原始碼來看，它本質上是：

- 用 `SDL2` 管視窗、事件、圖片、字型、音效
- 用 `OpenGL` 做繪圖
- 用 `glm` 做 2D 轉換矩陣
- 內建 `Dear ImGui` 當除錯 UI
- 提供一層較容易使用的 2D 遊戲工具類別

它比較像「2D 遊戲底層工具包」，不是完整商業遊戲引擎。它有畫圖、輸入、時間、音效、動畫、資源快取，但沒有直接幫你做好：

- 碰撞系統
- 物理系統
- Tilemap 編輯器
- 路徑搜尋
- ECS
- 完整場景管理器
- 存檔系統

所以你可以把它理解成：

`PTSD = 遊戲底層 + 畫面/輸入/音效工具`

而《元氣騎士》的玩法邏輯、角色系統、武器系統、地圖房間、碰撞判定，仍然要你在專案內自己設計。

---

## 2. 框架整體分層

目前這套程式大致分成兩層：

### 2.1 PTSD 的底層模組

- `Core/*`
  - 偏 OpenGL/視窗層
  - 包含 `Context`、`Program`、`Texture`、`VertexArray`、`UniformBuffer`
- `Util/*`
  - 偏遊戲開發常用工具
  - 包含 `GameObject`、`Renderer`、`Image`、`Text`、`Animation`、`Input`、`Time`、`BGM`、`SFX`

### 2.2 你專案自己的遊戲邏輯層

你的專案目前已經在這層做了自己的抽象：

- `App`
  - 負責整體程式狀態
- `Scene`
  - 你自定義的場景基底
- `MainMenu`
  - 主選單場景
- `MapTest`
  - 測試地圖場景
- `Component/*`
  - 你自己的按鈕、玩家、地圖等元件

這樣的方向是對的。之後做《元氣騎士》時，建議延續：

- `PTSD` 負責底層能力
- 你的專案負責玩法與架構

---

## 3. 你每天最常碰到的 3 個核心類別

如果只先記住三個類別，請先掌握：

- `Core::Context`
- `Util::GameObject`
- `Util::Renderer`

它們大致對應：

- `Context`：開視窗、初始化框架、維護每幀流程
- `GameObject`：一個會被畫出來的遊戲物件
- `Renderer`：把很多 `GameObject` 依照 z-index 畫到畫面上

---

## 4. Core::Context：主迴圈與視窗管理

檔案：

- `PTSD/include/Core/Context.hpp`
- `PTSD/src/Core/Context.cpp`

### 4.1 它負責什麼

`Core::Context` 在建立時會初始化：

- SDL 視窗
- OpenGL Context
- SDL_image
- SDL_ttf
- SDL_mixer
- ImGui
- logger

也就是說，只要你有：

```cpp
auto context = Core::Context::GetInstance();
```

整個框架所需的基礎環境就會被拉起來。

### 4.2 重要功能

- `GetInstance()`
  - 取得單例
- `GetExit() / SetExit(bool)`
  - 控制程式是否結束
- `SetWindowIcon(path)`
  - 設定視窗圖示
- `Setup()`
  - 準備本幀的 ImGui / render frame
- `Update()`
  - 更新輸入、交換畫面、清除畫面、更新 delta time、執行 FPS cap

### 4.3 建議怎麼用

標準主迴圈概念應該像 PTSD 範例那樣：

```cpp
auto context = Core::Context::GetInstance();

while (!context->GetExit()) {
    context->Setup();

    // 你的 app / scene update

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    context->Update();
}
```

### 4.4 你目前專案的狀況

你現在的 [`src/main.cpp`](../src/main.cpp) 已經有基本主迴圈，但沒有呼叫 `context->Setup()`，也沒有執行 ImGui render。

這代表：

- 一般圖片/文字/動畫繪製仍然可以跑
- 但如果你想正式用 ImGui 做 debug window，應回到 PTSD 範例那種寫法

### 4.5 適合拿來做什麼

- 啟動遊戲
- 控制整個程式何時結束
- 掛視窗 icon
- 控制 FPS cap 與每幀更新節奏
- 將 ImGui 納入除錯流程

---

## 5. Util::GameObject：遊戲物件基底

檔案：

- `PTSD/include/Util/GameObject.hpp`
- `PTSD/src/Util/GameObject.cpp`

### 5.1 它是什麼

`Util::GameObject` 是 PTSD 最重要的遊戲物件容器。它本身不定義玩法，但提供一個物件該有的基本資料：

- `m_Transform`
  - 位置、旋轉、縮放
- `m_Drawable`
  - 真正負責畫圖的物件
- `m_ZIndex`
  - 繪製層級
- `m_Visible`
  - 是否顯示
- `m_Pivot`
  - 旋轉/縮放時的基準點調整
- `m_Children`
  - 子物件

### 5.2 實務上的角色

你通常不會直接把所有邏輯寫在 `Drawable` 裡，而是會：

1. 繼承 `GameObject`
2. 在裡面放自己的狀態
3. 用 `SetDrawable()` 指定圖片、文字或動畫
4. 在自己的 `Update()` 裡更新位置、狀態、碰撞、攻擊

例如：

```cpp
class Player : public Util::GameObject {
public:
    void Start() {
        SetDrawable(std::make_shared<Util::Animation>(paths, true, 80));
    }

    void Update() {
        if (Util::Input::IsKeyPressed(Util::Keycode::W)) {
            m_Transform.translation.y += m_Speed * Util::Time::GetDeltaTimeMs();
        }
    }

private:
    float m_Speed = 0.3F;
};
```

### 5.3 常用方法

- `SetDrawable(drawable)`
  - 指定圖像、文字、動畫
- `SetZIndex(index)`
  - 設定圖層
- `SetVisible(bool)`
  - 控制顯示/隱藏
- `SetPivot(vec2)`
  - 調整 pivot
- `AddChild(child)`
  - 掛子物件
- `RemoveChild(child)`
  - 移除子物件
- `Draw()`
  - 由 `Renderer` 呼叫，通常不需要自己手動呼叫

### 5.4 適合拿來做什麼

- 玩家
- 敵人
- 子彈
- 武器
- 掉落物
- 寶箱
- 門
- UI 元件
- 地圖上的單一可見物件

---

## 6. Transform 與座標系統

相關檔案：

- `PTSD/include/Util/Transform.hpp`
- `PTSD/include/Util/TransformUtils.hpp`
- `PTSD/src/Util/TransformUtils.cpp`

### 6.1 Transform 內容

`Transform` 有三個欄位：

- `translation`
- `rotation`
- `scale`

### 6.2 重要規則

- `translation` 用來放 2D 位置
- `rotation` 單位是 radians，不是 degrees
- `scale` 是倍率，不是像素

### 6.3 PTSD 的畫面座標概念

從 `TransformUtils.cpp` 與 `Input.cpp` 可以看出，這套框架採用的是：

- 視窗中心為 `(0, 0)`
- `x` 往右為正
- `y` 往上為正

所以：

- 左上角大約是 `(-WINDOW_WIDTH/2, WINDOW_HEIGHT/2)`
- 右下角大約是 `(WINDOW_WIDTH/2, -WINDOW_HEIGHT/2)`

這對射擊遊戲很方便，因為：

- 角色站在中央房間比較直觀
- 計算朝向、位移、子彈方向會比左上角原點更自然

### 6.4 在《元氣騎士》裡怎麼用

- 玩家位置：`m_Transform.translation`
- 子彈速度方向：直接加到 `translation`
- 角色朝向：用 `rotation`
- 武器翻面或放大縮小：用 `scale`

---

## 7. Core::Drawable：真正負責畫圖的介面

檔案：

- `PTSD/include/Core/Drawable.hpp`

`Drawable` 是繪圖介面。它只要求兩件事：

- `Draw(const Core::Matrices&)`
- `GetSize() const`

PTSD 已經幫你準備了幾個最常用的 `Drawable` 實作：

- `Util::Image`
- `Util::Text`
- `Util::Animation`

你平常多半不需要自己寫 `Drawable`，除非你要：

- 自訂 shader 效果
- 自訂粒子系統
- 自訂幾何圖形
- 自訂進階 UI 元件

---

## 8. Util::Renderer：負責把 GameObject 畫出來

檔案：

- `PTSD/include/Util/Renderer.hpp`
- `PTSD/src/Util/Renderer.cpp`

### 8.1 它做什麼

`Renderer` 管理一組根物件，並在 `Update()` 時：

1. 蒐集所有 root child
2. 走訪它們的 children
3. 依 `z-index` 排序
4. 逐個呼叫 `Draw()`

### 8.2 重要觀念

`Renderer::Update()` 只負責畫，不負責遊戲邏輯更新。

也就是說：

- 你的 `Player::Update()`
- `Enemy::Update()`
- `Bullet::Update()`

這些仍要由你自己在場景或 app 裡手動呼叫。

### 8.3 目前實作上的限制

雖然 `GameObject` 有 `children`，但目前 `Renderer` 只把子物件加入繪製佇列，沒有把父物件 transform 疊加到子物件上。

這代表現在的 parent-child 關係比較像：

- 繪製管理上的群組

而不是：

- 完整的階層式座標系統

這點很重要。假設你把槍掛在玩家底下：

- 你不能期待玩家移動時，槍會自動跟著做父子 transform 疊加
- 目前仍需要你自己在 `Update()` 裡同步子物件位置

### 8.4 適合拿來做什麼

- 場景的根渲染器
- 管理主選單 UI
- 管理房間內所有可見物件
- 管理同一場景的玩家/敵人/子彈/掉落物

---

## 9. Util::Image：顯示圖片

檔案：

- `PTSD/include/Util/Image.hpp`
- `PTSD/src/Util/Image.cpp`

### 9.1 它做什麼

`Util::Image` 是最基本的可繪製圖片物件。你只要提供圖片路徑，就能把圖片當成 `Drawable` 掛到 `GameObject` 上。

```cpp
SetDrawable(std::make_shared<Util::Image>("Resources/Character/character.png"));
```

### 9.2 常用方法

- `Image(filepath)`
  - 讀入圖片
- `SetImage(filepath)`
  - 切換圖片
- `GetSize()`
  - 取得圖片大小

### 9.3 實際用途

- 靜態角色立繪
- 地圖磚塊
- UI 背景
- 房間門
- 箱子
- 地板/牆壁素材

### 9.4 內部特性

`Image` 會透過 `AssetStore` 快取 `SDL_Surface`。這表示同一路徑的圖片重複載入時，底層 surface 可以重用，避免每次都重新讀檔。

這很適合：

- 地圖上大量重複 tile
- 多顆同款子彈
- 多個敵人共用同一張素材

### 9.5 注意事項

如果圖片載入失敗，框架會改用 missing texture 並輸出 log，不會直接讓程式爆掉。

---

## 10. Util::Text：顯示文字

檔案：

- `PTSD/include/Util/Text.hpp`
- `PTSD/src/Util/Text.cpp`

### 10.1 它做什麼

`Util::Text` 用字型檔把文字轉成可繪製紋理。

建立方式：

```cpp
auto text = std::make_shared<Util::Text>(
    "Resources/Font/Cubic-Font/Cubic_11.ttf",
    24,
    "HP: 6",
    Util::Color::FromRGB(255, 255, 255)
);
```

### 10.2 常用方法

- `SetText(string)`
  - 更新文字內容
- `SetColor(color)`
  - 更新顏色
- `GetSize()`
  - 取得目前字塊大小

### 10.3 適合用在哪裡

- 血量顯示
- 金幣數字
- 傷害數字
- 房間提示
- 主選單文字
- 暫停選單
- debug 資訊

### 10.4 注意事項

`SetText()` 和 `SetColor()` 會重新生成文字貼圖，所以它不是免費操作。

實務建議：

- `HP: 6`、`Coins: 120` 這種 UI 很適合
- 每幀更新一大段長文字就不太適合
- 若只是數字偶爾改變，使用上沒問題

---

## 11. Util::Animation：逐張圖片動畫

檔案：

- `PTSD/include/Util/Animation.hpp`
- `PTSD/src/Util/Animation.cpp`

### 11.1 它做什麼

`Animation` 本質上是多張 `Image` 的序列播放器。你給它一串圖片路徑，它會依時間切換 frame。

```cpp
auto anim = std::make_shared<Util::Animation>(
    std::vector<std::string>{
        "run_0.png",
        "run_1.png",
        "run_2.png",
        "run_3.png",
    },
    true,
    80,
    true,
    100
);
```

參數概念：

- frame 圖片路徑陣列
- 是否一開始就播放
- frame 間隔毫秒
- 是否循環
- 循環之間的 cooldown

### 11.2 常用方法

- `Play()`
- `Pause()`
- `SetInterval(ms)`
- `SetLooping(bool)`
- `SetCooldown(ms)`
- `SetCurrentFrame(index)`
- `GetCurrentFrameIndex()`
- `GetState()`

### 11.3 很適合用在哪裡

- 玩家跑步動畫
- 敵人待機/移動動畫
- 爆炸動畫
- 開門動畫
- 拾取效果
- 子彈命中特效

### 11.4 目前實作上的重要特性

`Animation` 的更新是在 `Draw()` 裡執行，不是在外部獨立 update loop。

這意味著：

- 物件有被畫到時，動畫才會推進
- 物件如果被隱藏，動畫也不會前進

這對特效來說常常是可以接受的，但你要知道它和一般「邏輯更新與渲染更新分離」的引擎不同。

### 11.5 目前沒有什麼

`Animation` 是逐張圖片播放，不是 sprite sheet 切格系統。

所以如果你之後拿到的是一張大圖集：

- 你需要自己切圖
- 或自己擴充一個 sprite-sheet animation 類別

---

## 12. Util::Input：鍵盤與滑鼠輸入

檔案：

- `PTSD/include/Util/Input.hpp`
- `PTSD/include/Util/Keycode.hpp`
- `PTSD/src/Util/Input.cpp`

### 12.1 最常用的三種鍵盤判斷

- `IsKeyPressed(key)`
  - 目前這一幀是否處於按住狀態
- `IsKeyDown(key)`
  - 這一幀剛按下
- `IsKeyUp(key)`
  - 這一幀剛放開

例子：

```cpp
if (Util::Input::IsKeyPressed(Util::Keycode::W)) {
    // 持續往上走
}

if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
    // 只在按下那一刻開槍一次
}
```

### 12.2 滑鼠功能

- `GetCursorPosition()`
- `SetCursorPosition(pos)`
- `IfScroll()`
- `GetScrollDistance()`
- `IsMouseMoving()`

### 12.3 退出事件

- `IfExit()`
  - 視窗關閉時會變成 true

### 12.4 《元氣騎士》常見用途

- `WASD` 控制角色移動
- 滑鼠或方向鍵控制瞄準
- 滑鼠左鍵開槍
- `ESC` 開暫停
- 滾輪切武器
- debug 快捷鍵

### 12.5 重要座標注意

從目前 `Input.cpp` 的實作看，`GetCursorPosition()` 回傳的是「以畫面中心為原點」的座標，而不是左上角原點。

這件事很好用，但要保持一致：

- 玩家位置也是中心原點
- 滑鼠位置也是中心原點
- 這樣你在算瞄準方向時會很順

---

## 13. Util::Time：時間與 delta time

檔案：

- `PTSD/include/Util/Time.hpp`
- `PTSD/src/Util/Time.cpp`

### 13.1 常用方法

- `GetDeltaTimeMs()`
  - 取得上一幀到這一幀的毫秒差
- `GetElapsedTimeMs()`
  - 取得程式啟動後經過多久

### 13.2 怎麼用

#### 移動

```cpp
m_Transform.translation += velocity * Util::Time::GetDeltaTimeMs();
```

#### 計時器

```cpp
if (Util::Time::GetElapsedTimeMs() - m_LastFireTime >= m_FireCooldown) {
    Fire();
    m_LastFireTime = Util::Time::GetElapsedTimeMs();
}
```

### 13.3 在《元氣騎士》裡的用途

- 角色移動速度
- 子彈飛行
- 武器攻速
- 無敵時間
- 敵人 AI 冷卻
- 技能 CD
- 房間事件倒數

---

## 14. Util::BGM 與 Util::SFX：音樂與音效

檔案：

- `PTSD/include/Util/BGM.hpp`
- `PTSD/src/Util/BGM.cpp`
- `PTSD/include/Util/SFX.hpp`
- `PTSD/src/Util/SFX.cpp`

### 14.1 BGM

`BGM` 用來處理背景音樂，適合長音訊：

```cpp
Util::BGM bgm("Resources/Audio/menu.mp3");
bgm.SetVolume(64);
bgm.Play(-1);
```

常用方法：

- `Play(loop)`
- `FadeIn(ms, loop)`
- `FadeOut(ms)`
- `Pause()`
- `Resume()`
- `SetVolume(volume)`

### 14.2 SFX

`SFX` 用來處理短音效：

```cpp
Util::SFX shot("Resources/Audio/shot.wav");
shot.Play();
```

常用方法：

- `Play(loop, duration)`
- `FadeIn(tick, loop, duration)`
- `SetVolume(volume)`

### 14.3 在《元氣騎士》裡的典型用途

- `BGM`
  - 主選單音樂
  - 戰鬥房間音樂
  - Boss 房音樂
- `SFX`
  - 射擊聲
  - 爆炸聲
  - 受傷聲
  - 開門聲
  - 撿道具聲

### 14.4 注意事項

- `BGM` 一次只會有一個音樂主體在播
- `SFX` 比較適合短音效，多次觸發也比較合理
- 兩者底層都透過 `AssetStore` 做資源快取

---

## 15. Util::AssetStore：資源快取

檔案：

- `PTSD/include/Util/AssetStore.hpp`
- `PTSD/include/Util/AssetStore.inl`

### 15.1 它做什麼

`AssetStore<T>` 是 PTSD 的資源快取工具。概念是：

- 第一次拿資源時才真的讀檔
- 之後同一路徑直接回傳快取

### 15.2 為什麼重要

做《元氣騎士》會有很多重複資源：

- 同一種子彈圖片
- 同一種敵人圖
- 同一張地板 tile
- 同一個音效

沒有快取的話，你每產生一顆子彈都重新讀檔，效能會很差。

### 15.3 你通常需不需要直接碰它

大部分情況下不需要。因為：

- `Image`
- `BGM`
- `SFX`

已經在內部使用它。

只有當你要管理自己的大型資源時，才需要自己直接用 `AssetStore`。

---

## 16. Util::Color：顏色工具

檔案：

- `PTSD/include/Util/Color.hpp`
- `PTSD/src/Util/Color.cpp`

### 16.1 它能做什麼

`Util::Color` 幫你處理顏色表示與轉換。

可以從這些方式建立：

- `FromRGB`
- `FromHSL`
- `FromHSV`
- `FromHex`
- `FromName`

例子：

```cpp
Util::Color::FromRGB(255, 0, 0);
Util::Color::FromHex("FF0000FF");
Util::Color::FromName(Util::Colors::YELLOW);
```

### 16.2 適合用在哪裡

- UI 文字顏色
- 受傷提示字
- 稀有度顏色
- debug 顏色標記

---

## 17. Util::Logger：除錯輸出

檔案：

- `PTSD/include/Util/Logger.hpp`
- `PTSD/src/Util/Logger.cpp`

### 17.1 常用巨集

- `LOG_TRACE`
- `LOG_DEBUG`
- `LOG_INFO`
- `LOG_WARN`
- `LOG_ERROR`
- `LOG_CRITICAL`

### 17.2 適合用在哪裡

- 場景切換
- 角色生成/死亡
- 武器開火
- 碰撞發生
- 資源載入失敗
- AI 狀態切換

### 17.3 開發建議

做《元氣騎士》時，這些 log 很有價值：

- 玩家進入哪個房間
- 哪個敵人被生成
- 武器 CD 何時重置
- 子彈何時命中
- 掉落物何時被撿取

---

## 18. Core 低階工具：當預設功能不夠時再碰

相關檔案：

- `PTSD/include/Core/Program.hpp`
- `PTSD/include/Core/Shader.hpp`
- `PTSD/include/Core/Texture.hpp`
- `PTSD/include/Core/VertexArray.hpp`
- `PTSD/include/Core/VertexBuffer.hpp`
- `PTSD/include/Core/IndexBuffer.hpp`
- `PTSD/include/Core/UniformBuffer.hpp`

這一層比較接近 OpenGL 封裝。大多數玩法開發不需要先碰它，但如果你之後想做：

- 自訂 shader
- 命中閃白效果
- 受傷變色效果
- 特殊 UI shader
- 粒子效果
- 自訂幾何

就會用到這層。

### 18.1 什麼時候該留在 Util 層

如果你只是要：

- 顯示圖片
- 顯示字
- 播動畫
- 處理輸入
- 播音效

那就留在 `Util` 層即可。

### 18.2 什麼時候要往 Core 層走

如果你要：

- 改 shader pipeline
- 自己控制紋理上傳
- 自訂 draw call 行為
- 做框架級擴充

才需要碰 `Core`。

---

## 19. PTSD 在《元氣騎士》專案中的推薦分工

下面是很實際的對應方式。

### 19.1 玩家 Player

建議：

- `Player : public Util::GameObject`
- `SetDrawable(Image 或 Animation)`
- `Update()` 處理移動、受傷、武器、狀態切換

PTSD 可用工具：

- `Input`：讀 WASD / 攻擊鍵
- `Time`：移動與技能冷卻
- `Animation`：跑步、受傷、死亡
- `SFX`：射擊聲、受傷聲

### 19.2 敵人 Enemy

建議：

- `Enemy : public Util::GameObject`
- 自己加 HP、狀態、AI、速度

PTSD 可用工具：

- `Animation`：待機、移動、死亡
- `Time`：攻擊間隔、AI 冷卻
- `Renderer`：顯示到場景

### 19.3 子彈 Bullet

建議：

- `Bullet : public Util::GameObject`
- 內部有方向、速度、傷害、壽命

PTSD 可用工具：

- `Image` 或 `Animation`
- `Time`：壽命倒數
- `Renderer`：畫出來

你自己要補：

- 與牆或敵人的碰撞
- 命中後銷毀

### 19.4 武器 Weapon

武器不一定要是 `GameObject`。可以分兩種做法：

- 如果武器需要畫出來：做成 `GameObject`
- 如果武器只是邏輯資料：做成普通 C++ 類別

PTSD 可用工具：

- `SFX`：射擊音效
- `Time`：攻速/CD
- `Animation`：開火特效

### 19.5 地圖與房間

`PTSD` 沒有現成 tilemap 系統，所以房間地圖要你自己做。

你可以用目前專案已經開始建立的方向：

- `Map`
- `MapPiece`
- `Scene/MapTest`

實作方式通常會是：

- 每個 tile 一個 `GameObject`
- 或自己做一層 map renderer
- 門、牆、地板、障礙物各自有資料結構

PTSD 能提供的是：

- `Image`：顯示 tile
- `Renderer`：排序並畫出來
- `AssetStore`：重複圖塊共用資源

### 19.6 UI

PTSD 很適合拿來做基礎 UI：

- `Text`
- `Image`
- 你自己的 `TextButton`

可做：

- 主選單
- 血量欄
- 金幣
- 武器名稱
- 暫停選單
- 房間提示

### 19.7 場景切換

這部分目前主要是你專案自己的設計，不是 PTSD 直接提供。

你現在已有：

- `Scene`
- `MainMenu`
- `MapTest`
- `GetRedirection()`

這很適合擴展成：

- `MainMenuScene`
- `LobbyScene`
- `BattleRoomScene`
- `BossScene`
- `GameOverScene`

也就是說：

- `PTSD` 提供畫面與事件底層
- 你的 `Scene` 系統負責流程與玩法切換

---

## 20. 目前這套框架最值得注意的限制

這段很重要，因為之後做大專案時會直接影響架構。

### 20.1 Renderer 不負責 Update

`Renderer` 只畫圖，不會替你呼叫每個物件的 `Update()`。

所以你需要自己有：

- 場景層的更新列表
- 或 entity manager
- 或手動逐個更新

### 20.2 Child 不會自動繼承 Parent Transform

目前 `children` 比較像繪製群組，不是真正完整的父子座標系統。

對《元氣騎士》來說，這會影響：

- 玩家與武器的掛載
- 敵人與血條的掛載
- UI 組件的相對位置

如果你想做父子跟隨：

- 自己在 `Update()` 裡同步子物件位置
- 或之後擴充 `Renderer/GameObject` 的 transform 疊加

### 20.3 Animation 在 Draw 時才前進

如果物件不可見或沒有被畫到，動畫就不會前進。

這對特效是合理的，但你必須知道這不是一般引擎常見的獨立動畫系統。

### 20.4 沒有現成碰撞系統

你目前專案裡已經有往這方向發展的介面：

- `IStateful`
- `ICollidable`

這是正確方向。建議你把：

- Hitbox
- Hurtbox
- 地圖阻擋
- 子彈命中

都放在你自己的 gameplay layer 實作。

### 20.5 沒有現成場景資源生命週期管理

切場景時，要自己確保：

- 把場景的物件從 `Renderer` 移除
- 清掉不需要的物件
- 避免保留舊場景的 shared_ptr

你目前 `MainMenu::Dispose()` 已經示範了這種做法。

---

## 21. 推薦你的開發策略

如果你要用這套框架完成《元氣騎士》，建議依序做：

### 第一階段：先把畫面與流程打通

- 完成 `Scene` 切換
- 完成主選單
- 完成一個測試房間
- 讓玩家能移動

### 第二階段：完成最小可玩戰鬥

- 玩家射擊
- 子彈移動
- 敵人生成
- 碰撞判定
- HP 與死亡

### 第三階段：補系統層

- 武器系統
- 掉落系統
- 房間清怪開門
- UI 狀態列
- 音樂與音效

### 第四階段：再考慮框架擴充

- 父子 transform
- sprite sheet 動畫
- 更好的 entity 管理
- debug overlay / ImGui 工具

---

## 22. 結論：你應該怎麼理解 PTSD

最實際的理解方式是：

- `PTSD` 不是完整遊戲引擎
- 它是一套「足夠做 2D 遊戲原型與課程專案」的底層框架
- 它很適合做《元氣騎士》這種 2D 房間制射擊遊戲的基礎建設

你可以直接仰賴它的部分：

- 視窗與主迴圈
- 輸入
- 時間
- 圖片顯示
- 文字顯示
- 簡單動畫
- 音效
- 資源快取
- debug log

你需要自己實作的部分：

- 場景規則
- 玩家與敵人邏輯
- 武器系統
- 子彈與碰撞
- 房間生成
- 地圖資料
- 存檔與流程控制

如果之後你要把這份文件繼續往下擴充，最值得再補的兩份是：

- 「PTSD + 目前專案架構的實戰範例」
- 「如何用 PTSD 實作玩家、敵人、子彈、地圖的模板」
