#include "PPM.hpp"
#include "Material.hpp"
#include <algorithm>
#include <stack>

using std::nth_element;

PPM::PPM(const char *filename)
{
    controller.parse(filename);
    height = controller.height;
    width = static_cast<int>(height * controller.w_h_ratio);
    DEPTH = controller.DEPTH;
    ITER = controller.ITER;
    EMIT_NUM = controller.EMIT_NUM;
    ALPHA = controller.ALPHA;
    pic = new Vector3f *[height];
    for (int i = 0; i < height; ++i)
        pic[i] = new Vector3f[width];
}

PPM::~PPM()
{
    for (int i = 0; i < height; ++i)
        delete[] pic[i];
    delete[] pic;
}

void PPM::run()
{
    /*1.先做光线追踪*/
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            float u = float(x + get_frand()) / float(width);
            float v = float(y + get_frand()) / float(height);
            Ray r_init = controller.camera.get_ray(u, v);
            ray_tracing(r_init, x, y);
        }
    /*2.光子发射*/
    for (int iter = 0; iter < ITER; ++iter)
    {
        float total_bright = 0.0;
        //每一轮分为两步，第一步发射，构建kd树后，第二步更新
        for (auto &light : controller.lights->list)
        {
            total_bright += light->material_p->get_bright();
        }
        for (auto &light : controller.lights->list)
        {
            int light_emit = static_cast<int>(EMIT_NUM * light->material_p->get_bright() / total_bright);
            for (int i = 0; i < light_emit; ++i)
            {
                Vector3f random_point = light->random(Vector3f::ZERO);
                Vector3f random_dir = random_cosine_direction();
                random_dir = Vector3f(random_dir.y(), random_dir.z(), random_dir.x());
                Ray r_init(random_point, random_dir, 0.0);
                Photon photon;
                photon.power = light->material_p->emitted(Hit(), 0, 0, random_point);
                photon.pos = random_point;
            }
        }
        build_kd_tree(root, photon_map, 0, photon_map.size(), get_irand(0, 2));
        //更新
        for (HitPoint &view_point : view_points)
            update(view_point);
        release_kd_tree(root);
        root = nullptr;
    }
    const float total_photon_num = static_cast<float>(ITER) * EMIT_NUM;
    /*向图像中赋值*/
    for (const HitPoint &view_point : view_points)
    {
        Vector3f color = view_point.flux / (M_PI * view_point.current_r * view_point.current_r * total_photon_num);
        pic[view_point.y][view_point.x] += color;
    }
}

void PPM::ray_tracing(const Ray &r_init, int x, int y)
{
    Vector3f color(1.0, 1.0, 1.0);
    Ray r = r_init;
    for (int i = 0; i < DEPTH; ++i)
    {
        Hit rec_world, rec_light;
        bool light_hit, world_hit;
        light_hit = controller.lights->hit(r, EPS, FLT_MAX, rec_light);
        world_hit = controller.world->hit(r, EPS, FLT_MAX, rec_world);
        if (!light_hit && !world_hit)
            break;
        else if ((light_hit && !world_hit) ||
                 (light_hit && world_hit && rec_light.t < rec_world.t)) //只命中灯或者先命中灯
        {
            pic[y][x] += rec.material_p->emitted(rec_light, rec_light.u, rec_light.v, rec_light.p);
            break;
        }
        else
        {
            ScatterRecord srec;
            if (!rec.material_p->scatter(r, rec_world, srec))
                break;
            if (!srec.is_specular)
            {
                view_points.emplace_back(rec_world, color, x, y);
            }
            else
            {
                r = srec.specular_ray;
                color = color * srec.attenuation;
            }
        }
    }
}

void PPM::photon_tracing(const Ray &r_init, Photon photon)
{
    Ray r = r_init;
    for (int i = 0; i < DEPTH; ++i)
    {
        Hit rec;
        if (controller.world->hit(r, EPS, FLT_MAX, rec))
        {
            ScatterRecord srec;
            if (!rec.material_p->scatter(r, rec, srec))
                break;
            if (srec.is_specular)
            {
                r = srec.specular_ray;
                photon.power = photon.power * srec.attenuation;
            }
            else
            {
                photon.pos = rec.p;           //保存击中点的信息
                photon_map.push_back(photon); //加入到photon_map
                r = Ray(rec.p, srec.pdf_ptr->generate(), r.time());
                photon.power = photon.power * srec.attenuation;
            }
        }
        else
            break;
    }
}

void PPM::update(HitPoint &view_point)
{
    std::vector<Photon *> res;
    root->query(view_point, res);
    for (const auto &p : res)
        view_point.flux += p->power;
    float update_rate = (static_cast<float>(view_point.photon_num) + ALPHA * res.size()) /
                        (static_cast<float>(view_point.photon_num) + res.size());
    view_point.photon_num += static_cast<int>(ALPHA * res.size());
    view_point.current_r *= sqrt(update_rate);
    view_point.flux *= update_rate;
}

void PPM::build_kd_tree(KDNode *&now, std::vector<Photon> l, int start, int end, int dim)
{
    if (start >= end)
        return;
    int mid = (start + end) >> 2;
    switch (dim)
    {
    case 0:
        nth_element(l.begin() + start, l.begin() + mid, l.begin() + end, comparatorX);
        break;
    case 1:
        nth_element(l.begin() + start, l.begin() + mid, l.begin() + end, comparatorY);
        break;
    case 1:
        nth_element(l.begin() + start, l.begin() + mid, l.begin() + end, comparatorZ);
        break;
    default:
        break;
    }
    now = new KDNode;
    now->value = l[mid];
    now->lc = now->rc = nullptr;
    now->max = now->value.pos;
    now->min = now->value.pos;
    build_kd_tree(now->lc, l, start, mid, get_irand(0, 2));
    if (node->lc)
    {
        now->max = node_max(now, now->lc);
        now->min = node_min(now, now->lc);
    }
    build_kd_tree(now->rc, l, mid, end, get_irand(0, 2));
    if (node->rc)
    {
        now->max = node_max(now, now->rc);
        now->min = node_min(now, now->rc);
    }
}

void PPM::release_kd_tree(KDNode *now)
{
    if (!now)
        return;
    release_kd_tree(now->lc);
    release_kd_tree(now->rc);
    delete now;
}

Vector3f KDNode::node_min(KDNode *a, KDNode *b)
{
    return Vector3f(ffmin(a->min.x(), b->min.x()), ffmin(a->min.y(), b->min.y()), ffmin(a->min.z(), b->min.z()));
}

Vector3f KDNode::node_max(KDNode *a, KDNode *b)
{
    return Vector3f(ffmax(a->max.x(), b->max.x()), ffmax(a->max.y(), b->max.y()), ffmax(a->max.z(), b->max.z()));
}

void KDNode::query(HitPoint &view_point, std::vector<Photon *> res) const
{
    //查找和view_point距离小于r的
    float dx, dy, dz;
    const Vector3f &point = view_point.hit_pos;
    if (point.x() < max.x() && point.x() > min.x())
        dx = 0.f;
    else
        dx = ffmin(fabs(point.x() - min.x()), fabs(point.x() - max.x()));
    if (point.y() < max.y() && point.y() > min.y())
        dy = 0.f;
    else
        dy = ffmin(fabs(point.y() - min.y()), fabs(point.y() - max.y()));
    if (point.z() < max.z() && point.z() > min.z())
        dz = 0.f;
    else
        dz = ffmin(fabs(point.z() - min.z()), fabs(point.z() - max.z()));

    float radius2 = view_point.current_r * view_point.current_r;
    if (dx * dx + dy * dy + dz * dz > radius2)
        return;
    if ((value.pos - point).squaredLength() < radius2)
        res.push_back(&value);
    /*递归*/
    if (lc)
        lc->query(view_point, res);
    if (rc)
        rc->query(view_point, res);
}