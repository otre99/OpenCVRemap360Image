#ifndef SIMPLEREMAP360_H
#define SIMPLEREMAP360_H
#include <opencv2/core/mat.hpp>

class SimpleRemap360 {
 public:
  SimpleRemap360() = default;
  void Init(const cv::Size &srcSize, const cv::Size &dstSize, float sr1,
            float sr2, bool opt_remap = false);
  void Remap(const cv::Mat &fish_eye_img, cv::Mat &out_image);

 private:
  cv::Mat map_x_;
  cv::Mat map_y_;
};

#endif  // SIMPLEREMAP360_H
