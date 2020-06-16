#ifndef PPM_HPP
#define PPM_HPP

#include "Hitable.hpp"
#include "Ray.hpp"
#include "Utils.hpp"

struct HitPoint //光子映射需要记录的撞击点
{
    Vector3f hit_pos;
    Vector3f normal;
    Vector3f attenuation; //累计衰减率
    int x, y;
    int photon_num;
};

struct KDNode
{
    void build_kd_tree(KDNode *&now, std::vector<HitPoint> l, int start, int end, int dim = 0);
    /*data*/
    KDNode *lc;
    KDNode *rc;
    HitPoint value;
    int split_dim;
};

/****************************************
 * 渐进光子映射
 ****************************************/
class PPM
{
  public:
    PPM(int width_, int height_, int max_depth);
    ~PPM();
    void ray_tracing(const Ray &r, const Vector3f &ray_color);
    void photon_tracing(const Ray &r, const Vector3f &ray_color, float current_r);

    /*data*/
    Vector3f **pic;
    int width, height;
    int MAX_DEPTH;
    std::vector<HitPoint> view_points;
    shared_ptr<Hitable> world;
    shared_ptr<Hitable> light;
};

#endif