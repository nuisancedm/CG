#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    // >>>>view_Matrix is a matrix to move the camera to (0,0,0)
    // >>>>this function doesn't rotate the camera to gaze at -z.
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
        0, 1, 0, -eye_pos[1],
        0, 0, 1, -eye_pos[2],
        0, 0, 0, 1;
    view = translate * view;
    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    // >>>>model_matrix is a matrix to rotate the object by input degrees.
    // >>>>this function doesn't move the object. only rotate.
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    // START TO DO
    float theta = rotation_angle / 180.0 * MY_PI; // >>>>Degree to Radian
    Eigen::Matrix4f rotation;
    rotation << cos(theta), -sin(theta), 0, 0,
        sin(theta), cos(theta), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;
    model = rotation * model;
    // END TO DO

    return model;
}

Eigen::Matrix4f get_rotation_matrix(Eigen::Vector3f axis, float angle)
{   
    //rotate around any axis by Rodrigus fomula.
    Eigen::Matrix3f I = Eigen::Matrix3f::Identity();

    axis = axis.normalized();
    float alpha = angle / 180 * MY_PI;
    float cosa = cos(alpha);
    float sina = sin(alpha);

    Eigen::Matrix3f RodTemp;
    RodTemp << 0, -axis.z(), axis.y(),
        axis.z(), 0, -axis.x(),
        -axis.y(), axis.x(), 0;

    Eigen::Matrix3f RodRotation = cosa * I + (1 - cosa) * (axis * axis.transpose()) + sina * RodTemp;

    Eigen::Matrix4f RodRotation_homo = Eigen::Matrix4f::Identity();
    RodRotation_homo.block<3,3>(0,0) = RodRotation;

    return RodRotation_homo;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // >>>> project_matrix is a matrix to prject the objects to a plane by perspective projection.

    // Students will implement this function
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    // START TO DO:
    // need to calculate l,r,t,b
    // something may need: tan(fovY/2)=t/n t=n*tan(fovY/2)
    //                     aspect_ratio=r/t
    float fov_theta = eye_fov / 180.0 * MY_PI;
    float t = zNear * tan(fov_theta / 2);
    float r = t * aspect_ratio;
    float b = -t;
    float l = -r;
    float n = -zNear; // should be nagetive here
    float f = -zFar;

    Eigen::Matrix4f ProjToOtho;
    ProjToOtho << n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, n + f, -n * f,
        0, 0, 1, 0;

    Eigen::Matrix4f Otho1;
    Otho1 << 2 / (r - l), 0, 0, 0,
        0, 2 / (t - b), 0, 0,
        0, 0, 2 / (n - f), 0,
        0, 0, 0, 1;

    Eigen::Matrix4f Otho2;
    Otho2 << 1, 0, 0, -(r + l) / 2,
        0, 1, 0, -(t + b) / 2,
        0, 0, 1, -(n + f) / 2,
        0, 0, 0, 1;

    Eigen::Matrix4f Otho;
    Otho = Otho1 * Otho2;

    projection = Otho * ProjToOtho;
    // END TO DO:

    return projection;
}

int main(int argc, const char **argv)
{
    float angle = 0;
    Eigen::Vector3f axis(0.5f, 0.0f, -2.0f);
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3)
    {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4)
        {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5}; // >>>> cemera position

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}}; // >>>> we have 3 coords in the space.

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}}; // >>>>render index

    auto pos_id = r.load_positions(pos);  // >>>> get the position ID to draw
    auto ind_id = r.load_indices(ind); // >>>> index id

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        
        //switch this line to rodrigus rotation for any axis.
        r.set_model(get_rotation_matrix(axis, angle));
        // r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a')
        {
            angle += 3;
        }
        else if (key == 'd')
        {
            angle -= 3;
        }
    }

    return 0;
}
