# 图形学大作业

黄嘉良 计86 2017010191

*本次图形学大作业主要实现了两种算法——光线追踪和渐进光子映射*

[项目地址](https://gitee.com/huang-jl/Path-Tracing-Render)



## 实现亮点

### 主要特点

- 光线追踪——src/alg/PathTracing.hpp、src/alg/PathTracing.cpp
- 重要性（蒙特卡洛）采样+对光源直接采样——src/utils/PDF.hpp, src/scene/Material.hpp中Lambertian类中的`scattering_pdf()`
- 渐进光子映射(Progressive Photon Mapping)——src/alg/PPM.hpp、src/alg/PPM.cpp
- Bezier曲线解析法求交——src/geometric/Curve.hpp、src/geometric/Curve.cpp
- 层次包围盒+kd树求交加速——src/scene/AABB.hpp、src/scene/AABB.cpp、src/scene/BVH.hpp、src/scene/BVH.cpp
- 景深——src/core/Camera.hpp、src/core/Camera.hpp，主要是通过相机发射光线的时候，加上一个孔径(aperture)的随机项实现
- UV纹理贴图——src/scene/Texture.hpp中`ImageTexture`类的实现
- Perlin噪声模拟大理石纹理——src/scene/Perlin.cpp, src/scene/Perlin.hpp, src/scene/Texture.hpp中`NoiseTexture`类的实现
- 使用参与介质材质+电解质材质模拟出次表面散射效果——src/scene/Material.hpp中`Dielectric`类和`Isotropic`类的实现
- 动态模糊效果——src/geometric/sphere.hpp、src/geometric/sphere.cpp中`MovingSphere`类的实现，同时对光线增加一个成员表示时间
- 参与介质(participating media)——src/geometric/wrapper/Media.cpp src/geometric/wrapper/Media.hpp  一种类似烟雾的效果

---

### 辅助特点

支持json文件作为场景输入，使用Rapidjson作为json解析工具——src/utils/Parser.hpp、src/utils/Parser.cpp

(使用OpenMP加速渲染)



## 最终成果图

> 原始图片请参考
>
> 光线追踪——参考文件夹 光线追踪结果
>
> 渐进光子映射——参考文件夹 渐进光子映射结果
>
> *<font color='red'>所有图片原始均为.ppm格式，文件夹中有png格式为用其他工具转换后的结果</font>*

### 光子映射效果
<center>
    <img style="border-radius: 0.3125em;
    box-shadow: 0 2px 4px 0 rgba(34,36,38,.12),0 2px 10px 0 rgba(34,36,38,.08);" 
    src="./PT/final_pic/png/final_ppm_40.png" width="80%">
    <br>
    <div style="color:orange; border-bottom: 1px solid #d9d9d9;
    display: inline-block;
    color: #999;
    padding: 2px;">渐进光子映射，发射光子30w，迭代共1500轮</div>
</center>

### 光线追踪结果（对比）
<center>
    <img style="border-radius: 0.3125em;
    box-shadow: 0 2px 4px 0 rgba(34,36,38,.12),0 2px 10px 0 rgba(34,36,38,.08);" 
    src="./PT/final_pic/png/pt_cornell.png" width="80%">
    <br>
    <div style="color:orange; border-bottom: 1px solid #d9d9d9;
    display: inline-block;
    color: #999;
    padding: 2px;">光线追踪，spp=4000</div>
</center>
---
### 参与介质
下面是一张带有参与介质(participating media)的效果图，能够发现周围场景中**包含一种雾气的感觉**，具体是使用了一个半径很大的球体包住了整个场景，这个球体的材质为参与介质
<center>
    <img style="border-radius: 0.3125em;
    box-shadow: 0 2px 4px 0 rgba(34,36,38,.12),0 2px 10px 0 rgba(34,36,38,.08);" 
    src="./PT/final_pic/png/output_1080p.png" width="80%">
    <br>
    <div style="color:orange; border-bottom: 1px solid #d9d9d9;
    display: inline-block;
    color: #999;
    padding: 2px;">光线追踪，spp=5000</div>
</center>

### 不含参与介质（对比）
相对应的不包括参与介质的效果图如下
<center>
    <img style="border-radius: 0.3125em;
    box-shadow: 0 2px 4px 0 rgba(34,36,38,.12),0 2px 10px 0 rgba(34,36,38,.08);" 
    src="./PT/final_pic/png/output_origin_1080p.png" width="80%">
    <br>
    <div style="color:orange; border-bottom: 1px solid #d9d9d9;
    display: inline-block;
    color: #999;
    padding: 2px;">光线追踪，spp=5000</div>
</center>

### 景深
最后是景深效果展示如下图
<center>
    <img style="border-radius: 0.3125em;
    box-shadow: 0 2px 4px 0 rgba(34,36,38,.12),0 2px 10px 0 rgba(34,36,38,.08);" 
    src="./PT/final_pic/png/output_origin_1080p.png" width="80%">
    <br>
    <div style="color:orange; border-bottom: 1px solid #d9d9d9;
    display: inline-block;
    color: #999;
    padding: 2px;">光线追踪，spp=5000</div>
</center>

## 编译与运行
```
cd Graphics-Renderer
mkdir -p build
cmake ..
make
../bin/PT ../testcases/simple_cornell_box.json
```

## 两种算法

### 渐进光子映射

渐进光子映射整体分为两个步骤

1. 首先做一次光线追踪，如果能够直接照射到光源，则直接对最终的像素颜色有贡献。如果撞到了漫反射表面，那么停止光线追踪，把视线碰撞点储存起来；如果是反射（折射）表面，则继续追踪。
2. 之后是做任意次的迭代
   - 首先从各个光源发射光子，本质也是进行路径追踪，当撞到漫反射表面时记录下光子碰撞点，仍然继续传播；撞到反射（折射）表面仍然继续传播。
   - 之后根据**光子**的光子图构成一棵kd树进行查询
   - 遍历所有的第一步光线追踪中的碰撞点，寻找在半径范围内的光子，这些光子对视线碰撞点有所贡献。根据碰撞点周围的光子及其数量对碰撞点的成员变量更新
3. 迭代次数达到要求后汇总视线碰撞点的信息即可

能够发现本质是两次追踪——第一次是相机发出视线，第二次是光源发出光子。第一次光线追踪能够找到所有的反射、折射直接到光源的信息，会形成反射、折射的效果（例如玻璃或者金属表面的反射效果等）；之后的光子的追踪主要是针对漫反射效果。

渐进光子映射的优点是迭代次数较小的时候，也能有**没有噪声**的图片形成，同时内存占用也比较低，因为光子的一次迭代后之前的kd树就可以抛弃了，两次迭代之间没有强依赖。对于比较复杂的场景PPM算法速度比较慢。



### 光线追踪

光线追踪的思想比较简单，但是十分有效，即使是复杂的场景也能够快速渲染出效果。相比PPM简洁而仍有不错的效果，本次大作业的最终复杂场景也是交给光线追踪来渲染。

场景中有一系列非反射、折射面，在实现过程中，采用理想Lambertian模型来表达漫反射。正是因为漫反射的随机性，导致图片通常有很多的噪声，从而需要大量的采样数量来使像素颜色最终收敛到真实值。因此我们考虑使用重要性采样+直接对光源采样结合的方法，增加收敛的速度。最终的图片spp仅为5000，让然能够渲染出十分清晰的图片



## 实现参考

1. 《Ray Tracing in One Weekend》系列
2. Hachisuka T, Ogaki S, Jensen H W. Progressive photon mapping[M]//ACM SIGGRAPH Asia 2008 papers. 2008: 1-8.