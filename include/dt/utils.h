/*Copyright (c) 2020 Christoph Meyer

 https://github.com/christophmeyer/twigy/blob/master/src/core/utils.h

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
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


#include <limits>
#include <set>


static inline
std::vector<int> getOnLocations(int a) {
    std::vector<int> result;
    int place = 0;
    while (a != 0) {
        if (a & 1) {
            result.push_back(place);
        }
        ++place;
        a >>= 1;
    }
    return result;
}

#include <cmath>
#include <unordered_set>

template<typename T>
std::vector<std::unordered_set<T> > powerSet(const std::vector<T>& set, size_t maxSize) {
    std::vector<std::unordered_set<T> > result;
    int numPowerSets = static_cast<int>(std::pow(2.0, static_cast<double>(set.size())));
    for (size_t i = 0; i < numPowerSets; ++i) {
        std::vector<int> onLocations = getOnLocations(i);
        if (onLocations.size() > maxSize) continue;
        if (onLocations.empty()) continue;
        std::unordered_set<T> subSet;
        for (size_t j = 0; j < onLocations.size(); ++j) {
            subSet.insert(set.at(onLocations.at(j)));
        }
        result.push_back(subSet);
    }
    return result;
}


/*def sigmoid(z):
# because that -z is too big will arise runtimeWarning in np.exp()
if isinstance(z, float) and (z < -500):
z = -500
elif not (isinstance(z, float)):
z[z < -500] = (-500) * np.ones(sum(z < -500))

return 1 / (np.exp(- z) + 1)*/


#include <ranges>
#include <vector>
#include <algorithm>
#include <random>

template <typename T>
void Sample(size_t first, size_t last, const std::vector<T>& in, std::vector<T>& out, std::size_t n, std::mt19937& g)
{
    first = std::min(first, in.size());
    last = std::min(last, in.size());
    std::vector<size_t> curr;
    curr.reserve(n);
    std::ranges::sample(
            std::views::iota(first, last), std::back_inserter(curr), n, g);
    for (const auto& idx : curr) {
        out.emplace_back(in.at(idx));
    }
}

#endif  // TWIGY_CORE_UTILS_H_
