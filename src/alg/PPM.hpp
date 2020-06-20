#ifndef PPM_HPP
#define PPM_HPP

#include "Camera.hpp"
#include "Hitable.hpp"
#include "HitableList.hpp"
#include "Parser.hpp"
#include "Ray.hpp"
#include "RenderAlg.hpp"
#include "Utils.hpp"

#define THREADS_NUM 8

struct HitPoint //光子映射需要记录的撞击点
{
    HitPoint(const Hit &rec, Vector3f attenuation_, int x_, int y_, float init_r)
        : hit_pos(rec.p), normal(rec.normal), attenuation(attenuation_), flux(Vector3f::ZERO), x(x_), y(y_),
          current_r(init_r), photon_num(0)
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
    Photon() {}
    Vector3f power; //光子的能量，对应光通量
    Vector3f pos;   //光子击中的位置
};

inline bool comparatorX(const Photon &a, const Photon &b) { return a.pos.x() < b.pos.x(); }
inline bool comparatorY(const Photon &a, const Photon &b) { return a.pos.y() < b.pos.y(); }
inline bool comparatorZ(const Photon &a, const Photon &b) { return a.pos.z() < b.pos.z(); }

struct KDNode
{
    KDNode() : lc(nullptr), rc(nullptr) {}
    void query(HitPoint &view_point, std::vector<const Photon *> &res) const;
    /*data*/
    KDNode *lc;
    KDNode *rc;
    Photon value;
    Vector3f min, max;
};

Vector3f node_max(KDNode *a, KDNode *b);
Vector3f node_min(KDNode *a, KDNode *b);

/****************************************
 * 渐进光子映射
 ****************************************/
class ProgressivePhotonMapping : public RenderAlg
{
  public:
    ProgressivePhotonMapping(const SceneInfo &info);
    ~ProgressivePhotonMapping();
    void run() override;
    void ray_tracing(const Ray &r, int x, int y, int spp, bool save = false);
    void photon_tracing(const Ray &r, Photon photon);
    void update(HitPoint &view_point);
    void build_kd_tree(KDNode *&now, std::vector<Photon> &l, int start, int end, int dim = 0);
    void save() override;

    /*data*/
    Vector3f **pic;
    int DEPTH;
    int ITER;
    int EMIT_NUM; //每轮发射光子数
    int sample_num;
    float ALPHA;
    float init_r;
    shared_ptr<HitableList> lights;

    std::vector<HitPoint> view_points;
    std::vector<Photon> photon_map;

    KDNode *root;
    KDNode *pool;
    int used_node;
};

#endif