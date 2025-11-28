#!/usr/bin/env python3
"""快速生成资源节点占位符纹理"""
from PIL import Image, ImageDraw

def create_placeholder(path, size, color, border_color=(255, 255, 255)):
    """创建占位符纹理"""
    img = Image.new('RGBA', (size, size), color)
    draw = ImageDraw.Draw(img)
    
    # 绘制边框
    draw.rectangle([(2, 2), (size-3, size-3)], outline=border_color, width=2)
    
    # 保存
    img.save(path)
    print(f"创建: {path}")

if __name__ == "__main__":
    # 树纹理（森林绿）
    create_placeholder("../build/assets/textures/items/tree.png", 48, (34, 139, 34, 255))
    
    # 矿石纹理（灰色）
    create_placeholder("../build/assets/textures/items/ore.png", 40, (128, 128, 128, 255))
    
    print("资源节点纹理生成完成！")
