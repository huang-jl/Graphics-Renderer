#include "Mesh.hpp"
#include "Triangle.hpp"
#include <exception>
#include <fstream>
using std::ifstream;

Mesh::Mesh(const char *filename, float scale, shared_ptr<Material> m_p) : Hitable(m_p)
{
    std::vector<shared_ptr<Hitable>> triangles;
    ifstream fin(filename, std::ios::in);
    std::string s;
    std::string line;
    int index[3];
    if (!fin)
    {
        std::cerr << "打开文件：" << filename << " 失败\n";
        char error[50];
        sprintf(error, "%s%s%s", "打开文件", filename, "失败");
        throw std::invalid_argument(error);
    }
    while (std::getline(fin, line))
    {
        int current = 0, next = -1;
        current = next + 1;
        next = line.find_first_of(' ', next + 1);
        s = line.substr(current, next - current);
        if (s == "v")
        {
            Vector3f v;
            for (int i = 0; i < 3; ++i) //三角面片三个顶点
            {
                current = next + 1;
                next = line.find_first_of(' ', next + 1);
                s = line.substr(current, next - current);
                sscanf(s.c_str(), "%f", &v[i]);
                v[i] *= scale;
            }
            vertex.push_back(v);
        }
        else if (s == "f")
        {
            for (int i = 0; i < 3; ++i) //三角面片三个顶点
            {
                current = next + 1;
                next = line.find_first_of(' ', next + 1);
                s = line.substr(current, next - current);
                sscanf(s.c_str(), "%d", &index[i]);
            }
            triangles.push_back(
                make_shared<Triangle>(vertex[index[0] - 1], vertex[index[1] - 1], vertex[index[2] - 1], material_p));
        }
    }
    mesh = make_shared<BVHNode>(triangles, 0, triangles.size(), 0.0, 1.0);
}

bool Mesh::hit(const Ray &r, float t_min, float t_max, Hit &rec) const { return mesh->hit(r, t_min, t_max, rec); }

bool Mesh::bounding_box(float t0, float t1, AABB &box) const { return mesh->bounding_box(0.0, 1.0, box); }