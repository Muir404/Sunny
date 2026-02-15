# Sunny
基于 SDL3、SDL3_mixer、SDL3_ttf、glm、spdlog、json 开发的轻量级 2D 平台跳跃游戏，在原项目基础上完成了音频模块对 SDL3_mixer 新接口的适配，保持核心玩法与架构不变。

## 特性
- 适配 SDL3_mixer 新接口的音频系统，兼容 SDL3 生态
- 轻量化 2D 平台跳跃核心玩法，操作简洁易上手
- 遵循 CC0 1.0 通用公共领域协议，可自由修改、分发、商用，无需署名

## 构建运行
### 前置依赖
- 环境：支持 C++20 的编译器（GCC/Clang/MSVC）、CMake 3.21 及以上
- 库依赖：SDL3、SDL3_mixer、SDL3_ttf、glm、spdlog、nlohmann/json 开发库

### 快速构建&运行
```bash
# 克隆代码
git clone https://github.com/Muir404/Sunny.git
cd Sunny

# 编译构建（Release 模式）
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc) --config Release

# 运行程序
# Linux/macOS
./build/SunnyLand-Linux
# Windows
build\Release\SunnyLand-Windows.exe
```

## 核心功能
- **多场景管理**：支持标题、游戏、帮助、结算等场景的无缝切换
- **适配版音频系统**：基于 SDL3_mixer 新接口实现背景音乐/音效播放、音量调节，兼容 SDL3 特性
- **通用 UI 组件**：封装按钮、文本标签、图片等可复用 UI 元素，快速搭建界面
- **可配置输入系统**：基于动作映射的按键配置，支持自定义操作键位
- **自动化资源管理**：对字体、纹理、音频等资源进行缓存，自动释放避免内存泄漏

## 协议
本项目采用 [CC0 1.0 Universal](LICENSE) 协议授权，你可无限制使用、修改、分发本项目的全部内容，无需标注原作者或来源。

## 致谢
- 原项目：[WispSnow/SunnyLand](https://github.com/WispSnow/SunnyLand)，本项目主要完成音频模块对 SDL3_mixer 接口的适配，其余核心逻辑参考原项目实现。

### 总结
1. 核心改动：聚焦音频模块，完成对 SDL3_mixer 新接口的适配，其余功能保持原项目架构；
2. 构建要求：需满足 C++20 编译环境及 SDL3 系列依赖库，构建命令跨平台通用；
3. 授权规则：遵循 CC0 1.0 协议，商用、修改、分发均无限制，无需署名。