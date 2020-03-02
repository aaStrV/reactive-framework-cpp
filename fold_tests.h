#ifndef FOLD_TESTS_H_
#define FOLD_TESTS_H_
#include <vector>
#include "reactive-framework-cpp.h"

void foldFloatTest(void) {
  using namespace reacf;

  ___Stream<float> s;
  float average = 0;

  std::function<void(const float&, float&)> averageF = [](const float &a,
                                                     float &acc) -> void {
    acc = acc * 0.9 + a * 0.1;
  };

  auto noiseF = [](float level) -> float {
    float resolution = 1000;
    return static_cast<float>(rand() % 1000) / (resolution / level) - level / 2;
  };

  fold(s, averageF, average);
  for (int i = 0; i < 100; ++i) {
    s.push(10 + noiseF(2));
  }
  ASSERT_NEAR(10, average, 1.01);
}

TEST(OldFoldTest, floafToFloat) {
  {
    SCOPED_TRACE("fold to float");
    for (int i = 0; i < 2; ++i) {
      foldFloatTest();
    }
  }
}

void foldVectorTest(void) {
  using namespace reacf;
  const int kMaxLen = 10;
  ___Stream<float> s;
  std::vector<float> v;

  std::function<void(const float&, std::vector<float>&)> foldF = [](
      const float &a, std::vector<float> &v) -> void {
    if (v.size() < kMaxLen) {
      v.push_back(a);
    } else {
      v.erase(v.begin());
      v.push_back(a);
    }
  };

  std::function<float(std::vector<float>&)> averageF = [](std::vector<float> &v) {
    float average = 0;
    for (auto a : v) {
      average += a;
    }
    return v.size() == 0 ? average : average / v.size();
  };

  fold(s, foldF, v);

  s.push(1);
  s.push(10);
  ASSERT_FLOAT_EQ(5.5, averageF(v));

  for (int i = 0; i < kMaxLen; ++i) {
    s.push(222.2);
  }
  ASSERT_FLOAT_EQ(222.2, averageF(v));
  ASSERT_EQ(kMaxLen, v.size());

  for (int i = 0; i < kMaxLen; ++i) {
    s.push(-0.99372);
  }
  ASSERT_FLOAT_EQ(-0.99372, averageF(v));
  ASSERT_EQ(kMaxLen, v.size());
}

TEST(OldFoldTest, floatToVector) {
  {
    SCOPED_TRACE("fold to vector");
    for (int i = 0; i < 2; ++i) {
      foldVectorTest();
    }
  }
}

#endif /* FOLD_TESTS_H_ */
