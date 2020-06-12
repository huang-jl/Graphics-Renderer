#include "Controller.hpp"
#include "BVH.hpp"
#include "Curve.hpp"
#include "HitableList.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Media.hpp"
#include "Mesh.hpp"
#include "Rect.hpp"
#include "Sphere.hpp"
#include "Texture.hpp"
#include "Transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <iostream>
#include <vector>
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;
using std::vector;
using namespace rapidjson;

Vector3f create_from_json_array(const Value &array)
{
    assert(array.IsArray());
    return Vector3f(array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat());
}

void Controller::set_camera_param(const Vector3f &look_from, const Vector3f &look_at, const Vector3f up, float vfov,
                                  float aperture, float focus_d, float time_0, float time_1)
{
    camera = Camera(look_from, look_at, up, vfov, w_h_ratio, aperture, focus_d, time_0, time_1);
}

void Controller::set_imgae_param(int height_, float w_h_ratio_, int sample_num_)
{
    height = height_;
    w_h_ratio = w_h_ratio_;
    sample_num = sample_num_;
}

void Controller::set_scene(shared_ptr<Hitable> world_) { world = world_; }

void Controller::generate_pic() const
{
    int width = w_h_ratio * height;
    std::cout << "Width = " << width << ", Height = " << height << "\n";
    ofstream output(save_path, std::ios::out | std::ios::trunc);
    output << "P3\n" << width << " " << height << "\n255\n";
    //因为光线是根据(x/width, y/height)生成，从左下为原点
    //但是ppm格式是从左上角开始，因此y需要倒序
    const int total = width * height;
    for (int y = height - 1; y >= 0; --y)
        for (int x = 0; x < width; ++x)
        {
            int processed = (height - 1 - y) * width + x;
            std::cout << "start : " << height - 1 - y << "/" << height << "\t(" << 100 * processed / total << "%)"
                      << "\r";
            Vector3f col(0, 0, 0);
            //抗锯齿采样
            for (int s = 0; s < sample_num; ++s)
            {
                //这个地方很重要，将(u,v)映射为(0,1)范围，这样nx和ny
                //就只是图像的一个大小比例，而不会影响图像的显示范围
                float u = float(x + get_rand()) / float(width);
                float v = float(y + get_rand()) / float(height);
                Ray r = camera.get_ray(u, v);
                col += color(r, world, 0);
            }
            col = col / float(sample_num);
            col = Vector3f(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //使用gamma2校验
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            // if (ir > 255 || ig > 255 || ib > 255)
            // std::cerr << "Value more than 255\n";
            output << imin(255, ir) << " " << imin(255, ig) << " " << imin(255, ib) << "\n";
            // std::cout << ir << " " << ig << " " << ib << "\n";
        }
    output.close();
}

bool Controller::parse(const char *filename) //解析表示输入场景的json文件
{
    /*1.读入json文件到string中*/
    ifstream ifs(filename, ios::in | ios::binary | ios::ate);
    if (!ifs)
        std::cerr << "Cannot open file" << filename << "\n";
    auto length = ifs.tellg();
    ifs.seekg(0, ios::beg);
    vector<char> bytes(length);
    ifs.read(bytes.data(), length);
    string s(bytes.data(), length);

    /*2.解析json文件*/
    const char split_line[] = "****************************************\n";
    Document d;
    d.Parse(s.c_str());
    for (auto itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr)
    {
        /*保存路径*/
        if (strcmp(itr->name.GetString(), "save_path") == 0)
        {
            save_path = itr->value.GetString();
            std::cout << "save path = " << save_path << "\n";
        }
        else if (strcmp(itr->name.GetString(), "height") == 0)
        {
            height = itr->value.GetInt();
            std::cout << "pic height = " << height << "\n";
        }
        else if (strcmp(itr->name.GetString(), "sample_number") == 0)
        {
            sample_num = itr->value.GetInt();
            std::cout << "sample num = " << sample_num << "\n";
        }
        else if (strcmp(itr->name.GetString(), "camera") == 0) /*相机*/
        {
            const Value &camera_info = itr->value;
            parse_camera(camera_info);
            std::cout << split_line;
        }
        else if (strcmp(itr->name.GetString(), "scene") == 0) /*场景*/
        {
            const Value &scene = itr->value;
            for (Value::ConstValueIterator itr = scene.Begin(); itr != scene.End(); ++itr)
            {
                auto scene_itr = itr->FindMember("name");
                if (scene_itr == itr->MemberEnd())
                {
                    std::cerr << "场景(scene)解析错误，没有name字段\n";
                    return false;
                }
                if (strcmp(scene_itr->value.GetString(), "sphere") == 0)
                {
                    std::cout << "Parse Sphere\n";
                    parse_sphere(*itr);
                    std::cout << split_line;
                }
                else if (strcmp(scene_itr->value.GetString(), "box") == 0)
                {
                    std::cout << "Parse Box\n";
                    parse_box(*itr);
                    std::cout << split_line;
                }
                else if (strcmp(scene_itr->value.GetString(), "mesh") == 0)
                {
                    std::cout << "Parse Mesh\n";
                    parse_mesh(*itr);
                    std::cout << split_line;
                }
                else if (strcmp(scene_itr->value.GetString(), "curve") == 0)
                {
                    std::cout << "Parse Curve\n";
                    parse_curve(*itr);
                    std::cout << split_line;
                }
                else if (strcmp(scene_itr->value.GetString(), "rect") == 0)
                {
                    std::cout << "Parse Rect\n";
                    parse_rect(*itr);
                    std::cout << split_line;
                }
            }
        }
    }
    world = make_shared<BVHNode>(objects, 0, objects.size(), 0.0, 1.0);
    return true;
}

bool Controller::parse_sphere(const Value &json)
{
    const Value &center_info = json["center"];
    Vector3f center = create_from_json_array(center_info);
    float radius = json["radius"].GetFloat();
    std::cout << "center = " << center << ", raduis = " << radius << "\n";

    shared_ptr<Material> m_p = parse_material(json["material"]);
    shared_ptr<Hitable> sphere = make_shared<Sphere>(center, radius, m_p);

    if (json.HasMember("wrapper"))
    {
        for (const auto &wrapper : json["wrapper"].GetArray())
            sphere = parser_wrapper(wrapper, sphere);
    }
    objects.push_back(sphere);
    return true;
}

bool Controller::parse_mesh(const Value &json)
{
    const char *filename = json["path"].GetString();
    shared_ptr<Material> m_p = parse_material(json["material"]);
    shared_ptr<Hitable> mesh = make_shared<Mesh>(filename, m_p);
    if (json.HasMember("wrapper"))
    {
        for (const auto &wrapper : json["wrapper"].GetArray())
            mesh = parser_wrapper(wrapper, mesh);
    }
    objects.push_back(mesh);
    return true;
}

bool Controller::parse_rect(const Value &json)
{
    string type(json["type"].GetString());
    shared_ptr<Hitable> rect(nullptr);
    const Value &bounding = json["boundary"];
    std::cout << "type = " << type << " : ";
    std::cout << bounding[0].GetFloat() << ", " << bounding[1].GetFloat() << ", " << bounding[2].GetFloat() << ", "
              << bounding[3].GetFloat() << ", " << bounding[4].GetFloat() << "\n";

    shared_ptr<Material> m_p = parse_material(json["material"]);
    assert(bounding.IsArray());
    if (type == "xy")
    {
        rect = make_shared<XYRect>(bounding[0].GetFloat(), bounding[1].GetFloat(), bounding[2].GetFloat(),
                                   bounding[3].GetFloat(), bounding[4].GetFloat(), m_p);
    }
    else if (type == "yz")
    {
        rect = make_shared<YZRect>(bounding[0].GetFloat(), bounding[1].GetFloat(), bounding[2].GetFloat(),
                                   bounding[3].GetFloat(), bounding[4].GetFloat(), m_p);
    }
    else if (type == "xz")
    {
        rect = make_shared<XZRect>(bounding[0].GetFloat(), bounding[1].GetFloat(), bounding[2].GetFloat(),
                                   bounding[3].GetFloat(), bounding[4].GetFloat(), m_p);
    }
    if (json.HasMember("wrapper"))
    {
        for (const auto &wrapper : json["wrapper"].GetArray())
            rect = parser_wrapper(wrapper, rect);
    }
    objects.push_back(rect);
    return true;
}

bool Controller::parse_box(const Value &json)
{
    const Value &left_bottom_info = json["left_bottom"];
    const Value &right_top_info = json["right_top"];
    Vector3f left_bottom = create_from_json_array(left_bottom_info);
    Vector3f right_top = create_from_json_array(right_top_info);
    std::cout << "left bottom = " << left_bottom << ", right top = " << right_top << "\n";

    shared_ptr<Material> m_p = parse_material(json["material"]);

    shared_ptr<Hitable> box = make_shared<Box>(left_bottom, right_top, m_p);
    if (json.HasMember("wrapper"))
    {
        for (const auto &wrapper : json["wrapper"].GetArray())
            box = parser_wrapper(wrapper, box);
    }
    objects.push_back(box);
    return true;
}

bool Controller::parse_curve(const Value &json)
{
    vector<Vector2f> control_points;
    const Value &control_point_info = json["control_point"];
    for (const auto &point : control_point_info.GetArray())
    {
        control_points.emplace_back(point[0].GetFloat(), point[1].GetFloat());
    }

    shared_ptr<Material> m_p = parse_material(json["material"]);
    shared_ptr<Hitable> curve = make_shared<BezierSurface>(control_points, m_p);

    if (json.HasMember("wrapper"))
    {
        for (const auto &wrapper : json["wrapper"].GetArray())
            curve = parser_wrapper(wrapper, curve);
    }
    objects.push_back(curve);
    return true;
}

shared_ptr<Material> Controller::parse_material(const Value &json)
{
    string type(json["type"].GetString());
    shared_ptr<Material> m_p(nullptr);
    std::cout << "Material = " << type << "\n";
    if (type == "lambertian")
    {
        shared_ptr<Texture> texture = parse_texture(json);
        m_p = make_shared<Lambertian>(texture);
    }
    else if (type == "metal")
    {
        const Value &albedo_info = json["albedo"];
        Vector3f albedo = create_from_json_array(albedo_info);
        const float fuzz = json["fuzz"].GetFloat();
        m_p = make_shared<Metal>(albedo, fuzz);
        std::cout << "albedo = " << albedo << ", fuzz = " << fuzz << "\n";
    }
    else if (type == "dielectric")
    {
        const float ref_idx = json["refraction_index"].GetFloat();
        m_p = make_shared<Dielectric>(ref_idx);
        std::cout << "refraction index = " << ref_idx << "\n";
    }
    else if (type == "diffuse_light")
    {
        Vector3f light_color = create_from_json_array(json["color"]);
        m_p = make_shared<DiffuseLight>(light_color);
        std::cout << "Light color = " << light_color << "\n";
    }
    else
    {
        std::cerr << "抱歉，暂时不支持" << type << "\n";
    }
    return m_p;
}

shared_ptr<Texture> Controller::parse_texture(const Value &json)
{
    string texture(json["texture"].GetString());
    std::cout << "Texture = " << texture << "\n";
    /*1.const texture*/
    if (texture == "constant")
    {
        const Value &albedo_info = json["albedo"];
        Vector3f albedo = create_from_json_array(albedo_info);
        std::cout << "albedo = " << albedo << "\n";
        return make_shared<ConstantTexture>(albedo);
    }
    else if (texture == "checker") /*2.CheckerTexture，棋盘状*/
    {
        const Value &checker_albedo_info = json["albedo"];
        Vector3f albedo_1 = create_from_json_array(checker_albedo_info[0]);
        Vector3f albedo_2 = create_from_json_array(checker_albedo_info[1]);
        shared_ptr<Texture> texture_1 = make_shared<ConstantTexture>(albedo_1);
        shared_ptr<Texture> texture_2 = make_shared<ConstantTexture>(albedo_2);
        std::cout << "albedo 1 = " << albedo_1 << ", albedo 2 = " << albedo_2 << "\n";
        return make_shared<CheckerTexture>(texture_1, texture_2);
    }
    else if (texture == "perlin")
    {
        const float scale = json["scale"].GetFloat();
        std::cout << "scale = " << scale << "\n";
        return make_shared<NoiseTexture>(scale);
    }
    else if (texture == "image")
    {
        int width, height, nn;
        unsigned char *texture_data = stbi_load(json["path"].GetString(), &width, &height, &nn, 0);
        std::cout << "[image] " << json["path"].GetString() << " width = " << width << " height = " << height
                  << " channel = " << nn << "\n";
        return make_shared<ImageTexture>(texture_data, width, height);
    }
    else
    {
        std::cerr << "抱歉，暂时不支持" << texture << "\n";
    }
}

shared_ptr<Hitable> Controller::parser_wrapper(const Value &json, shared_ptr<Hitable> hitable)
{
    string type = json["type"].GetString();
    std::cout << "Wrapper = " << type << "\n";
    if (type == "translate") //平移
    {
        const Value &displace_info = json["displacement"];
        Vector3f displace = create_from_json_array(displace_info);
        std::cout << "displacement = " << displace << "\n";
        return make_shared<Translate>(hitable, displace);
    }
    else if (type == "rotate")
    {
        const int axis = json["axis"].GetInt(); // 0——绕x轴；1——绕y轴；z——绕z轴
        const float degree = json["degree"].GetFloat();
        std::cout << "degree = " << degree << ", axis = " << axis << "\n";
        return make_shared<Rotate>(hitable, degree, axis);
    }
    else if (type == "media")
    {
        shared_ptr<Texture> texture = parse_texture(json);
        const float density = json["density"].GetFloat();
        std::cout << "density = " << density << "\n";
        return make_shared<ConstantMedium>(hitable, density, texture);
    }
    else
    {
        std::cerr << "抱歉，暂时不支持" << type << "\n";
    }
}

bool Controller::parse_camera(const rapidjson::Value &camera_info)
{
    const Value &look_from_info = camera_info["look_from"];
    const Value &look_at_info = camera_info["look_at"];
    const Value &up_info = camera_info["up"];
    const float vfov = camera_info["vfov"].GetFloat();         //视角，角度制
    w_h_ratio = camera_info["w_h_ratio"].GetFloat();           //长宽比
    const float aperture = camera_info["aperture"].GetFloat(); //孔径，可以形成景深效果
    const float focus_d = camera_info["focus_d"].GetFloat();   //焦距，即投影平面距离
    float time_0 = 0., time_1 = 1.;
    auto time_itr = camera_info.FindMember("time");
    if (time_itr != camera_info.MemberEnd())
    {
        time_0 = time_itr->value[0].GetFloat();
        time_1 = time_itr->value[1].GetFloat();
    }
    Vector3f look_from = create_from_json_array(look_from_info);
    Vector3f look_at = create_from_json_array(look_at_info);
    Vector3f up = create_from_json_array(up_info);
    camera = Camera(look_from, look_at, up, vfov, w_h_ratio, aperture, focus_d, time_0, time_1);
    std::cout << "[Camera]\nlook from = " << look_from << ", look at = " << look_at << ", up = " << up << "\n";
    std::cout << "weight height ratio = " << w_h_ratio;
    std::cout << ", vfov = " << vfov << ", aperture = " << aperture << ", focus distance = " << focus_d << ", time = ["
              << time_0 << ", " << time_1 << "]\n";
}