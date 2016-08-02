#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> fix_procmap(std::string raw_groups, const std::vector<int>& first_last_procs)
{
    //I didn't know how to do this any easier way, without rewriting other stuff myself....
    std::vector<std::string> group_parsed;

    int prev = -1;
    size_t pos = 0;
    while (pos != std::string::npos)
    {

        pos = raw_groups.find(' ', pos + 1);

        if(!std::isspace(raw_groups[prev + 1]))
        {
            std::cout << "Emplacing on the back!\n";
            group_parsed.emplace_back(raw_groups.begin() + prev + 1, pos == std::string::npos ? raw_groups.end() : raw_groups.begin() + pos);
        }

        prev = pos;
    }

    for(int i = 0; i < group_parsed.size(); ++i)
        std::cout << group_parsed[i] << "\n";

    return group_parsed;

}


int main(int argc, char** argv)
{
    std::string test = "group1  group2  group3=4    group5=9";

    std::vector<int> not_imp(2,0);

    std::vector<std::string> final_ans(fix_procmap(test, not_imp));

    for(int i = 0; i < final_ans.size(); ++i)
    {
        std::cout << final_ans[i] << "\n";
    }
    

    std::cout << "Finished!\n";


    return 0;
}
