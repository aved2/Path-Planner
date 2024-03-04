#include "path_image.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

PathImage::PathImage(const GrayscaleImage& image,
                     const ElevationDataset& dataset):
    width_(image.Width()), height_(image.Height()) {
  // Copy the topographical image from GrayscaleImage to path_image_
  CopyImage(image);
  // Calculate all possible paths and store in paths_
  unsigned int best_row = 0;
  CalculateAllPaths(dataset, best_row);
  // Set final value for best_row
  for (size_t t = 0; t < paths_.size(); t++) {
    if (paths_.at(t).EleChange() < paths_.at(best_row).EleChange()) {
      best_row = t;
    }
  }
  // Color paths
  ColorPaths(best_row);
}

void PathImage::CopyImage(const GrayscaleImage& image) {
  for (size_t i = 0; i < image.GetImage().size(); i++) {
    std::vector<Color> row;
    for (size_t j = 0; j < image.GetImage().at(0).size(); j++) {
      row.push_back(image.GetImage().at(i).at(j));
    }
    path_image_.push_back(row);
  }
}

void PathImage::CalculateAllPaths(const ElevationDataset& dataset,
                                  unsigned int& best_row) {
  int curr = 0;
  int top = 0;
  int mid = 0;
  int bot = 0;
  int change_t = 0;
  int change_m = 0;
  int change_b = 0;
  for (size_t i = 0; i < height_; i++) {
    Path p(width_, i);
    unsigned int current_row = i;
    unsigned int j = 1;
    while (j < width_) {
      curr = dataset.DatumAt(current_row, j - 1);
      if (current_row == 0) {
        // Edge Case: Current row is first row, no top value to compare
        mid = dataset.DatumAt(current_row, j);
        bot = dataset.DatumAt(current_row + 1, j);
        change_m = abs(mid - curr);
        change_b = abs(bot - curr);
        if (change_b < change_m) {
          current_row++;
          p.IncEleChange(change_b);
        } else {
          p.IncEleChange(change_m);
        }
      } else if (current_row == height_ - 1) {
        // Edge Case: Current row is last row, no bottom value to compare
        top = dataset.DatumAt(current_row - 1, j);
        mid = dataset.DatumAt(current_row, j);
        change_t = abs(top - curr);
        change_m = abs(mid - curr);
        if (change_t < change_m) {
          current_row--;
          p.IncEleChange(change_t);
        } else {
          p.IncEleChange(change_m);
        }
      } else {
        top = dataset.DatumAt(current_row - 1, j);
        mid = dataset.DatumAt(current_row, j);
        bot = dataset.DatumAt(current_row + 1, j);
        change_t = abs(top - curr);
        change_m = abs(mid - curr);
        change_b = abs(bot - curr);
        // If change_m is the least or tied with the least, it wins.
        if ((change_m <= change_t && change_m < change_b) ||
            (change_m < change_t && change_m <= change_b)) {
          p.IncEleChange(change_m);
        }
        // If change_t and change_b are tied and are the least, change_b wins.
        else if (change_t == change_b && change_t < change_m) {
          current_row++;
          p.IncEleChange(change_b);
        }
        // If change_t is strictly the least, it wins.
        else if (change_t < change_m && change_t < change_b) {
          current_row--;
          p.IncEleChange(change_t);
        }
        // If none of the above, change_b wins by default because it's the only
        // one left.
        else {
          current_row++;
          p.IncEleChange(change_b);
        }
      }
      p.SetLoc(j, current_row);
      j++;
    }
    if (dataset.DatumAt(i, 0) < dataset.DatumAt(best_row, 0)) {
      best_row = i;
    }
    paths_.push_back(p);
  }
}
void PathImage::ColorPaths(unsigned int best_row) {
  constexpr int kR1 = 252;
  constexpr int kR2 = 25;
  constexpr int kR3 = 63;
  constexpr int kG1 = 31;
  constexpr int kG2 = 253;
  constexpr int kG3 = 13;
  Color red(kR1, kR2, kR3);
  Color green(kG1, kG2, kG3);
  for (size_t z = 0; z < paths_.size(); z++) {
    Path p = paths_.at(z);
    for (size_t f = 0; f < p.Length(); f++) {
      path_image_.at(p.GetPath().at(f)).at(f) = red;
    }
  }
  for (size_t f = 0; f < paths_.at(best_row).Length(); f++) {
    path_image_.at(paths_.at(best_row).GetPath().at(f)).at(f) = green;
  }
}
size_t PathImage::Width() const { return width_; }
size_t PathImage::Height() const { return height_; }
unsigned int PathImage::MaxColorValue() const { return kMaxColorValue; }
const std::vector<Path>& PathImage::Paths() const { return paths_; }
const std::vector<std::vector<Color>>& PathImage::GetPathImage() const {
  return path_image_;
}
void PathImage::ToPpm(const std::string& name) const {
  std::string identifier = "P3";

  //Convert vector of colors into ppm image
  std::ofstream ofs(name);
  ofs << identifier << std::endl;
  ofs << width_ << " " << height_ << std::endl;
  ofs << kMaxColorValue << std::endl;

  for (size_t i = 0; i < path_image_.size(); i++) {
    for (size_t j = 0; j < path_image_.at(0).size(); j++) {
      ofs << path_image_.at(i).at(j).Red() << " ";
      ofs << path_image_.at(i).at(j).Green() << " ";
      ofs << path_image_.at(i).at(j).Blue() << " ";
    }
    ofs << std::endl;
  }
  ofs.close();
}