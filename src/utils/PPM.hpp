#ifndef PPM_HPP
#define PPM_HPP

#include "Camera.hpp"
#include "Controller.hpp"
#include "Hitable.hpp"
#include "HitableList.hpp"
#include "Ray.hpp"
#include "Utils.hpp"

struct HitPoint //光子映射需要记录的撞击点
{
    HitPoint(const Hit &rec, Vector3f attenuation_, int x_, int y_, float init_r)
        : hit_pos(rec.p), normal(rec.normal), attenuation(attenuation_), x(x_), y(y_), current_r(init_r), photon_num(0)
    {
    }

    /*data*/
    Vector3f hit_pos;
    Vector3f normal;
    Vector3f attenuation; //累计衰减率
    Vector3f flux;        //累计的光通量
    int x, y;
    float current_r;
    int photon_num; //累计光子
};

struct Photon
{
    Vector3f power; //光子的能量，对应光通量
    Vector3f pos;   //光子击中的位置
};

struct KDNode
{
    void query(HitPoint &view_point, std::vector<Photon *> &res) const;

    bool comparatorX(const Photon &a, const Photon &b) const { return a.pos.x() < b.pos.x(); }
    bool comparatorY(const Photon &a, const Photon &b) const { return a.pos.y() < b.pos.y(); }
    bool comparatorZ(const Photon &a, const Photon &b) const { return a.pos.z() < b.pos.z(); }
    Vector3f node_max(KDNode *a, KDNode *b);
    Vector3f node_min(KDNode *a, KDNode *b);
    /*data*/
    KDNode *lc;
    KDNode *rc;
    Photon value;
    Vector3f min, max;
};

/****************************************
 * 渐进光子映射
 ****************************************/
class PPM
{
  public:
    PPM(const char *filename, int max_depth, int iter, int emit_num, float ALPHA_);
    ~PPM();
    void run();
    void ray_tracing(const Ray &r);
    void photon_tracing(const Ray &r, Photon photon);
    void update(HitPoint &view_point);
    void build_kd_tree(KDNode *&now, std::vector<Photon> l, int start, int end, int dim = 0);
    void release_kd_tree(KDNode *now);

    /*data*/
    Controller controller;
    Vector3f **pic;
    int width;
    int height;
    int DEPTH;
    int ITER;
    int EMIT_NUM; //每轮发射光子数
    float ALPHA;
    std::vector<HitPoint> view_points;
    std::vector<Photon> photon_map;

    KDNode *root;
};

#endif