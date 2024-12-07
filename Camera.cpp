#include "Camera.h"
#include "Constants.h"
#include "Sphere.h"
#include "Universe.h"
#include "Geometry.h"


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
    if (targetSpheres.empty()){
        for (Sphere& sphere : universe.spheres){
            targetSpheres_.push_back(&sphere);
        }
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
// 視野角をalpha、任意の天体から重心までの距離をl, 重心からその天体までの方向ベクトルと重心からカメラまでの方向ベクトルのなす角をthetaとしたとき、
// 各天体について、l*sin(theta)/tan(alpha/2)+l*cos(theta)を計算する。(図をかくと分かる)
// この値の最大値だけ重心から離れた位置にカメラを置くのが最適と思われる。
// カメラのTargetをアップデート。TargetはtargetSpheresの重心とする。
    float massPos[3] = {0.0f, 0.0f, 0.0f};
    float totalMass = 0.0f;
    // 重心を計算してtarget_に格納
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
    
    // カメラのpositionをアップデート
    // 重心からカメラへの方向ベクトルを作成
    float z_min=0.3f;
    float camera_r[3]={                      //方向ベクトル   
        static_cast<float>((std::abs(cos(omegaLatitude_*universe_.getSimulationTime() / scaling::time))+0.1f)*cos(omegaLongitude_*universe_.getSimulationTime() / scaling::time)),       //カメラ位置のx座標(z軸からの距離がゼロにならないようにしている)
        static_cast<float>((std::abs(cos(omegaLatitude_*universe_.getSimulationTime() / scaling::time))+0.1f)*sin(omegaLongitude_*universe_.getSimulationTime() / scaling::time)),       //カメラ位置のy座標(z軸からの距離がゼロにならないようにしている)
        static_cast<float>(std::abs(z_min+(1-2*z_min)*sin(omegaLatitude_*universe_.getSimulationTime() / scaling::time)*sin(omegaLatitude_*universe_.getSimulationTime() / scaling::time)))   //カメラ位置のz座標(z>0を維持するとともに、sinを二乗して折り返し時になめらかな動きにする)
        // static_cast<float>(std::abs(0.1f+0.9f*cos(omegaLatitude_*universe_.getSimulationTime() / scaling::time)*cos(omegaLatitude_*universe_.getSimulationTime() / scaling::time)))   //カメラ位置のz座標(z>0を維持するとともに、sinを二乗して折り返し時になめらかな動きにする)
    };
    // 重心からカメラまでの距離を作成
    float distance_g2camera = 0.0f;    //重心からカメラまでの距離を格納する変数
    std::vector<std::tuple<float, float, float>> pastPoints;    // 全天体の軌跡を構成するすべての点および天体の現在位置を格納する->画角に収めるべきすべての点
    for (Sphere* sphere : targetSpheres_) {
        pastPoints.push_back(std::make_tuple(sphere->x, sphere->y, sphere->z));    // 天体の現在位置の格納
        for (auto& point : sphere->trajectory) { 
            // pastPoints.push_back(std::make_tuple(std::get<0>(point), std::get<1>(point), std::get<2>(point)));  // 軌跡を構成する点の格納k
        }
    }
    for (auto& point : pastPoints){
            // 重心からその天体までの方向ベクトルと重心からカメラまでの方向ベクトルのなす角thetaを作成
            float r_fromG[3] = {std::get<0>(point)-target_[0], std::get<1>(point)-target_[1], std::get<2>(point)-target_[2]};  //重心からその天体までのベクトル
            float theta = Geometry::angleBetweenSegments(camera_r, r_fromG);
            float zeroVector[3] = {0.0f,0.0f,0.0f};
            float l = Geometry::distanceBetweenPoints(zeroVector, r_fromG);     // 重心から天体までの距離l
            float alpha = 2 * M_PI * cameraSetting::fovy / 360.0f;  // 視野角alpha(弧度法に変換)
            distance_g2camera = std::max(distance_g2camera, static_cast<float>(l*sin(theta)/tan(alpha/2)+l*cos(theta))); // これが答え
    }


    float distance_adjuster=1.0f;        //距離の調整(加算)
    distance_g2camera *=distance_adjuster;     //全天体の重心から最小質量天体までの距離に加算する
    position_[0] = target_[0]+distance_g2camera*camera_r[0];
    position_[1] = target_[1]+distance_g2camera*camera_r[1];
    position_[2] = target_[2]+distance_g2camera*camera_r[2];
    

    // カメラのUpをアップデート
    up_[0]=0.0f;
    up_[1]=0.0f;
    up_[2]=1.0f;
}