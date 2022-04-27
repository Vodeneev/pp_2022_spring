// Copyright 2022 VODENEEV MIKHAIL

#include <iostream>
#include "../../../modules/task_3/vodeneev_m_jarvis/jarvis.h"

std::vector<std::pair<double, double>> GetPoints(int left_border, int right_border,
    int low_border, int high_border, size_t size) {

    srand(time(NULL));

    std::vector<std::pair<double, double>> res(size);
    for (size_t i = 0; i < size; i++) {
        res[i].first = left_border + static_cast<double>(std::rand()) / RAND_MAX * (right_border - left_border);
        res[i].second = low_border + static_cast<double>(std::rand()) / RAND_MAX * (high_border - low_border);
    }

    return res;
}

double OrientationPointRelativeToVector(std::pair<double, double> A,
    std::pair<double, double> B, std::pair<double, double> point) {

    return (B.first - A.first) * (point.second - B.second)
        - (B.second - A.second) * (point.first - B.first);
}

std::vector<std::pair<double, double>> JarvisSeq(std::vector<std::pair
    <double, double>> points) {
    if (points.size() < 1)
        throw - 1;

    std::pair<double, double> left_point = points[0];
    int current_index = 0;
    for (size_t i = 0; i < points.size(); i++) {
        if (points[i].first < left_point.first) {
            left_point = points[i];
            current_index = i;
        }
    }

    std::vector<std::pair<double, double>> hull;
    hull.push_back(left_point);

    points.erase(points.begin() + current_index);
    points.push_back(left_point);

    while (true) {
        size_t next_point = 0;
        for (size_t i = 0; i < points.size(); i++) {
            if (OrientationPointRelativeToVector(hull.back(), points[next_point], points[i]) < 0) {
                next_point = i;
            }
        }

        if (points[next_point] == hull[0]) {
            return hull;
        }
        else {
            hull.push_back(points[next_point]);
            points.erase(points.begin() + next_point);
        }
    }
}

class Jarvis {

    const std::vector<std::pair<double, double>>& points, hull;
    int next_point;

public:

    Jarvis(const std::vector<std::pair<double, double>>& _points,
        const std::vector<std::pair<double, double>>& _hull) : points(_points), hull(_hull), next_point(0)
    {
    }

    Jarvis(Jarvis& jarvis, tbb::split)
        : points(jarvis.points),
        hull(jarvis.hull),
        next_point(jarvis.next_point) {}

    void operator()(const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); i++) {
            if (OrientationPointRelativeToVector(hull.back(), points[next_point], points[i]) < 0) {
                this->next_point = i;
            }
        }
    }

    void join(const Jarvis& jarvis) {
        if (OrientationPointRelativeToVector(hull.back(), points[next_point],
            points[jarvis.next_point]) < 0) {
            this->next_point = jarvis.next_point;
        }
    }

    int GetIndexNextPoint() {
        return this->next_point;
    }
};

std::vector<std::pair<double, double>> JarvisTbb(std::vector<std::pair
    <double, double>> points) {
    if (points.size() < 1)
        throw - 1;

    std::pair<double, double> left_point = points[0];
    int current_index = 0;
    for (size_t i = 0; i < points.size(); i++) {
        if (points[i].first < left_point.first) {
            left_point = points[i];
            current_index = i;
        }
    }

    std::vector<std::pair<double, double>> hull;
    hull.push_back(left_point);

    points.erase(points.begin() + current_index);
    points.push_back(left_point);

    while (true) {
        Jarvis jarvis(points, hull);
        tbb::parallel_reduce(tbb::blocked_range<size_t>(0, points.size()), jarvis);

        if (points[jarvis.GetIndexNextPoint()] == hull[0]) {
            return hull;
        }
        else {
            hull.push_back(points[jarvis.GetIndexNextPoint()]);
            points.erase(points.begin() + jarvis.GetIndexNextPoint());
        }
    }
}