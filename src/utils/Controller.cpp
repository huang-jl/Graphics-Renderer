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
    double begin_time = omp_get_wtime();
    int width = w_h_ratio * height;
    std::cout << "Width = " << width << ", Height = " << height << "\n";
    // allocate buffer
    Vector3f **pic_buffer = new Vector3f *[height];
    for (int i = 0; i < height; ++i)
        pic_buffer[i] = new Vector3f[width];
    //因为光线是根据(x/width, y/height)生成，从左下为原点
    //但是ppm格式是从左上角开始，因此y需要倒序
    int processed = 0;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1)
    for (int y = height - 1; y >= 0; --y)
    {
#pragma omp atomic
        processed++;
#pragma omp critical(stdout)
        std::cout << "(" << 100 * processed / height << "%)\r" << std::flush;
        for (int x = 0; x < width; ++x)
        {
            Vector3f col(0, 0, 0);
            Vector3f temp;
            //抗锯齿采样
            for (int s = 0; s < sample_num; ++s)
            {
                //这个地方很重要，将(u,v)映射为(0,1)范围，这样nx和ny
                //就只是图像的一个大小比例，而不会影响图像的显示范围
                float u = float(x + get_frand()) / float(width - 1);
                float v = float(y + get_frand()) / float(height - 1);
                Ray r = camera.get_ray(u, v);
                temp = color(r, world, sample_list, 0);
#ifdef DEBUG
                if (temp[0] != temp[0] || temp[1] != temp[1] || temp[2] != temp[2])
                {
                    std::cout << temp << "\n";
                    std::cerr << "Meet NaN\n";
                }
#endif
                col += temp;
            }
            col = col / float(sample_num);
            col = Vector3f(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //使用gamma2校验
            pic_buffer[y][x] = col;
        }
    }

    ofstream output(save_path, std::ios::out | std::ios::trunc);
    output << "P3\n" << width << " " << height << "\n255\n";
    for (int y = height - 1; y >= 0; --y)
        for (int x = 0; x < width; ++x)
        {
            const Vector3f &col = pic_buffer[y][x];
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            // if (ir > 255 || ig > 255 || ib > 255)
            // std::cerr << "Value more than 255\n";
            output << imin(255, ir) << " " << imin(255, ig) << " " << imin(255, ib) << "\n";
            // std::cout << ir << " " << ig << " " << ib << "\n";
        }
    output.close();
    for (int i = 0; i < height; ++i)
        delete[] pic_buffer[i];
    delete[] pic_buffer;
    double elapsed_time = omp_get_wtime() - begin_time;
    std::cout << "\nTotal time = " << elapsed_time << " s\t avg = " << elapsed_time / static_cast<double>(sample_num)
              << " s\n";
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
        else if (strcmp(itr->name.GetString(), "emit_num") == 0)
        {
            EMIT_NUM = itr->value.GetInt();
            std::cout << "emit num = " << EMIT_NUM << "\n";
        }
        else if (strcmp(itr->name.GetString(), "alpha") == 0)
        {
            ALPHA = itr->value.GetFloat();
            std::cout << "alpha = " << ALPHA << "\n";
        }
        else if (strcmp(itr->name.GetString(), "ppm_depth") == 0)
        {
            DEPTH = itr->value.GetInt();
            std::cout << "ppm depth = " << DEPTH << "\n";
        }
        else if (strcmp(itr->name.GetString(), "iter") == 0)
        {
            ITER = itr->value.GetInt();
            std::cout << "iter num = " << ITER << "\n";
        }
        else if (strcmp(itr->name.GetString(), "camera") == 0) /*相机*/
        {
            const Value &camera_info = itr->value;
            parse_camera(camera_info);
            std::cout << split_line;
        }
        else if (strcmp(itr->name.GetString(), "init_radius") == 0) /*相机*/
        {
            init_r = itr->value.GetFloat();
            std::cout << "ppm init radius = " << init_r << "\n";
        }
        else if (strcmp(itr->name.GetString(), "lights") == 0)
        {
            std::cout << "Lights\n";
            const Value &lights_info = itr->value;
            lights = make_shared<HitableList>();
            for (auto imp_itr = lights_info.Begin(); imp_itr != lights_info.End(); ++imp_itr)
            {
                shared_ptr<Hitable> imp_obj = parse_hitable(*imp_itr);
                lights->add_hitable(imp_obj);
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
            sample_list = imp_list;
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
    world = make_shared<BVHNode>(objects, 0, objects.size(), 0.0, 1.0);
    return true;
}

shared_ptr<Hitable> Controller::parse_sphere(const Value &json)
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

shared_ptr<Hitable> Controller::parse_mesh(const Value &json)
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

shared_ptr<Hitable> Controller::parse_rect(const Value &json)
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

shared_ptr<Hitable> Controller::parse_box(const Value &json)
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

shared_ptr<Hitable> Controller::parse_curve(const Value &json)
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
    else if (type == "null")
        return nullptr;
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

void Controller::parse_camera(const rapidjson::Value &camera_info)
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

shared_ptr<Hitable> Controller::parse_hitable(const rapidjson::Value &json)
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
