#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <map>
#include <set>

std::vector<std::string> splitString(const std::string& inputString, const char delimiter = ' ') {
    std::vector<std::string> strTokens;
    std::size_t startPos = 0;
    std::size_t endPos = inputString.find(delimiter);

    while (endPos != std::string::npos) {
        strTokens.push_back(inputString.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
        endPos = inputString.find(delimiter, startPos);
    }

    strTokens.push_back(inputString.substr(startPos));
    return strTokens;
}

struct WayPoint {
    WayPoint(const int x, const int y, const int z, const int penalty)
        : x_(x), y_(y), z_(z), penalty_(penalty) {

        }
    const int x_;
    const int y_;
    const int z_;
    const int penalty_;
};

class FindShortest {
private:
    double shortestTime_ = std::numeric_limits<double>::infinity();

    //      <     tuple<x,   y,   z>,   min Cost>
    std::map<std::tuple<int, int, int>, double> minCostMap_;

    void dfsVisit(const int x, const int y, const int z, const std::vector<WayPoint>& points,
        const int index, const double cost) {

        if (x == 100 && y == 100) {
            shortestTime_ = std::min(shortestTime_, cost);
            return;
        }

        const auto it = minCostMap_.find(std::make_tuple(x, y, z));
        if (it != minCostMap_.end()) {
            // std::cout << "Found x=" << x << " y=" <<y << " value=" << it->second << std::endl;
            shortestTime_ = std::min(shortestTime_, cost + it->second);
            return;
        }

        if (index == points.size()) {
            std::cout << "WARN: Cannot reach the destination" << std::endl;
            return;
        }

        // std::cout<<"At x=" << x << " y=" << y << " cost=" << cost <<std::endl;
        if (!(points[index].x_ == 100 && points[index].y_ == 100)) { //Skip the points[index]
            dfsVisit(x, y, z, points, index + 1, cost + points[index].penalty_);
        }
        
        //Go to the points[index]
        const int newX = points[index].x_;
        const int newY = points[index].y_;
        const int newZ = points[index].z_;
        const double travleCost = 
            std::sqrt(
                ((newX - x) * (newX - x) + (newY - y) * (newY - y))
            ) / SPEED_;

        dfsVisit(newX, newY, newZ, points, index + 1, cost + travleCost + STOP_COST_);
    }

public:
    static const double STOP_COST_;
    static const double SPEED_;

    double findShortest(const std::vector<WayPoint>& points) {
        shortestTime_ = std::numeric_limits<double>::infinity();
        minCostMap_.clear();

        for (int i = points.size() - 1; i >= 0; --i) {
            //start from the last points. Record the smallest time in minCostMap_
            dfsVisit(points[i].x_, points[i].y_, points[i].z_, points, i+1, 0.0);

            minCostMap_[std::make_tuple(points[i].x_, points[i].y_, points[i].z_)] = shortestTime_;
            // std::cout << points[i].x_ << ", " << points[i].y_ << " = " << shortestTime_ << std::endl;
            shortestTime_ = std::numeric_limits<double>::infinity();
        }

        dfsVisit(0, 0, 0, points, 0, 0.0);
        // std::cout << "0, 0 = " << shortestTime_ << std::endl;
        return std::round(shortestTime_ * 1000.0) / 1000.0; // rounded to 3 decimal places
    }
};

const double FindShortest::STOP_COST_ = 10.0;//10 seconds
const double FindShortest::SPEED_ = 2.0;//2 m/s

int main() {
    int N = -1;
    std::vector<WayPoint> points;
    FindShortest findSh;

    std::cout << std::fixed;
    std::cout << std::setprecision(3);

    //      <         <x,   y>,   appear time>
    std::map<std::pair<int, int>, int> pointZMap;

    for (std::string line; std::getline(std::cin, line);) {
        // std::cout << "Get a line: " << line << " : ";
        const std::vector<std::string> strTokens = splitString(line);
        // for (const auto& t : strTokens) {
        //     std::cout << "," << t;
        // }
        // std::cout << std::endl;

        if (strTokens.size() == 1) {//start a new test
            if (N > 0) {
                if (N != points.size()) {
                    std::cout << "Error: the waypoint size is different from N" << std::endl;
                } else {
                    if (points.back().x_ != 100 || points.back().y_ != 100){
                        points.emplace_back(100, 100, 0, FindShortest::STOP_COST_);// push the destination.
                    }
                    std::cout << findSh.findShortest(points) << std::endl;
                }
            }
            
            points.clear();
            pointZMap.clear();

            N = std::stoi(strTokens[0]);//let it throw, if it cannot parse
            if (N == 0) {
                // std::cout << "Reach the end of the test" << std::endl;
                break;
            }
        } else if (strTokens.size() == 3) {
            const int x = std::stoi(strTokens[0]);
            const int y = std::stoi(strTokens[1]);
            const int penalty = std::stoi(strTokens[2]);

            const std::pair<int, int> mapKey = std::make_pair(x, y);

            auto it = pointZMap.find(mapKey);
            if (it == pointZMap.end()) {
                // if the point in a 2-D coordinate appears the first time, add a Z-axis as 0.
                // Then it becomes 3-D coordinate
                pointZMap[mapKey] = 0;
                points.emplace_back(x, y, 0, penalty);
            } else {
                // std::cout<< "WARNING "<< x << " " << y << " is duplcated\n";
                // if the point in a 2-D coordinate appears more than once, increase its Z-axis
                // Then it becomes 3-D coordinate
                ++pointZMap[mapKey];
                points.emplace_back(x, y, pointZMap[mapKey], penalty);
            }
        } else {
            std::cout << " WARNING: Invalid line: " << line << std::endl;
        }
    }


    return 0;
}