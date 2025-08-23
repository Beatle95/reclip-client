#include <QString>
#include <string>
#include <tuple>

#include "gtest/gtest.h"

import ui.host_controller;

using namespace reclip;

TEST(HostControllerTest, SanitizeText) {
  const auto test_cases = {
      // Lines removing.
      std::make_tuple(std::string{""}, QStringLiteral(""), 3, false),
      std::make_tuple(std::string{"1"}, QStringLiteral("1"), 3, false),
      std::make_tuple(std::string{"1\n2\n3"}, QStringLiteral("1\n2\n3"), 3, false),
      std::make_tuple(std::string{"1\n2\n3\n4"}, QStringLiteral("1\n@l\n3\n4"), 3, false),
      std::make_tuple(std::string{"1\n2\n3\n4\n5"}, QStringLiteral("1\n2\n@l\n4\n5"), 4, false),
      // Trimming.
      std::make_tuple(std::string{" "}, QStringLiteral("@t\n"), 4, true),
      std::make_tuple(std::string{"a "}, QStringLiteral("@t\na"), 4, true),
      std::make_tuple(std::string{" a "}, QStringLiteral("@t\na"), 4, true),
      std::make_tuple(std::string{" a\na "}, QStringLiteral("@t\na\na"), 4, true),
      std::make_tuple(std::string{" a\na\n "}, QStringLiteral("@t\na\na"), 4, true),
      std::make_tuple(std::string{"\n\n\na\na\n \n\n"}, QStringLiteral("@t\na\na"), 4, true),
      // Combination.
      std::make_tuple(std::string{" 1\n2\n3\n4\n5\n"}, QStringLiteral("@t\n1\n@l\n4\n5"), 4, true),
      // Edge cases.
      std::make_tuple(std::string{"1\n2\n3\n4\n5"}, QStringLiteral("1\n@l\n5"), 1, true),
      std::make_tuple(std::string{"1\n2\n3\n4\n5"}, QStringLiteral("1\n@l\n5"), 0, true),
      std::make_tuple(std::string{"1\n2\n3\n4\n5"}, QStringLiteral("1\n@l\n5"), -199, true),
  };
  for (const auto& [input, expected, lines_count, trim_enabled] : test_cases) {
    QString result = HostController::SanitizeTextForTesting(input, lines_count, trim_enabled);
    EXPECT_EQ(result, expected) << std::format("Result was: '{}'",
                                               result.replace('\n', "\\n").toStdString());
  }
}
