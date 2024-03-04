#ifndef COLOR_H
#define COLOR_H

class Color {
public:
  Color() = default;
  Color(int r, int g, int b);
  int Red() const;
  int Green() const;
  int Blue() const;

private:
  static constexpr int kColorValueMin = 0;
  static constexpr int kColorValueMax = 255;
  int red_ = 0;
  int green_ = 0;
  int blue_ = 0;
};

bool operator==(const Color& rhs, const Color& lhs);

#endif