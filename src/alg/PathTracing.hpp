#ifndef PATH_TRACING_HPP
#define PATH_TRACING_HPP

#include "Parser.hpp"
#include "RenderAlg.hpp"
#include "vecmath.h"

class PathTracing : public RenderAlg
{
  public:
    PathTracing(const SceneInfo &info);
    ~PathTracing();
    void run() override;
    void save() override;

    /*data*/
    shared_ptr<Hitable> sample_list; //直接采样对象
    int MAX_DEPTH;
    int sample_num;
    Vector3f **pic;

  protected:
    /******************************
     光线追踪的关键函数
        input:
            r——待求交的光线
            world——物体，是一个Hitable
            fake_light——表明光源的位置、大小，不参与真正的绘制
            depth——递归深度
     ******************************/
    Vector3f path_tracing(const Ray &r, int depth);
};
#endif