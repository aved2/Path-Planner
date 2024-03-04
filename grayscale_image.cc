#include "grayscale_image.hpp"

#include <cmath>
#include <fstream>
#include <iostream>

#include "color.hpp"

GrayscaleImage::GrayscaleImage(const ElevationDataset& dataset):
    width_(dataset.Width()), height_(dataset.Height()) {
  constexpr int kMax = 255;

  //Populate vector storing color at each pixel on map
  for (size_t i = 0; i < dataset.GetData().size(); i++) {
    std::vector<Color> row;
    for (size_t j = 0; j < dataset.GetData().at(i).size(); j++) {
      double a = (dataset.GetData().at(i).at(j) - dataset.MinEle());
      double b = (dataset.MaxEle() - dataset.MinEle());
      double c = a / b;
      int d = static_cast<int>(std::round(c * kMax));
      Color color = Color(d, d, d);
      row.push_back(color);
    }
    image_.push_back(row);
  }
}

GrayscaleImage::GrayscaleImage(const std::string& filename,
                               size_t width,
                               size_t height):
    width_(width), height_(height) {
  ElevationDataset dataset = ElevationDataset(filename, width, height);
  constexpr int kMax = 255;
  for (size_t i = 0; i < dataset.GetData().size(); i++) {
    std::vector<Color> row;
    for (size_t j = 0; j < dataset.GetData().at(i).size(); j++) {
      double a = (dataset.GetData().at(i).at(j) - dataset.MinEle());
      double b = (dataset.MaxEle() - dataset.MinEle());
      double c = a / b;
      int d = static_cast<int>(std::round(c * kMax));
      Color color = Color(d, d, d);
      row.push_back(color);
    }
    image_.push_back(row);
  }
}

size_t GrayscaleImage::Width() const { return width_; }

size_t GrayscaleImage::Height() const { return height_; }

unsigned int GrayscaleImage::MaxColorValue() const { return kMaxColorValue; }

const Color& GrayscaleImage::ColorAt(int row, int col) const {
  return image_.at(row).at(col);
}

const std::vector<std::vector<Color>>& GrayscaleImage::GetImage() const {
  return image_;
}

void GrayscaleImage::ToPpm(const std::string& name) const {
  std::string identifier = "P3";
  
  //Convert vector of colors into ppm image.
  std::ofstream ofs(name);
  ofs << identifier << std::endl;
  ofs << width_ << " " << height_ << std::endl;
  ofs << kMaxColorValue << std::endl;

  for (size_t i = 0; i < image_.size(); i++) {
    for (size_t j = 0; j < image_.at(0).size(); j++) {
      ofs << image_.at(i).at(j).Red() << " ";
      ofs << image_.at(i).at(j).Green() << " ";
      ofs << image_.at(i).at(j).Blue() << " ";
    }
    ofs << std::endl;
  }
  ofs.close();
}
