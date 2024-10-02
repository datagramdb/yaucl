#ifndef TWIGY_CORE_UTILS_H_
#define TWIGY_CORE_UTILS_H_

#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include <variant>

using union_minimal = std::variant<std::string,double>;

struct SampleData {
    int sample_number;
    union_minimal current_feature_value;
};

class table {
public:
    std::vector<std::vector<std::pair<std::string,union_minimal>>> data_;
    std::vector<std::string> headers_;
    std::vector<int> col_widths_;

    void AddRow(const std::vector<std::pair<std::string,union_minimal>>& row);

    void SetHeaders(std::vector<std::string> h);

    void Print(size_t num_rows);
};

table ReadCsv(const std::string &input_path);

std::vector<int> GenerateRandomIntVector(int a, int b, int n_samples,
                                         std::mt19937 &gen);

int ArgMax(std::vector<double> &vector);
int ArgMax(std::vector<int> &vector);

void Split_X_y(table &data_frame, std::vector<std::vector<std::pair<std::string,union_minimal>>> &X,
               std::vector<int> &y, std::string feature);

void CheckNegativeLabels(const std::vector<int> *label_data);

#endif  // TWIGY_CORE_UTILS_H_
