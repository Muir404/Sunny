# SunnyLand
基于 SDL3、SDL3_mixer、SDL3_ttf 开发的 2D 横版游戏项目，本分支适配了 SDL3_mixer 新接口，保证与最新 SDL 生态兼容。

## 特性
- 适配 SDL3_mixer 全新 API，保留音频音量控制、淡入淡出等核心功能
- 模块化引擎架构，核心逻辑与游戏业务解耦
- 基于 JSON 的可配置系统（窗口、音频、输入映射等）
- 场景管理、2D 渲染、物理碰撞、AI 行为等完整游戏引擎能力
- CC0 1.0 协议授权，可自由修改、分发、商用

## 构建运行
### 前置依赖
- C++20 编译器、CMake 3.21+
- SDL3、SDL3_mixer、SDL3_ttf 开发库

### 步骤
```bash
# 克隆代码
git clone https://github.com//SunnyLand.git
cd SunnyLand

# 编译构建
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 运行
# Linux/macOS
./build/SunnyLand
# Windows
build\Release\SunnyLand.exe
```

## 核心功能
- **场景系统**：标题、游戏、帮助、结束等多场景切换
- **音频管理**：背景音乐/音效播放、音量调节（适配 SDL3_mixer 新接口）
- **UI 系统**：按钮、标签、图片等可复用 UI 组件
- **输入系统**：基于动作的可配置按键映射
- **资源管理**：字体、纹理、音频等资源缓存与自动释放

## 协议
本项目采用 [CC0 1.0 Universal](LICENSE) 协议，可无限制使用、修改和分发，无需署名。

## 致谢
- 原项目：[WispSnow/SunnyLand](https://github.com/WispSnow/SunnyLand)
- SDL3 及扩展库：[libsdl.org](https://libsdl.org/)