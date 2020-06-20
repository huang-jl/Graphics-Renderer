#include "PathTracing.hpp"
#include "Hitable.hpp"
#include "Material.hpp"
#include "PDF.hpp"
#include <fstream>
#include <omp.h>

using std::ofstream;

PathTracing::PathTracing(const SceneInfo &info)
    : RenderAlg(info), sample_list(info.sample_list), MAX_DEPTH(info.DEPTH), sample_num(info.sample_num)
{
    pic = new Vector3f *[height];
    for (int i = 0; i < height; ++i)
        pic[i] = new Vector3f[width];
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            pic[y][x] = Vector3f::ZERO;
}

PathTracing::~PathTracing()
{
    for (int i = 0; i < height; ++i)
        delete[] pic[i];
    delete[] pic;
}

void PathTracing::run()
{
    double begin_time = omp_get_wtime();

    std::cout << "Width = " << width << ", Height = " << height << "\n";
    // allocate buffer
    //因为光线是根据(x/width, y/height)生成，从左下为原点
    //但是ppm格式是从左上角开始，因此y需要倒序
    int processed = 0;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1)
    for (int y = 0; y < height; ++y)
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
                Ray r = camera_p->get_ray(u, v);
                temp = path_tracing(r, 0);
#ifdef DEBUG
                if (temp[0] != temp[0] || temp[1] != temp[1] || temp[2] != temp[2])
                {
                    std::cout << temp << "\n";
                    std::cerr << "Meet NaN\n";
                }
#endif
                col += temp;
            }
            col = col / static_cast<float>(sample_num);
            col = Vector3f(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //使用gamma2校验
            pic[y][x] = col;
        }
    }

    double elapsed_time = omp_get_wtime() - begin_time;
    std::cout << "\nTotal time = " << elapsed_time << " s\t avg = " << elapsed_time / static_cast<double>(sample_num)
              << " s\n";
}

void PathTracing::save()
{
    ofstream output(save_path, std::ios::out | std::ios::trunc);
    output << "P3\n" << width << " " << height << "\n255\n";
    for (int y = height - 1; y >= 0; --y)
        for (int x = 0; x < width; ++x)
        {
            const Vector3f &col = pic[y][x];
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            output << imin(255, ir) << " " << imin(255, ig) << " " << imin(255, ib) << "\n";
        }
    output.close();
    std::cout << "[Finish] Save to " << save_path << "\n";
}

/******************************
 光线追踪的关键函数
    input:
        r——待求交的光线
        depth——递归深度
 ******************************/
Vector3f PathTracing::path_tracing(const Ray &r, int depth)
{
    Hit rec;
    //当交点处的t<0.001的时候，认为不相交，从而能绘出阴影的效果
    if (world->hit(r, EPS, FLT_MAX, rec))
    {
        //根据法向量构建颜色，最后的图像颜色和法向量有关(x,y,z)<=>(r,g,b)
        //采用递归的方式
        //有散射的物体本身没有颜色，它的颜色通过其之后的光线射中的位置确定
        //这里的attenuation是反射的系数，它会吸收相应的光线能量
        //最终的颜色为A*s(direction)*color(direction)/p(direction)
        ScatterRecord scattered;
        Vector3f emitted = rec.material_p->emitted(rec, rec.u, rec.v, rec.p);
        if (depth < MAX_DEPTH && rec.material_p->scatter(r, rec, scattered))
        {
            if (scattered.is_specular) //如果是镜面反射光线，不能直接对光源采样
            {
                return scattered.attenuation * path_tracing(scattered.specular_ray, depth + 1);
            }
            else
            {
                // (optional By dreamHuang)TODO:可以a时发送多个反射光线，一根去光源，一根反射
                shared_ptr<PDF> hit_pdf_ptr = make_shared<HitablePDF>(sample_list, rec.p);
                MixturePDF mix_pdf(hit_pdf_ptr, scattered.pdf_ptr);
                Ray reflect_r(rec.p, mix_pdf.generate(), r.time());
                float pdf_val = mix_pdf.value(reflect_r.direction());
                return emitted + scattered.attenuation * rec.material_p->scattering_pdf(r, rec, reflect_r) *
                                     path_tracing(reflect_r, depth + 1) / pdf_val;
            }
        }
        else
            return emitted;
    }
    else
        return Vector3f(0, 0, 0);
    // else
    // return Vector3f(0.70, 0.80, 1.00);
    // return Vector3f(0.15, 0.22, 0.68); //蓝色背景
}
