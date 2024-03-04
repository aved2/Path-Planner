#include "elevation_dataset.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

ElevationDataset::ElevationDataset(const std::string& filename,
                                   size_t width,
                                   size_t height):
    width_(width), height_(height) {
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    std::cout << "file not open" << std::endl;
  }
  int val = 0;

  //Read elevation data points into vector
  for (size_t i = 0; i < height_; i++) {
    std::vector<int> row;
    for (size_t j = 0; j < width_; j++) {
      ifs >> val;
      if (ifs.fail()) {
        throw std::runtime_error("Too little data");
      } else {
        row.push_back(val);
      }
    }
    data_.push_back(row);
  }

  //Validate that actual size matches declared heigth, width
  ifs >> val;
  if (!ifs.eof()) {
    throw std::runtime_error("Too much data");
  }

  if (data_.size() * data_.at(0).size() > height_ * width_) {
    if (ifs.fail()) {
      throw std::runtime_error("Too much data");
    }
  }

  min_ele_ = 0;
  max_ele_ = 0;
  MinAndMax(min_ele_, max_ele_);
}

void ElevationDataset::MinAndMax(int& min_ele_, int& max_ele_) {
  max_ele_ = data_.at(0).at(0);
  min_ele_ = data_.at(0).at(0);
  for (size_t i = 0; i < data_.size(); i++) {
    for (size_t j = 0; j < data_.at(i).size(); j++) {
      if (data_.at(i).at(j) > max_ele_) {
        max_ele_ = data_.at(i).at(j);
      }
      if (data_.at(i).at(j) < min_ele_) {
        min_ele_ = data_.at(i).at(j);
      }
    }
  }
}

size_t ElevationDataset::Width() const { return width_; }

size_t ElevationDataset::Height() const { return height_; }

int ElevationDataset::MaxEle() const { return max_ele_; }

int ElevationDataset::MinEle() const { return min_ele_; }

int ElevationDataset::DatumAt(size_t row, size_t col) const {
  return data_.at(row).at(col);
}

const std::vector<std::vector<int>>& ElevationDataset::GetData() const {
  return data_;
}