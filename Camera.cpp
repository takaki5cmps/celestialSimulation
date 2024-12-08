#include "Camera.h"
#include "Constants.h"
#include "Sphere.h"
#include "Universe.h"
#include "Geometry.h"
#include <GL/gl.h>   // OpenGLの基本機能を使うためのヘッダー


// Cameraクラス実装部分
// Camera::Camera()    // デフォルトコンストラクタ
//     :omegaLatitude_(cameraSetting::omega_z), omegaLongitude_(cameraSetting::omega)
// {
//     // for (Sphere& sphere : universe.spheres){
//     //     targetSpheres_.push_back(&sphere);
//     // }
// }
Camera::Camera(Universe& universe, std::vector<Sphere*> targetSpheres = {})    // コンストラクタ
    :universe_(universe), targetSpheres_(targetSpheres), omegaLatitude_(cameraSetting::omega_z), omegaLongitude_(cameraSetting::omega)  // 適当な初期値
{
    float initialx = 0.0f;
    if (targetSpheres.empty()){
        for (Sphere& sphere : universe.spheres){
            targetSpheres_.push_back(&sphere);
            initialx = std::max (initialx, static_cast<float>(std::sqrt(sphere.x*sphere.x+sphere.y*sphere.y+sphere.z*sphere.z)));
        }
//         position_[0]=celestialConstants::distance_sun_earth*scaling::distance*1.01f;
//         position_[1]=0.0f;
//         position_[2]=0.0f;
//         target_[0]=0.0f;
//         target_[1]=0.0f;
//         target_[2]=0.0f;
// std::cout << "position: " << position_[0] << ", " << position_[1] << ", " << position_[2] << std::endl;
// std::cout << "target: " << target_[0] << ", " << target_[1] << ", " << target_[2] << std::endl;
    }
    
}
void Camera::changeSpheres(std::vector<Sphere*> targetSpheres){
    targetSpheres_=targetSpheres;
}
void Camera::addSphere(Sphere* sphere){
    targetSpheres_.push_back(sphere);
}
float Camera::getPosition(int i) const  {
    if (i < 0 || i >= 3) {
        throw std::out_of_range("Index out of range");
    }
    return position_[i];
}
float Camera::getTarget(int i) const  {
    if (i < 0 || i >= 3) {
        throw std::out_of_range("Index out of range");
    }
    return target_[i];
}
float Camera::getUp(int i) const  {
    if (i < 0 || i >= 3) {
        throw std::out_of_range("Index out of range");
    }
    return up_[i];
}
void Camera::update(){
//カメラは、球面上を動きながら全天体を画角に収めたい。
// したがって球面の半径を求める必要がある。以下の考え方で半径を作成する。
// 視野角をalpha、任意の天体から見る対象までの距離をl, 見る対象からその天体までの方向ベクトルと見る対象からカメラまでの方向ベクトルのなす角をthetaとしたとき、
// 各天体について、l*sin(theta)/tan(alpha/2)+l*cos(theta)を計算する。(図をかくと分かる)
// この値の最大値だけ見る対象から離れた位置にカメラを置くのが最適と思われる。
// カメラのTargetをアップデート。TargetはtargetSpheresの見る対象とする。
    float massPos[3] = {0.0f, 0.0f, 0.0f};
    float totalMass = 0.0f;
    // 見る対象を計算してtarget_に格納
    for (Sphere* sphere : targetSpheres_){
        // massPos[0]+=sphere->mass*sphere->x;
        // massPos[1]+=sphere->mass*sphere->y;
        // massPos[2]+=sphere->mass*sphere->z;
        // totalMass+=sphere->mass;
        massPos[0]+=1.0*sphere->x;
        massPos[1]+=1.0*sphere->y;
        massPos[2]+=1.0*sphere->z;
        totalMass+=1.0;
    }
    target_[0]=massPos[0]/totalMass;
    target_[1]=massPos[1]/totalMass;
    target_[2]=massPos[2]/totalMass;

    // ビューポートの取得
    GLint viewport[4]; // 左下のx, 左下のy, width, height
    double aspect = viewport[2] / viewport[3];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // モデルビュー行列、投影行列、ビューポートの取得
    GLdouble modelMatrix[16];
    GLdouble projMatrix[16];
    // GLint viewport[4]; // 左下のx, 左下のy, width, height
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // 見る対象からカメラまでの距離を計算
    double maxDistance = -10000.0f; // カメラが前後すべき最大距離
    double tanHalfFovy = tan(cameraSetting::fovy * 2.0 *M_PI / 360.0f);
std::cout << "fovy: " << cameraSetting::fovy * 2.0 *M_PI / 360.0f << std::endl;
    for (Sphere* sphere : targetSpheres_) {
        double winX, winY, winZ;

        // 3D空間の天体の座標を2Dスクリーン座標に変換(対象天体の深度バッファが欲しい)
        GLdouble obj[3] = {static_cast<double>(sphere->x), static_cast<double>(sphere->y), static_cast<double>(sphere->z)};        
std::cout << "obj: " << obj[0] << " " << obj[1] << " " << obj[2] << std::endl;
        GLdouble winX_d, winY_d, winZ_d;
        if (gluProject(obj[0], obj[1], obj[2], modelMatrix, projMatrix, viewport, &winX_d, &winY_d, &winZ_d) == GL_TRUE) {
            winX = winX_d;
            winY = winY_d;
            winZ = winZ_d;
        } else {
        }
std::cout << "winX: " << winX << " winY: " << winY << " winZ: " << winZ << std::endl;
        // 対象天体の深度バッファにおいて、画面中央から画面の端までの3D空間の距離を計算
        double edgeY[3] = {viewport[2]/2.0f, static_cast<double>(viewport[3]), winZ}; // 画面の上端の座標
std::cout << "edgeY: " << edgeY[0] << " " << edgeY[1] << " " << edgeY[2] << std::endl;
        double center[3] = {viewport[2]/2.0f, viewport[3]/2.0f, 0.0f}; // 画面の中央の座標
std::cout << "center: " << center[0] << " " << center[1] << " " << center[2] << std::endl;
        // ウィンドウ座標をオブジェクト座標に変換
        double edgeY_3D[3], center_3D[3];
        if (gluUnProject(edgeY[0], edgeY[1], edgeY[2], modelMatrix, projMatrix, viewport, edgeY_3D, edgeY_3D+1, edgeY_3D+2) == GL_TRUE) {
        } else {
        }
        if (gluUnProject(center[0], center[1], center[2], modelMatrix, projMatrix, viewport, center_3D, center_3D+1, center_3D+2) == GL_TRUE) {
        } else {
        }
std::cout << "edgeY_3D: " << edgeY_3D[0] << " " << edgeY_3D[1] << " " << edgeY_3D[2] << std::endl;
std::cout << "center_3D: " << center_3D[0] << " " << center_3D[1] << " " << center_3D[2] << std::endl;
        // カメラが前後すべき距離
        maxDistance = 
            std::max(maxDistance, 
                Geometry::distanceBetweenPoints(obj, center_3D) 
                - Geometry::distanceBetweenPoints(edgeY_3D, center_3D)
            )/tanHalfFovy;
std::cout << "Geometry::distanceBetweenPoints(obj, center_3D): " << Geometry::distanceBetweenPoints(obj, center_3D) << std::endl;
std::cout << "Geometry::distanceBetweenPoints(edgeY_3D, center_3D): " << Geometry::distanceBetweenPoints(edgeY_3D, center_3D) << std::endl;
std::cout << "maxDistance: " << maxDistance << std::endl;
    }
    float zeroVector[3] = {0.0f,0.0f,0.0f};
    float distance_g2camera = maxDistance + Geometry::distanceBetweenPoints(target_, position_); //これがカメラの位置ベクトルの長さ
std::cout << "distance_g2camera: " << distance_g2camera << std::endl;

    // 見る対象からカメラへの方向ベクトルを作成
    const float z_min=0.3f; //z座標の最小値
    const float z_max=0.9f; //z座標の最大値
    const float theta_min=asin(z_min); //z_minにおけるthetaの値
    const float theta_max=asin(z_max); //z_maxにおけるthetaの値
    const float theta_amplitude=theta_max-theta_min; //thetaの振幅
    const float theta_center=(theta_max+theta_min)/2; //thetaの中心
    float theta = theta_center + theta_amplitude*sin(omegaLatitude_*universe_.getSimulationTime() / scaling::time); //thetaの値（緯度）
    float phi = omegaLongitude_*universe_.getSimulationTime() / scaling::time; //phiの値（経度）
    float camera_r[3]={                      //方向ベクトル   
        static_cast<float>(cos(theta)*cos(phi)),       //カメラ位置のx座標(z軸からの距離がゼロにならないようにしている)
        static_cast<float>(cos(theta)*sin(phi)),       //カメラ位置のy座標(z軸からの距離がゼロにならないようにしている)
        static_cast<float>(sin(theta))   //カメラ位置のz座標(z>0を維持するとともに、sinを二乗して折り返し時になめらかな動きにする)
    };
//     std::vector<std::tuple<float, float, float>> pastPoints;    // 全天体の軌跡を構成するすべての点および天体の現在位置を格納する->画角に収めるべきすべての点
//     for (Sphere* sphere : targetSpheres_) {
//         pastPoints.push_back(std::make_tuple(sphere->x, sphere->y, sphere->z));    // 天体の現在位置の格納
//         for (auto& point : sphere->trajectory) { 
//             // pastPoints.push_back(std::make_tuple(std::get<0>(point), std::get<1>(point), std::get<2>(point)));  // 軌跡を構成する点の格納k
//         }
//     }
//     for (auto& point : pastPoints){
//             // 見る対象からその天体までの方向ベクトルと見る対象からカメラまでの方向ベクトルのなす角thetaを作成
//             float r_fromG[3] = {std::get<0>(point)-target_[0], std::get<1>(point)-target_[1], std::get<2>(point)-target_[2]};  //見る対象からその天体までのベクトル
//             float theta = Geometry::angleBetweenSegments(camera_r, r_fromG);
//             float zeroVector[3] = {0.0f,0.0f,0.0f};
//             float l = Geometry::distanceBetweenPoints(zeroVector, r_fromG);     // 見る対象から天体までの距離l
//             float alpha = 2 * M_PI * cameraSetting::fovy / 360.0f;  // 視野角alpha(弧度法に変換)
//             distance_g2camera = std::max(distance_g2camera, static_cast<float>(l*sin(theta)/tan(alpha/2)+l*cos(theta))); // これが答え
//     }

    // カメラの位置を決定
    // const float distance_adjuster=-100+celestialConstants::moon_radius*scaling::distance;        //距離の調整(加算)
    // distance_g2camera +=distance_adjuster;     //全天体の見る対象から最小質量天体までの距離に加算する
    position_[0] = target_[0]+distance_g2camera*camera_r[0];
    position_[1] = target_[1]+distance_g2camera*camera_r[1];
    position_[2] = target_[2]+distance_g2camera*camera_r[2];



    // カメラのUpをアップデート
    up_[0]=0.0f;
    up_[1]=0.0f;
    up_[2]=1.0f;

}