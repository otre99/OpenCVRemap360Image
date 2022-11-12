#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "simpleremap360.h"

using namespace cv;
using namespace std;

const string params =
    "{help h usage ? |        | print this message }"
    "{video          | <none> | Input video        }"
    "{image          | <none> | Input Image        }"
    "{out_file       | <none> | Output Image/Video }"
    "{r1             |  1.0   | Inner radius       }"
    "{r2             |  0.1   | Outer radius       }"
    "{out_w          |  1920  | Out Width          }"
    "{out_h          |  700   | Out Height         }"
    "{opt_remap      |  false | Optimize remap matrices}";

bool CheckParameters(const cv::CommandLineParser &parser) {
  bool is_ok = true;
  if (!parser.check()) {
    parser.printErrors();
    return false;
  }
  if (parser.has("help")) {
    parser.printMessage();
    return false;
  }
  if (parser.has("video") && parser.has("image")) {
    std::cerr << "You must specify only one of these two arguments: "
                 "'video' or 'image'"
              << std::endl;
    is_ok = false;
  }
  if (!parser.has("video") && !parser.has("image")) {
    std::cerr << "You must specify: 'video' or 'image' argument " << std::endl;
    is_ok = false;
  }
  return is_ok;
}

int main(int argc, const char **argv) {
  cv::CommandLineParser parser(argc, argv, params);
  if (!CheckParameters(parser)) {
    parser.printErrors();
    return -1;
  }

  const string input_video_path = parser.get<string>("video");
  const string input_image_path = parser.get<string>("image");
  const float r1 = parser.get<float>("r1");
  const float r2 = parser.get<float>("r2");
  const int w = parser.get<int>("out_w");
  const int h = parser.get<int>("out_h");
  const bool opt_remap = parser.get<bool>("opt_remap");
  const string output_file = parser.get<string>("out_file");

  Mat frame, dst;
  SimpleRemap360 dewaper;
  cv::TickMeter tictac;

  if (!input_video_path.empty()) {
    cv::VideoCapture video_cap(input_video_path);
    if (!video_cap.isOpened()) {
      cerr << " Error opening input video: " << input_video_path << endl;
      return -1;
    }

    if (!video_cap.read(frame)) {
      cerr << " Error reading input video " << input_video_path << endl;
      return -1;
    }

    dewaper.Init(frame.size(), {w, h}, r1, r2, opt_remap);
    size_t nframes = 0;

    cv::VideoWriter ovideo;
    if (!output_file.empty()) {
      int video_fps = video_cap.get(cv::CAP_PROP_FPS);
      bool ok =
          ovideo.open(output_file, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                      video_fps, {w, h});
      if (!ok) {
        cerr << "Failing to open the output video " << output_file << endl;
      }
    }
    while (video_cap.read(frame)) {
      tictac.start();
      dewaper.Remap(frame, dst);
      tictac.stop();

      if (nframes++ % 100 == 0) {
        cout << "Remap FPS " << tictac.getFPS() << endl;
      }

      if (ovideo.isOpened()) {
        ovideo.write(dst);
      } else {
        cv::imshow("Original", frame);
        cv::imshow("Remaped", dst);
        if (cv::waitKey(1) == 27) {
          break;
        }
      }
    }
    cout << "Remap FPS " << tictac.getFPS() << endl;
    if (ovideo.isOpened()) ovideo.release();
  } else {
    frame = cv::imread(input_image_path);
    dewaper.Init(frame.size(), {w, h}, r1, r2, opt_remap);
    tictac.start();
    dewaper.Remap(frame, dst);
    tictac.stop();

    if (output_file.empty()) {
      cv::imshow("Original", frame);
      cv::imshow("Remaped", dst);
      cv::waitKey(-1);
    } else {
      cv::imwrite(output_file, dst);
    }
    cout << "Remap FPS " << tictac.getFPS() << endl;
  }
  return 0;
}
