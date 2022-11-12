#include "simpleremap360.h"

#include <algorithm>
#include <cmath>
#include <opencv2/imgproc.hpp>

void SimpleRemap360::Init(const cv::Size &srcSize, const cv::Size &dstSize,
                          float sr1, float sr2, bool opt_remap) {
  const float R = std::min(srcSize.width, srcSize.height) / 2;
  const float r1 = R * sr1;
  const float r2 = R * sr2;
  const float cx = srcSize.width * 0.5;
  const float cy = srcSize.height * 0.5;

  map_x_.create(dstSize, CV_32FC1);
  map_y_.create(dstSize, CV_32FC1);

  const float out_height = dstSize.height;
  const float out_width = dstSize.width;

  for (int y = 0; y < dstSize.height; y++) {
    for (int x = 0; x < dstSize.width; x++) {
      float r = (y / out_height) * (r2 - r1) + r1;
      float theta = (x / out_width) * 2.0 * M_PI;

      map_x_.at<float>(y, x) = cx + r * cos(theta);
      map_y_.at<float>(y, x) = cy + r * sin(theta);
    }
  }
  if (opt_remap) {
    cv::convertMaps(map_x_, map_y_, map_x_, map_y_, CV_16SC2);
  }
}

void SimpleRemap360::Remap(const cv::Mat &fish_eye_img, cv::Mat &out_image) {
  cv::remap(fish_eye_img, out_image, map_x_, map_y_, cv::INTER_LINEAR);
}
