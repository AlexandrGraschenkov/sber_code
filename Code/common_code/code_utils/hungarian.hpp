//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#ifndef DETECTOR_HUNGARIAN_HPP
#define DETECTOR_HUNGARIAN_HPP

#include <opencv2/core.hpp>


// взято из https://github.com/mcximing/sort-cpp/blob/master/sort-c%2B%2B/Hungarian.cpp
// Делает сопоставление разных детекций, так чтобы cost был минимальный
// Не надо больше писать кучу "умных" условий по сопоставлению
namespace matching {

class HungarianAlgorithm {
 public:
  HungarianAlgorithm();
  ~HungarianAlgorithm();
  float solve(const cv::Mat1f &distMatrix, std::vector<int> &assignment);

 private:
  void assignmentOptimal(int *assignment, float *cost, float *distMatrix, int nOfRows, int nOfColumns);
  void buildAssignmentVector(int *assignment, bool *starMatrix, int nOfRows, int nOfColumns);
  void computeAssignmentCost(int *assignment, float *cost, float *distMatrix, int nOfRows);
  void step2a(int *assignment,
              float *distMatrix,
              bool *starMatrix,
              bool *newStarMatrix,
              bool *primeMatrix,
              bool *coveredColumns,
              bool *coveredRows,
              int nOfRows,
              int nOfColumns,
              int minDim);
  void step2b(int *assignment,
              float *distMatrix,
              bool *starMatrix,
              bool *newStarMatrix,
              bool *primeMatrix,
              bool *coveredColumns,
              bool *coveredRows,
              int nOfRows,
              int nOfColumns,
              int minDim);
  void step3(int *assignment,
             float *distMatrix,
             bool *starMatrix,
             bool *newStarMatrix,
             bool *primeMatrix,
             bool *coveredColumns,
             bool *coveredRows,
             int nOfRows,
             int nOfColumns,
             int minDim);
  void step4(int *assignment,
             float *distMatrix,
             bool *starMatrix,
             bool *newStarMatrix,
             bool *primeMatrix,
             bool *coveredColumns,
             bool *coveredRows,
             int nOfRows,
             int nOfColumns,
             int minDim,
             int row,
             int col);
  void step5(int *assignment,
             float *distMatrix,
             bool *starMatrix,
             bool *newStarMatrix,
             bool *primeMatrix,
             bool *coveredColumns,
             bool *coveredRows,
             int nOfRows,
             int nOfColumns,
             int minDim);
};
} // namespace matching

#endif //DETECTOR_HUNGARIAN_HPP
