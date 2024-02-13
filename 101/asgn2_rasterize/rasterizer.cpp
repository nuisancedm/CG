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


static bool insideTriangle(float x, float y, const Vector3f* _v)
{   
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    // >>>> start to do
    Vector3f Q(x, y, 0.0f); // 测试点

    // 计算向量和它们的叉乘
    Vector3f edge1 = _v[2] - _v[1];
    Vector3f QToVertex1 = Q - _v[1];
    Vector3f cross1 = QToVertex1.cross(edge1);

    Vector3f edge2 = _v[0] - _v[2];
    Vector3f QToVertex2 = Q - _v[2];
    Vector3f cross2 = QToVertex2.cross(edge2);

    Vector3f edge3 = _v[1] - _v[0];
    Vector3f QToVertex3 = Q - _v[0];
    Vector3f cross3 = QToVertex3.cross(edge3);

    // 检查叉乘的z分量是否同号
    bool sameSide1and2 = (cross1.z() * cross2.z() >= 0);
    bool sameSide2and3 = (cross2.z() * cross3.z() >= 0);

    return sameSide1and2 && sameSide2and3;
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

    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.

    //>>>> start to do
    //>>>> get the bounding box of current triangle
    float minx = v[0].x();
    float maxx = v[0].x();
    float miny = v[0].y();
    float maxy = v[0].y();

    for(int i = 1 ; i < 3 ; i++){
        if(v[i].x() < minx){
            minx = v[i].x();
        }
        if(v[i].x() > maxx){
            maxx = v[i].x();
        }
        if(v[i].y() < miny){
            miny = v[i].y();
        }
        if(v[i].y() > maxy){
            maxy = v[i].y();
        }
    }
    //>>>>iterate through the pixel and find if the current pixel is inside the triangle
    for(int x = minx ; x < maxx ; x++){
        for(int y = miny; y < maxy;y++){
            // no MSAA
            // if(insideTriangle(x+0.5,y+0.5,t.v)){
            //     // is in side
            //     auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
            //     float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
            //     float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
            //     z_interpolated *= w_reciprocal;//z 深度插值

            //     int id = get_index(x,y);
            //     if(z_interpolated < depth_buf[id]){
            //         depth_buf[id] = -z_interpolated;
            //         frame_buf[id] = t.getColor();
            //     }
            // }
            
            // MSAA 2x
            int multisample = 2;
            std::vector<float> sampleListsZ;
            std::vector<Eigen::Vector3f> colors;
            float step = 1.0f / multisample;
            for(int j=0; j<2; j++) {
                for(int k=0; k<2;k++){
                    float offsetX = (j + 0.5f) * step;
                    float offsetY = (k + 0.5f) * step;
                    // 计算采样点的实际坐标
                    float sampleX = x + offsetX;
                    float sampleY = y + offsetY;
                    if(insideTriangle(sampleX,sampleY,t.v)){
                        auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
                        float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        z_interpolated *= w_reciprocal;//z 深度插值
                        sampleListsZ.push_back(-z_interpolated);
                        Eigen::Vector3f color = t.getColor();
                        colors.push_back(color);
                    }else{
                        sampleListsZ.push_back(0);
                        Eigen::Vector3f color(0,0,0);
                        colors.push_back(color);
                    }
                }
            }
            int id = get_index(x,y);
            float avgZ = 0;
            Eigen::Vector3f avgColor(0,0,0);
            for(int i=0;i<multisample*multisample;i++){
                avgZ += sampleListsZ[i];
                avgColor[0] = avgColor[0] + colors[i][0];
                avgColor[1] = avgColor[1] + colors[i][1];
                avgColor[2] = avgColor[2] + colors[i][2];
            }
            avgZ = avgZ / multisample*multisample;
            avgColor = avgColor / (multisample*multisample);

            if(avgZ < depth_buf[id]){
                depth_buf[id] = avgZ;
                frame_buf[id] = avgColor;
            }
        }
    }

    
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

// clang-format on