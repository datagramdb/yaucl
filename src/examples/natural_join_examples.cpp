//
// Created by giacomo on 07/01/23.
//

#include <yaucl/structures/RelationalTables.h>

int main(void) {
    std::vector<std::vector<size_t>> vv1{{1,2,3,10,11},{4,5,6,12,13},{7,8,9,14,15}};
    std::vector<std::string> S1{"a","b","c"};
    auto t1 = std::make_shared<ConcreteEquiJoinTable<std::string, size_t>>(S1, vv1);
//    ConcreteEquiJoinTable<std::string, size_t> t1{S1, vv1};

    std::vector<std::vector<size_t>> vv2{{11,12,13},{14,15,16},{17,18,19}};
    std::vector<std::string> S2{"d","e","f"};
    auto t2 = std::make_shared<ConcreteEquiJoinTable<std::string, size_t>>(S2, vv2);
    auto t2_ = std::make_shared<ConcreteEquiJoinTable<std::string, size_t>>(S1, vv2);
    auto CPT = std::make_shared<CrossProductTable<std::string, size_t>>(t1, t2);

    std::vector<std::vector<size_t>> vv3{{1,2},{11,12,13},{},{21,22,23,24}};
    std::vector<std::string> S3{"a","b","c","d"};
    std::vector<std::string> S4{"a","x","y","z"};
    auto t3 = std::make_shared<TableColumnCombinations<std::string, size_t>>(S3, vv3);
    auto t3_ = std::make_shared<TableColumnCombinations<std::string, size_t>>(S4, vv3);

    std::vector<std::shared_ptr<AbstractEquiJoinTable<std::string,size_t>>> W;
    W.emplace_back(t1);
    W.emplace_back(t2);
    auto tu = std::make_shared<TableUnion<std::string, size_t>>(W);

    auto ext = std::make_shared<ExtendTableWithData<std::string, size_t>>(t1, std::vector<std::string>{"j"}, std::vector<size_t>{208});

    auto projs = std::make_shared<ProjectAndSelectTable<std::string, size_t>>(t1, std::vector<size_t>{0,1,3,4}, std::vector<std::string>{"a","c"});

    auto elements = natural_join<std::string, size_t>(t1, t3_);
    for (size_t i = 0, N = elements->size(); i<N; i++) {
        std::cout << *elements->getRow(i) << std::endl;
    }
}