## 简介
> 内嵌字幕提取器（Embed-Subtitle-Extractor-Lite） 旨在打造一款简单实用且快速地识别视频内嵌字幕文本的一款工具。

### 产品架构
- 用户界面使用QtCpp编写

- 后台服务使用Flask进行图像的识别（调用了百度Paddle API）

> **注意：源码里面包含了Qt和Flask**
> **目前Window平台经过测试可用，其他平台暂未开启测试**

### 特点
- 操作简单
- 快速
- 轻量
- 仅需CPU
- 支持大部分语言

### 使用说明

- 方式一：使用已经部署在云服务器上的后台服务（易上手;快速使用;CPU）
  1. 下载Releases中最新的压缩包（.zip）
  2. 解压后双击exe文件即可使用

- 方式二：使用本地计算机来后台服务（麻烦;CPU;GPU）

> 如果云服务器上的后台服务版本无法满足需要的识别精度，可以使用自定义的本地服务来提高识别精度
> 自己想动手本地部署的也可以使用这个方式

   1. 和前一个使用一样，在Releases下载最新的压缩包
   2. 克隆项目源码（注意这里）
     ```
     git clone https://github.com/capp-adocia/Embed-Subtitle-Extractor-Lite.git
     ```
   3. 安装所需要的pip包
      > 注意这里pip包是对Windows平台适用
      ```
      pip install -r requirements.txt
      ```
   4. 本地运行OCR.py文件
      ```
      python OCR.py
      ```
   

### 项目地址

- 
