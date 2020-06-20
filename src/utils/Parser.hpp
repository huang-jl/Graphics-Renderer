#ifndef PARSER_HPP
#define PARSER_HPP
#include "Camera.hpp"
#include "HitableList.hpp"
#include "rapidjson/document.h"

/****************************************
 * 注意：
 * 1.做旋转平移变换时，json文件推荐把旋转放前面
 ****************************************/
class Material;
class Texture;

enum RenderType
{
    PT,
    PPM
};
/****************************************
 * 传递给Render的结构体，全是可以直接赋值的变量
 ****************************************/
struct SceneInfo
{
    shared_ptr<Camera> camera;
    int height;
    float w_h_ratio;
    int sample_num;
    shared_ptr<Hitable> world;
    shared_ptr<Hitable> sample_list; //直接重点采样的物体
    shared_ptr<HitableList> lights;

    /*relate to output*/
    std::string save_path;

    /*和渐进光子映射相关*/
    int ITER, DEPTH, EMIT_NUM;
    float ALPHA, init_r;
    RenderType alg; //采用光线追踪还是PPM
};

Vector3f create_from_json_array(const rapidjson::Value &array);
class Parser
{
  public:
    Parser() {}
    SceneInfo parse(const char *); //解析表示输入场景的json文件

    /*data*/
    std::vector<shared_ptr<Hitable>> objects;

  protected:
    shared_ptr<Hitable> parse_hitable(const rapidjson::Value &);
    shared_ptr<Hitable> parse_sphere(const rapidjson::Value &);
    shared_ptr<Hitable> parse_mesh(const rapidjson::Value &);
    shared_ptr<Hitable> parse_curve(const rapidjson::Value &);
    shared_ptr<Hitable> parse_rect(const rapidjson::Value &);
    shared_ptr<Hitable> parse_box(const rapidjson::Value &);
    shared_ptr<Camera> parse_camera(const rapidjson::Value &);
    shared_ptr<Hitable> parse_light_shape(const rapidjson::Value &);
    shared_ptr<Material> parse_material(const rapidjson::Value &);
    shared_ptr<Texture> parse_texture(const rapidjson::Value &);
    shared_ptr<Hitable> parser_wrapper(const rapidjson::Value &, shared_ptr<Hitable>);
};
#endif