#include "Camera.h"
#include "Constants.h"
#include "Sphere.h"
#include "Universe.h"
#include "Geometry.h"
#include <GL/gl.h>   // OpenGLの基本機能を使うためのヘッダー


// カメラや描画に関する定数
namespace cameraSetting{
    const float fovy = 60.0f;
    const float zNear = 1.0e-5f;
    const float zFar = 10000.0f;
    const float omega= 10*2.0*M_PI/(365*24*60*60);               //カメラの経度方向の移動の角速度
    const float omega_z= 0.5*omega;            //カメラの緯度方向の移動の角速度
}

// クラスCameraの実装部分
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
    // 見る対象を計算
    float massPos[3] = {0.0f, 0.0f, 0.0f};
    float totalMass = 0.0f;
    // 見る対象を計算してtarget_に格納
    for (Sphere* sphere : targetSpheres_){
        massPos[0]+=sphere->mass*sphere->x;
        massPos[1]+=sphere->mass*sphere->y;
        massPos[2]+=sphere->mass*sphere->z;
        totalMass+=sphere->mass;
        // massPos[0]+=1.0*sphere->x;
        // massPos[1]+=1.0*sphere->y;
        // massPos[2]+=1.0*sphere->z;
        // totalMass+=1.0;
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

    // カメラに収める点をコンテナに入れる。(Sphre*だけでなく、各Sphereの軌跡を構成する点も格納する)
    std::vector<std::tuple<float,float,float>> targetPoints;
    for (Sphere* sphere : targetSpheres_){
        targetPoints.push_back(std::make_tuple(sphere->x, sphere->y, sphere->z));
        for (std::tuple<float, float, float> point : sphere->trajectory){
            targetPoints.push_back(point);
        }
    }


    // 見る対象からカメラまでの距離を計算
    double maxDistance = -10000.0f; // カメラが前後すべき最大距離
    double tanHalfFovy = tan(cameraSetting::fovy * 2.0 *M_PI / 360.0f);
    for (std::tuple<float, float, float> targetPoint : targetPoints) {
        double winX, winY, winZ;
        // 3D空間の天体の座標を2Dスクリーン座標に変換(対象天体の深度バッファが欲しい)
        GLdouble obj[3] = {static_cast<double>(std::get<0>(targetPoint)), static_cast<double>(std::get<1>(targetPoint)), static_cast<double>(std::get<2>(targetPoint))};        
        GLdouble winX_d, winY_d, winZ_d;
        if (gluProject(obj[0], obj[1], obj[2], modelMatrix, projMatrix, viewport, &winX_d, &winY_d, &winZ_d) == GL_TRUE) {
            winX = winX_d;
            winY = winY_d;
            winZ = winZ_d;
        } else {
        }
        // 対象天体の深度バッファにおいて、画面中央から画面の端までの3D空間の距離を計算
        double edgeY[3] = {viewport[2]/2.0f, static_cast<double>(viewport[3]), winZ}; // 画面の上端の座標
        double center[3] = {viewport[2]/2.0f, viewport[3]/2.0f, 0.0f}; // 画面の中央の座標
        // ウィンドウ座標をオブジェクト座標に変換
        double edgeY_3D[3], center_3D[3];
        if (gluUnProject(edgeY[0], edgeY[1], edgeY[2], modelMatrix, projMatrix, viewport, edgeY_3D, edgeY_3D+1, edgeY_3D+2) == GL_TRUE) {
        } else {
        }
        if (gluUnProject(center[0], center[1], center[2], modelMatrix, projMatrix, viewport, center_3D, center_3D+1, center_3D+2) == GL_TRUE) {
        } else {
        }
        // カメラが前後すべき距離
        maxDistance = 
            std::max(
                maxDistance, 
                (Geometry::distanceBetweenPoints(obj, center_3D) 
                - Geometry::distanceBetweenPoints(edgeY_3D, center_3D)
                )/tanHalfFovy
            );
    }
    const float zeroVector[3] = {0.0f,0.0f,0.0f};
    const float buffer = 0.0f; // カメラが前後する際のバッファ
    float distance_g2camera = buffer+maxDistance + Geometry::distanceBetweenPoints(target_, position_); //これがカメラの位置ベクトルの長さ

    // 見る対象からカメラへの方向ベクトルを作成
    const float z_min=-0.2f; //z座標の最小値
    const float z_max=0.4f; //z座標の最大値
    const float theta_min=asin(z_min); //z_minにおけるthetaの値
    const float theta_max=asin(z_max); //z_maxにおけるthetaの値
    const float theta_amplitude=(theta_max-theta_min)/2; //thetaの振幅
    const float theta_center=(theta_max+theta_min)/2; //thetaの中心
    // float theta = theta_center + theta_amplitude*sin(omegaLatitude_*universe_.getSimulationTime() / scaling::time); //thetaの値（緯度）
    // float phi = omegaLongitude_*universe_.getSimulationTime() / scaling::time; //phiの値（経度）
    float theta = theta_center + theta_amplitude*sin(omegaLatitude_*universe_.getSimulationTime() / scaling::time); //thetaの値（緯度）
    float phi = omegaLongitude_*universe_.getSimulationTime() / scaling::time; //phiの値（経度）
    float camera_r[3]={                      //方向ベクトル   
        static_cast<float>(cos(theta)*cos(phi)),       //カメラ位置のx座標(z軸からの距離がゼロにならないようにしている)
        static_cast<float>(cos(theta)*sin(phi)),       //カメラ位置のy座標(z軸からの距離がゼロにならないようにしている)
        static_cast<float>(sin(theta))   //カメラ位置のz座標(z>0を維持するとともに、sinを二乗して折り返し時になめらかな動きにする)
    };

    // カメラの位置を決定
    position_[0] = target_[0]+distance_g2camera*camera_r[0];
    position_[1] = target_[1]+distance_g2camera*camera_r[1];
    position_[2] = target_[2]+distance_g2camera*camera_r[2];



    // カメラのUpをアップデート
    up_[0]=0.0f;
    up_[1]=0.0f;
    up_[2]=1.0f;

}