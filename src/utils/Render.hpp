#ifndef RENDER_HPP
#define RENDER_HPP
#include "PPM.hpp"
#include "Parser.hpp"
#include "PathTracing.hpp"
#include "RenderAlg.hpp"

class Render
{
  public:
    Render(const SceneInfo &info_) : info(info_)
    {
        if (info.alg == PT)
        {
            std::cout << "Path Tracing!\n";
            alg_p = make_shared<PathTracing>(info);
        }
        else if (info.alg == PPM)
        {
            std::cout << "Progressive Photon Mapping!\n";
            alg_p = make_shared<ProgressivePhotonMapping>(info);
        }
    }
    void render()
    {
        alg_p->run();
        alg_p->save();
    }

    /*data*/
    SceneInfo info;
    shared_ptr<RenderAlg> alg_p;
};
#endif