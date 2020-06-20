#include "Parser.hpp"
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
#include "omp.h"

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

const char split_line[] = "****************************************\n";
Vector3f create_from_json_array(const Value &array)
{
    assert(array.IsArray());
    return Vector3f(array[0].GetFloat(), array[1].GetFloat(), array[2].GetFloat());
}

SceneInfo Parser::parse(const char *filename) //解析表示输入场景的json文件
{
    SceneInfo info;
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
    Document d;
    d.Parse(s.c_str());
    for (auto itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr)
    {
        /*保存路径*/
        if (strcmp(itr->name.GetString(), "save_path") == 0)
        {
            info.save_path = itr->value.GetString();
            std::cout << "save path = " << info.save_path << "\n";
        }
        else if (strcmp(itr->name.GetString(), "alg") == 0)
        {
            const char *alg_name = itr->value.GetString();
            if (strcmp(alg_name, "ppm") == 0)
                info.alg = PPM;
            else if (strcmp(alg_name, "pt") == 0)
                info.alg = PT;
            std::cout << "algorithm : " << alg_name << "\n";
        }
        else if (strcmp(itr->name.GetString(), "height") == 0)
        {
            info.height = itr->value.GetInt();
            std::cout << "pic height = " << info.height << "\n";
        }
        else if (strcmp(itr->name.GetString(), "sample_number") == 0)
        {
            info.sample_num = itr->value.GetInt();
            std::cout << "sample num = " << info.sample_num << "\n";
        }
        else if (strcmp(itr->name.GetString(), "emit_num") == 0)
        {
            info.EMIT_NUM = itr->value.GetInt();
            std::cout << "emit num = " << info.EMIT_NUM << "\n";
        }
        else if (strcmp(itr->name.GetString(), "alpha") == 0)
        {
            info.ALPHA = itr->value.GetFloat();
            std::cout << "alpha = " << info.ALPHA << "\n";
        }
        else if (strcmp(itr->name.GetString(), "depth") == 0)
        {
            info.DEPTH = itr->value.GetInt();
            std::cout << "depth = " << info.DEPTH << "\n";
        }
        else if (strcmp(itr->name.GetString(), "iter") == 0)
        {
            info.ITER = itr->value.GetInt();
            std::cout << "iter num = " << info.ITER << "\n";
        }
        else if (strcmp(itr->name.GetString(), "init_radius") == 0) /*相机*/
        {
            info.init_r = itr->value.GetFloat();
            std::cout << "ppm init radius = " << info.init_r << "\n";
        }
        else if (strcmp(itr->name.GetString(), "camera") == 0) /*相机*/
        {
            const Value &camera_info = itr->value;
            info.camera = parse_camera(camera_info);
            info.w_h_ratio = camera_info["w_h_ratio"].GetFloat(); //长宽比
            std::cout << split_line;
        }
        else if (strcmp(itr->name.GetString(), "lights") == 0)
        {
            std::cout << "Lights\n";
            const Value &lights_info = itr->value;
            info.lights = make_shared<HitableList>();
            for (auto imp_itr = lights_info.Begin(); imp_itr != lights_info.End(); ++imp_itr)
            {
                shared_ptr<Hitable> imp_obj = parse_hitable(*imp_itr);
                info.lights->add_hitable(imp_obj);
                std::cout << "Brightness = " << imp_obj->material_p->get_bright() << "\n";
            }
        }
        else if (strcmp(itr->name.GetString(), "importance") == 0)
        {
            std::cout << "importance shape\n";
            const Value &importance = itr->value;
            shared_ptr<HitableList> imp_list = make_shared<HitableList>();
            for (auto imp_itr = importance.Begin(); imp_itr != importance.End(); ++imp_itr)
            {
                shared_ptr<Hitable> imp_obj = parse_hitable(*imp_itr);
                imp_list->add_hitable(imp_obj);
            }
            info.sample_list = imp_list;
            std::cout << split_line;
        }
        else if (strcmp(itr->name.GetString(), "scene") == 0) /*场景*/
        {
            const Value &scene = itr->value;
            shared_ptr<Hitable> o;
            for (Value::ConstValueIterator scene_itr = scene.Begin(); scene_itr != scene.End(); ++scene_itr)
            {
                o = parse_hitable(*scene_itr);
                objects.push_back(o);
            }
        }
    }
    info.world = make_shared<BVHNode>(objects, 0, objects.size(), 0.0, 1.0);
    return info;
}

shared_ptr<Hitable> Parser::parse_sphere(const Value &json)
{
    shared_ptr<Hitable> sphere;
    shared_ptr<Material> m_p = parse_material(json["material"]);
    if (json.HasMember("moving"))
    {
        const Value &center_info_1 = json["center_1"];
        const Value &center_info_2 = json["center_2"];
        Vector3f center_1 = create_from_json_array(center_info_1);
        Vector3f center_2 = create_from_json_array(center_info_2);
        float radius = json["radius"].GetFloat();
        float t0 = json["time0"].GetFloat();
        float t1 = json["time1"].GetFloat();
        std::cout << "center_1 = " << center_1 << ", center_2 = " << center_2 << ", raduis = " << radius << "\n";
        sphere = make_shared<MovingSphere>(center_1, center_2, t0, t1, radius, m_p);
    }
    else
    {
        const Value &center_info = json["center"];
        Vector3f center = create_from_json_array(center_info);
        float radius = json["radius"].GetFloat();
        std::cout << "center = " << center << ", raduis = " << radius << "\n";
        sphere = make_shared<Sphere>(center, radius, m_p);
    }

    if (json.HasMember("wrapper"))
    {
        for (const auto &wrapper : json["wrapper"].GetArray())
            sphere = parser_wrapper(wrapper, sphere);
    }
    return sphere;
}

shared_ptr<Hitable> Parser::parse_mesh(const Value &json)
{
    const char *filename = json["path"].GetString();
    shared_ptr<Material> m_p = parse_material(json["material"]);
    float scale = 1.0;
    if (json.HasMember("scale"))
        scale = json["scale"].GetFloat();
    std::cout << "scale = " << scale << "\n";
    shared_ptr<Hitable> mesh = make_shared<Mesh>(filename, scale, m_p);

    if (json.HasMember("wrapper"))
    {
        for (const auto &wrapper : json["wrapper"].GetArray())
            mesh = parser_wrapper(wrapper, mesh);
    }
    return mesh;
}

shared_ptr<Hitable> Parser::parse_rect(const Value &json)
{
    string type(json["type"].GetString());
    shared_ptr<Hitable> rect(nullptr);
    const Value &bounding = json["boundary"];
    assert(bounding.IsArray());
    std::cout << "type = " << type << " : ";
    std::cout << bounding[0].GetFloat() << ", " << bounding[1].GetFloat() << ", " << bounding[2].GetFloat() << ", "
              << bounding[3].GetFloat() << ", " << bounding[4].GetFloat() << "\n";

    shared_ptr<Material> m_p = parse_material(json["material"]);
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
    return rect;
}

shared_ptr<Hitable> Parser::parse_box(const Value &json)
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
    return box;
}

shared_ptr<Hitable> Parser::parse_curve(const Value &json)
{
    vector<Vector2f> control_points;
    const Value &control_point_info = json["control_points"];
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
    return curve;
}

shared_ptr<Material> Parser::parse_material(const Value &json)
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
    else if (type == "null")
        return nullptr;
    else
    {
        std::cerr << "抱歉，暂时不支持" << type << "\n";
    }
    return m_p;
}

shared_ptr<Texture> Parser::parse_texture(const Value &json)
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
        float scale = json["scale"].GetFloat();
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
    return nullptr;
}

shared_ptr<Hitable> Parser::parser_wrapper(const Value &json, shared_ptr<Hitable> hitable)
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
    else if (type == "medium")
    {
        shared_ptr<Texture> texture = parse_texture(json);
        const float density = json["density"].GetFloat();
        std::cout << "density = " << density << "\n";
        return make_shared<ConstantMedium>(hitable, density, texture);
    }
    else if (type == "flip")
    {
        std::cout << "flip face\n";
        return make_shared<FlipFace>(hitable);
    }
    else
    {
        std::cerr << "抱歉，暂时不支持" << type << "\n";
    }
    return nullptr;
}

shared_ptr<Camera> Parser::parse_camera(const rapidjson::Value &camera_info)
{
    const Value &look_from_info = camera_info["look_from"];
    const Value &look_at_info = camera_info["look_at"];
    const Value &up_info = camera_info["up"];
    const float vfov = camera_info["vfov"].GetFloat();           //视角，角度制
    const float w_h_ratio = camera_info["w_h_ratio"].GetFloat(); //长宽比
    const float aperture = camera_info["aperture"].GetFloat();   //孔径，可以形成景深效果
    const float focus_d = camera_info["focus_d"].GetFloat();     //焦距，即投影平面距离
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
    shared_ptr<Camera> camera =
        make_shared<Camera>(look_from, look_at, up, vfov, w_h_ratio, aperture, focus_d, time_0, time_1);

    std::cout << "[Camera]\nlook from = " << look_from << ", look at = " << look_at << ", up = " << up << "\n";
    std::cout << "weight height ratio = " << w_h_ratio;
    std::cout << ", vfov = " << vfov << ", aperture = " << aperture << ", focus distance = " << focus_d << ", time = ["
              << time_0 << ", " << time_1 << "]\n";
    return camera;
}

shared_ptr<Hitable> Parser::parse_hitable(const rapidjson::Value &json)
{
    shared_ptr<Hitable> hitable(nullptr);
    auto itr = json.FindMember("name");
    if (itr == json.MemberEnd())
    {
        std::cerr << "场景(scene)解析错误，没有name字段\n";
    }
    if (strcmp(itr->value.GetString(), "sphere") == 0)
    {
        std::cout << "Parse Sphere\n";
        hitable = parse_sphere(json);
        std::cout << split_line;
    }
    else if (strcmp(itr->value.GetString(), "box") == 0)
    {
        std::cout << "Parse Box\n";
        hitable = parse_box(json);
        std::cout << split_line;
    }
    else if (strcmp(itr->value.GetString(), "mesh") == 0)
    {
        std::cout << "Parse Mesh\n";
        hitable = parse_mesh(json);
        std::cout << split_line;
    }
    else if (strcmp(itr->value.GetString(), "curve") == 0)
    {
        std::cout << "Parse Curve\n";
        hitable = parse_curve(json);
        std::cout << split_line;
    }
    else if (strcmp(itr->value.GetString(), "rect") == 0)
    {
        std::cout << "Parse Rect\n";
        hitable = parse_rect(json);
        std::cout << split_line;
    }
    else if (strcmp(itr->value.GetString(), "random_box") == 0)
    {
        std::cout << "Random Box\n";
        hitable = random_box();
        std::cout << split_line;
    }
    else
    {
        std::cout << "目前不支持" << itr->value.GetString() << "\n";
    }
    return hitable;
}
