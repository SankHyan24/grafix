// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>

rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}
static bool sc(Vector2f a,Vector2f b)
{
    if((a[0]*b[1]-a[1]*b[0])>=0)
        return 1;
    else
        return 0;
}

static bool insideTriangle(float x,float y, const Vector3f* _v)
{   
    //  : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Eigen::Vector2f a=_v[0].head(2),b=_v[1].head(2),c=_v[2].head(2),d={x,y};
    Eigen::Vector2f ab=a-b,bc=b-c,ca=c-a,ad=a-d,bd=b-d,cd=c-d;
    bool flag=0;
    if (sc(ab,ad)==1&&sc(bc,bd)==1&&sc(ca,cd)==1)
        return 1; 
    else 
        return 0;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle
    // bounding box
    int min_x = (int)std::floor(std::min(v[0][0], std::min(v[1][0], v[2][0])));
    int max_x = (int)std::ceil(std::max(v[0][0], std::max(v[1][0], v[2][0])));
    int min_y = (int)std::floor(std::min(v[0][1], std::min(v[1][1], v[2][1])));
    int max_y = (int)std::ceil(std::max(v[0][1], std::max(v[1][1], v[2][1])));
// #define MSAA

#ifndef MSAA
    for (int i = min_x; i <=max_x ; i++)
    {
        for (int j =min_y ; j <=max_y ; j++)
        {
            if(insideTriangle((float)i+0.5,(float)j+0.5,t.v))
            {
                // 计算插值
                auto interpolation=computeBarycentric2D((float)i+0.5,(float)j+0.5,t.v);
                float alpha,beta,gamma;
                std::tie(alpha, beta, gamma) = interpolation;//插值系数
                float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                z_interpolated *= w_reciprocal;
                if (depth_buf[get_index(i, j)] > z_interpolated) {
					Vector3f color = t.getColor();
					Vector3f point={(float)i, (float)j, z_interpolated};
					depth_buf[get_index(i, j)] = z_interpolated;
					set_pixel(point, color);}
            }
        }
    }
#else
    int cnt;
    float cntmat[4][2]={0.25,0.25,0.25,0.75,0.75,0.25,0.75,0.75}, mindepth;
    for (int i = min_x; i <=max_x ; i++)
        for (int j =min_y ; j <=max_y ; j++)
        {
            cnt = 0;
            mindepth = FLT_MAX;
            bool ifin[4]={0,0,0,0};
            float tmpdepth[] = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX};
            for (int k = 0; k < 4; k++)// 对四个点进行插值z
                if(insideTriangle((float)i+cntmat[k][0],(float)j+cntmat[k][1],t.v))
                {
                    cnt++;
                    ifin[k]=true;
                    auto interpolation=computeBarycentric2D((float)i+cntmat[k][0],(float)j+cntmat[k][1],t.v);
                    float alpha,beta,gamma;
                    std::tie(alpha, beta, gamma) = interpolation;
                    float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    tmpdepth[k]=z_interpolated;
                    if (mindepth > z_interpolated)
					    mindepth = z_interpolated;
                }
            if(cnt==0)
                continue;
            if(cnt==4&&depth_buf[get_index(i, j)] > mindepth)//quan覆盖，直接渲染
            {
                Vector3f point={(float)i, (float)j, depth_buf[get_index(i, j)]};
			    Vector3f color = (float)cnt/4.0*t.getColor();
                set_pixel(point, color);
			    depth_buf[get_index(i, j)] = mindepth;//像素的zbuffer永远维护最小z值
                int edgeindex=if_edge(i,j);
                if(-1!=edgeindex)//如果是边缘点，删了
                {
                    edge_index.erase(edge_index.begin()+edgeindex);
                    aslist.erase(aslist.begin()+4*edgeindex,aslist.begin()+4*edgeindex+3);
                    aslistcolor.erase(aslistcolor.begin()+4*edgeindex,aslistcolor.begin()+4*edgeindex+3);
                }
            }
            else if(cnt==4&&-1!=if_edge(i,j))//非边缘点且不覆盖，查找是否是前者边缘点.是边缘点，所以要比较一下
                {
                    int id=if_edge(i,j),counter=0;
                    Vector3f color = {0,0,0};
                    for(int k=0;k<4;k++)
                        if(aslist[4*id+k]>mindepth){
                            aslist[4*id+k]=mindepth;
                            aslistcolor[4*id+k]=t.getColor();
                            color+=t.getColor()/4;
                        }else{color+=aslistcolor[4*id+k]/4;}
                    Vector3f point={(float)i, (float)j, depth_buf[get_index(i, j)]};
                    set_pixel(point, color);
                }//非前者边缘点，无事发生
            //是边缘点的情况
            else if(depth_buf[get_index(i, j)]>mindepth)//没被覆盖的边缘点
            {//如果已经是边缘点了，去他妈的不算了，就当全不是边缘点
                int edgeindex=if_edge(i,j);
                if(-1==edgeindex)
                {   
                    add_edge_id(i,j,ifin,tmpdepth,t.getColor()); 
                    edgeindex=if_edge(i,j);
                }
                upload_cache_byid(i,j,tmpdepth,t.getColor());
			    depth_buf[get_index(i, j)] = mindepth;
                Vector3f point={(float)i, (float)j, depth_buf[get_index(i, j)]};
			    Vector3f color = {0,0,0};
                for(int k=0;k<4;k++)
                    color+=aslistcolor[4*edgeindex+k]/4;
                set_pixel(point, color);
            }
        }
#endif
    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

bool rst::rasterizer::add_edge_id(int x,int y,bool ifin[],float z[],const Eigen::Vector3f& color)
{
    int id=get_index(x,y);
    std::vector<int>::iterator result = find( edge_index.begin( ), edge_index.end( ), id ); //查找3
    Eigen::Vector3f black={0,0,0};
    if(result != edge_index.end())
        return 0;
    else
    {
        edge_index.push_back(id);
        for(int k=0;k<4;k++)
        {   
            aslist.push_back(ifin[k]?z[k]:FLT_MAX);
            aslistcolor.push_back(ifin[k]?color:black);
        }
        return 1;
    }
}
void rst::rasterizer::upload_cache_byid(int x, int y, float z[],const Eigen::Vector3f& color)
{
    int id=get_index(x,y),posi=if_edge(x,y);
    for(int i=4*posi,j=0;j<4;j++)
        if(aslist[i+j]>z[j])
        {
            aslist[i+j]=z[j];
            aslistcolor[i+j]=color;
        }
}
int rst::rasterizer::if_edge(int x, int y)
{
    int id=get_index(x,y);
    std::vector<int>::iterator result = find( edge_index.begin( ), edge_index.end( ), id ); //查找3
    if(result == edge_index.end())
        return -1;
    return (result-edge_index.begin());
}

// clang-format on