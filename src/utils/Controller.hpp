#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include "Camera.hpp"
#include "rapidjson/document.h"

/****************************************
 * 注意：
 * 1.做旋转平移变换时，json文件推荐把旋转放前面
****************************************/
class Material;
class Texture;

Vector3f create_from_json_array(const rapidjson::Value&array);
class Controller
{
  public:
    Controller() : save_path("output.ppm") {}
    void set_camera_param(const Vector3f &look_from, const Vector3f &look_at, const Vector3f up, float vfov,
                          float aperture, float focus_d, float time_0, float time_1);
    void set_imgae_param(int height_, float w_h_rati_o, int sample_num_);
    void set_scene(shared_ptr<Hitable> world_);
    void generate_pic() const;
    bool parse(const char *); //解析表示输入场景的json文件
    /*data*/
    Camera camera;
    int height;
    float w_h_ratio;
    int sample_num;
    shared_ptr<Hitable> world;
    std::vector<shared_ptr<Hitable>> objects;

    /*relate to output*/
    std::string save_path;

  protected:
    bool parse_sphere(const rapidjson::Value&);
    bool parse_mesh(const rapidjson::Value&);
    bool parse_curve(const rapidjson::Value&);
    bool parse_rect(const rapidjson::Value&);
    bool parse_box(const rapidjson::Value&);
    bool parse_camera(const rapidjson::Value&);
    shared_ptr<Material> parse_material(const rapidjson::Value&);
    shared_ptr<Texture> parse_texture(const rapidjson::Value&);
    shared_ptr<Hitable> parser_wrapper(const rapidjson::Value&, shared_ptr<Hitable>);
};
#endif