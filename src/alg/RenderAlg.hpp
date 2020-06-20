#ifndef RENDER_ALG_HPP
#define RENDER_ALG_HPP

class RenderAlg
{
  public:
    RenderAlg(const SceneInfo &info)
        : height(info.height), width(static_cast<int>(info.height * info.w_h_ratio)), camera_p(info.camera),
          world(info.world), save_path(info.save_path)
    {
    }
    virtual ~RenderAlg() {}
    virtual void run() = 0;
    virtual void save() = 0;

    /*data*/
    int height;
    int width;
    shared_ptr<Camera> camera_p;
    shared_ptr<Hitable> world;
    std::string save_path;
};

#endif